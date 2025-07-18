#include "Prefetcher.h"

#include <vector>
#include <mutex>
#include <atomic>
#ifdef INTEL_INTRINSICS
#include <mmintrin.h>
#endif
#include "avisynth.h"
#include "ThreadPool.h"
#include "ObjectPool.h"
#include "LruCache.h"
#include "InternalEnvironment.h"
#include "internal.h"

struct PrefetcherJobParams
{
  int frame;
  Prefetcher* prefetcher;
  LruCache<size_t, PVideoFrame>::handle cache_handle;
};

struct PrefetcherPimpl
{
  PClip child;
  VideoInfo vi;

  // The number of threads to use for prefetching
  const int nThreads;

  // Maximum number of frames to prefetch
  const int nPrefetchFrames;

  ThreadPool* thread_pool;

  ObjectPool<PrefetcherJobParams> JobParamsPool;
  std::mutex params_pool_mutex;

  // Contains the pattern we are locked on to
  int LockedPattern;

  // The number of consecutive frames Pattern has repeated itself
  int PatternHits;

  // The number of consecutive frames LockedPattern was invalid
  int PatternMisses;

  // The current pattern that we are not locked on to
  int Pattern;

  // True if we have found a pattern to lock onto
  bool IsLocked;

  // The frame number that GetFrame() has been called with the last time
  int LastRequestedFrame;

  std::shared_ptr<LruCache<size_t, PVideoFrame> > VideoCache;
  std::atomic<int> running_workers;
  std::mutex worker_exception_mutex;
  std::exception_ptr worker_exception;
  bool worker_exception_present;
  InternalEnvironment *EnvI;

  PrefetcherPimpl(const PClip& _child, int _nThreads, int _nPrefetchFrames, IScriptEnvironment2 *env2) :
    child(_child),
    vi(_child->GetVideoInfo()),
    nThreads(_nThreads),
    nPrefetchFrames(_nPrefetchFrames),
    thread_pool(NULL),
    LockedPattern(1),
    PatternHits(0),
    PatternMisses(0),
    Pattern(1),
    IsLocked(false),
    LastRequestedFrame(0),
    VideoCache(NULL),
    running_workers(0),
    worker_exception_present(0),
    EnvI(static_cast<InternalEnvironment*>(env2))
  {
		thread_pool = EnvI->NewThreadPool(nThreads);
  }

  ~PrefetcherPimpl()
  {
		for (void* data : thread_pool->Finish()) {
			PrefetcherJobParams *ptr = (PrefetcherJobParams*)data;
			VideoCache->rollback(&ptr->cache_handle);
		}
  }
};


// The number of intervals a pattern has to repeat itself to become (un)locked
#define PATTERN_LOCK_LENGTH 3

AVSValue Prefetcher::ThreadWorker(IScriptEnvironment2* env, void* data)
{
  PrefetcherJobParams *ptr = (PrefetcherJobParams*)data;
  Prefetcher *prefetcher = ptr->prefetcher;
  int n = ptr->frame;
  LruCache<size_t, PVideoFrame>::handle cache_handle = ptr->cache_handle;

  {
    std::lock_guard<std::mutex> lock(prefetcher->_pimpl->params_pool_mutex);
    prefetcher->_pimpl->JobParamsPool.Destruct(ptr);
  }

  try
  {
    cache_handle.first->value = prefetcher->_pimpl->child->GetFrame(n, env);
#ifdef INTEL_INTRINSICS
    #ifdef X86_32
    _mm_empty();
    #endif
#endif

    prefetcher->_pimpl->VideoCache->commit_value(&cache_handle);
    --(prefetcher->_pimpl->running_workers);
  }
  catch(...)
  {
    prefetcher->_pimpl->VideoCache->rollback(&cache_handle);

    std::lock_guard<std::mutex> lock(prefetcher->_pimpl->worker_exception_mutex);
    prefetcher->_pimpl->worker_exception = std::current_exception();
    prefetcher->_pimpl->worker_exception_present = true;
    --(prefetcher->_pimpl->running_workers);
  }

  return AVSValue();
}

Prefetcher::Prefetcher(const PClip& _child, int _nThreads, int _nPrefetchFrames, IScriptEnvironment *env) :
  _pimpl(NULL)
{
  _pimpl = new PrefetcherPimpl(_child, _nThreads, _nPrefetchFrames, static_cast<IScriptEnvironment2*>(env));
  _pimpl->VideoCache = std::make_shared<LruCache<size_t, PVideoFrame> >(_pimpl->nPrefetchFrames*2, CACHE_NO_RESIZE);
}

void Prefetcher::Destroy()
{
  if (_pimpl)
  {
    PrefetcherPimpl *pimpl = _pimpl;
    delete pimpl;
		_pimpl = nullptr;
  }
}

Prefetcher::~Prefetcher()
{
  Destroy();
}

size_t Prefetcher::NumPrefetchThreads() const
{
  return _pimpl->nThreads;
}

int __stdcall Prefetcher::SchedulePrefetch(int current_n, int prefetch_start, InternalEnvironment* env)
{
  int n = prefetch_start;
  while ((_pimpl->running_workers < _pimpl->nPrefetchFrames) && (std::abs(n - current_n) < _pimpl->nPrefetchFrames) )
  {
    n += _pimpl->IsLocked ? _pimpl->LockedPattern : 1;
    if (n >= _pimpl->vi.num_frames)
      break;

    PVideoFrame result;
    LruCache<size_t, PVideoFrame>::handle cache_handle;
    switch(_pimpl->VideoCache->lookup(n, &cache_handle, false, result))
    {
    case LRU_LOOKUP_NOT_FOUND:
      {
        PrefetcherJobParams *p = NULL;
        {
          std::lock_guard<std::mutex> lock(_pimpl->params_pool_mutex);
          p = _pimpl->JobParamsPool.Construct();
        }
        p->frame = n;
        p->prefetcher = this;
        p->cache_handle = cache_handle;
        ++_pimpl->running_workers;
        _pimpl->thread_pool->QueueJob(ThreadWorker, p, env, NULL);
        break;
      }
    case LRU_LOOKUP_FOUND_AND_READY:      // Fall-through intentional
    case LRU_LOOKUP_NO_CACHE:             // Fall-through intentional
    case LRU_LOOKUP_FOUND_BUT_NOTAVAIL:
      {
        break;
      }
    default:
      {
        assert(0);
        break;
      }
    }
  } // switch

  return n;
}

PVideoFrame __stdcall Prefetcher::GetFrame(int n, IScriptEnvironment* env_)
{
  InternalEnvironment* IEnv = GetAndRevealCamouflagedEnv(env_);
  IScriptEnvironment* env = static_cast<IScriptEnvironment*>(IEnv);

  if (IEnv->GetSuppressThreadCount() > 0) {
    // do not use thread when invoke running
    return _pimpl->child->GetFrame(n, env);
  }

  int pattern = n - _pimpl->LastRequestedFrame;
  _pimpl->LastRequestedFrame = n;
  if (pattern == 0)
    pattern = 1;

  if (_pimpl->IsLocked)
  {
    if (_pimpl->LockedPattern == pattern)
    {
      _pimpl->PatternHits = 0;    // Tracks Pattern
      _pimpl->PatternMisses = 0;  // Tracks LockedPattern
    }
    else if (_pimpl->Pattern == pattern)
    {
      _pimpl->PatternHits++;    // Tracks Pattern
      _pimpl->PatternMisses++;  // Tracks LockedPattern
    }
    else
    {
      _pimpl->PatternHits = 0;  // Tracks Pattern
      _pimpl->PatternMisses++;  // Tracks LockedPattern
    }
    _pimpl->Pattern = pattern;

    if ((_pimpl->PatternMisses >= PATTERN_LOCK_LENGTH) && (_pimpl->PatternHits >= PATTERN_LOCK_LENGTH))
    {
      _pimpl->LockedPattern = _pimpl->Pattern;
      _pimpl->PatternHits = 0;    // Tracks Pattern
      _pimpl->PatternMisses = 0;  // Tracks LockedPattern
    }
    else if ((_pimpl->PatternMisses >= PATTERN_LOCK_LENGTH) && (_pimpl->PatternHits < PATTERN_LOCK_LENGTH))
    {
      _pimpl->IsLocked = false;
    }
  }
  else
  {
    if (_pimpl->Pattern == pattern)
    {
      _pimpl->PatternHits++;      // Tracks Pattern
      _pimpl->PatternMisses = 0;  // Tracks Pattern
    }
    else
    {
      _pimpl->PatternHits = 0;    // Tracks Pattern
      _pimpl->PatternMisses++;    // Tracks Pattern
    }

    if (_pimpl->PatternHits >= PATTERN_LOCK_LENGTH)
    {
      _pimpl->LockedPattern = pattern;
      _pimpl->PatternMisses = 0;  // Tracks Pattern
      _pimpl->IsLocked = true;
    }
  }


  {
    std::lock_guard<std::mutex> lock(_pimpl->worker_exception_mutex);
    if (_pimpl->worker_exception_present)
    {
      std::rethrow_exception(_pimpl->worker_exception);
    }
  }


  // Prefetch 1
  int prefetch_pos = SchedulePrefetch(n, n, IEnv);

  // Get requested frame
  PVideoFrame result;
  LruCache<size_t, PVideoFrame>::handle cache_handle;
  // fill result if LRU_LOOKUP_FOUND_AND_READY
  switch(_pimpl->VideoCache->lookup(n, &cache_handle, true, result))
  {
  case LRU_LOOKUP_NOT_FOUND:
    {
      try
      {
        result = _pimpl->child->GetFrame(n, env); // P.F. fill result before Commit!
        cache_handle.first->value = result;
        // cache_handle.first->value = _pimpl->child->GetFrame(n, env); // P.F. before Commit!
#ifdef INTEL_INTRINSICS
  #ifdef X86_32
        _mm_empty();
  #endif
#endif
        _pimpl->VideoCache->commit_value(&cache_handle);
      }
      catch(...)
      {
        _pimpl->VideoCache->rollback(&cache_handle);
        throw;
      }
      break;
    }
  case LRU_LOOKUP_FOUND_AND_READY:
    {
    //result = cache_handle.first->value; // old method, result is filled already
    break;
    }
  case LRU_LOOKUP_NO_CACHE:
    {
      result = _pimpl->child->GetFrame(n, env);
      break;
    }
  case LRU_LOOKUP_FOUND_BUT_NOTAVAIL:    // Fall-through intentional
  default:
    {
      assert(0);
      break;
    }
  }

  // Prefetch 2
  SchedulePrefetch(n, prefetch_pos, IEnv);

  return result;
}

bool __stdcall Prefetcher::GetParity(int n)
{
  return _pimpl->child->GetParity(n);
}

void __stdcall Prefetcher::GetAudio(void* buf, int64_t start, int64_t count, IScriptEnvironment* env_)
{
  InternalEnvironment* IEnv = GetAndRevealCamouflagedEnv(env_);
  IScriptEnvironment* env = static_cast<IScriptEnvironment*>(IEnv);

  _pimpl->child->GetAudio(buf, start, count, env);
}

int __stdcall Prefetcher::SetCacheHints(int cachehints, int frame_range)
{
  AVS_UNUSED(frame_range);
  if (CACHE_GET_MTMODE == cachehints)
    return MT_NICE_FILTER;

  if (CACHE_GET_DEV_TYPE == cachehints)
    return (_pimpl->child->GetVersion() >= 5) ? _pimpl->child->SetCacheHints(CACHE_GET_DEV_TYPE, 0) : 0;

  return 0;
}

const VideoInfo& __stdcall Prefetcher::GetVideoInfo()
{
  return _pimpl->vi;
}

AVSValue Prefetcher::Create(AVSValue args, void*, IScriptEnvironment* env)
{
  InternalEnvironment *envi = static_cast<InternalEnvironment*>(env);
  PClip child = args[0].AsClip();

  int PrefetchThreads = args[1].AsInt((int)envi->GetEnvProperty(AEP_PHYSICAL_CPUS)+1);
  int PrefetchFrames = args[2].AsInt(PrefetchThreads * 2);

  if (PrefetchThreads > 0 && PrefetchFrames > 0)
  {
    return new Prefetcher(child, PrefetchThreads, PrefetchFrames, env);
  }
  else
    return child;
}
