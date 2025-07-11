#ifndef _AVS_THREADPOOL_H
#define _AVS_THREADPOOL_H

#include "avisynth.h"
#include <future>
#include <vector>
#if HAVE_RESAMPLEMT || HAVE_NNEDI3
#include <windows.h>
#include "plugins/ResampleMT/ThreadPoolDef.h"
#endif

typedef std::future<AVSValue> AVSFuture;
typedef std::promise<AVSValue> AVSPromise;

class InternalEnvironment;

class JobCompletion : public IJobCompletion
{
private:
  const size_t max_jobs;
  size_t nJobs;

public:
  typedef std::pair<AVSPromise, AVSFuture> PromFutPair;
  PromFutPair *pairs;

  JobCompletion(size_t _max_jobs) :
    max_jobs(_max_jobs),
    nJobs(0),
    pairs(NULL)
  {
    pairs = new PromFutPair[max_jobs];

    // Initialize for first use
    nJobs = max_jobs;
    Reset();
  }

  AVSPromise* Add()
  {
    if (nJobs == max_jobs)
      throw AvisynthError("The completion object is already full.");

    AVSPromise* ret = &(pairs[nJobs].first);
    ++nJobs;
    return ret;
  }

  virtual ~JobCompletion()
  {
    Wait();
    delete [] pairs;
  }

  void __stdcall Wait()
  {
    for (size_t i = 0; i < nJobs; ++i)
      pairs[i].second.wait();
  }
  size_t __stdcall Size() const
  {
    return nJobs;
  }
  size_t __stdcall Capacity() const
  {
    return max_jobs;
  }
  AVSValue __stdcall Get(size_t i)
  {
    return pairs[i].second.get();
  }
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpessimizing-move"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
#elif defined(_MSC_VER)
  // Add MSVC-specific pragma if needed
#endif
  void __stdcall Reset()
  {
    for (size_t i = 0; i < nJobs; ++i)
    {
      // To be on the safe side, keep std::move to ensure efficient move semantics,
      // even if copy elision is possible (despite compiler warnings).
      // AVSValue does not have a move constructor or move assignment operator.
      // It only has a copy constructor and copy assignment operator. This means that
      // AVSValue objects will be copied rather than moved, which can be less efficient.
      pairs[i].first = std::move(AVSPromise());
      pairs[i].second = std::move(pairs[i].first.get_future());
    }
    nJobs = 0;
  }
#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
  // Add MSVC-specific pragma if needed
#endif
  void __stdcall Destroy()
  {
    delete this;
  }
};

#if HAVE_RESAMPLEMT || HAVE_NNEDI3
#define MAX_PHYSICAL_CORES 64

typedef struct _MT_Data_Thread
{
	Public_MT_Data_Thread *MTData;
	uint8_t f_process,thread_Id;
	HANDLE nextJob,jobFinished;
} MT_Data_Thread;

typedef struct _Arch_CPU
{
	uint8_t NbPhysCore,NbLogicCPU;
	uint8_t NbHT[MAX_PHYSICAL_CORES];
	ULONG_PTR ProcMask[MAX_PHYSICAL_CORES];
	ULONG_PTR FullMask;
} Arch_CPU;
#endif

class ThreadPoolPimpl;
class ThreadPool
{
public:
#if HAVE_RESAMPLEMT || HAVE_NNEDI3
  ThreadPool(void);
  //virtual ~ThreadPool();
#endif
  ThreadPool(size_t nThreads, size_t nStartId, InternalEnvironment* env);
  ~ThreadPool();
//#endif

#if HAVE_RESAMPLEMT || HAVE_NNEDI3
protected :
	Arch_CPU CPU;

public:
	uint8_t GetThreadNumber(uint8_t thread_number,bool logical);
	bool AllocateThreads(uint8_t thread_number,uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,
		bool SetAffinity,bool sleep,ThreadLevelName priority);
	bool AllocateThreads(uint8_t thread_number,uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,
		bool SetAffinity,bool sleep)
		{return(AllocateThreads(thread_number,offset_core,offset_ht,UseMaxPhysCore,SetAffinity,sleep,NormalThreadLevel));}
	bool DeAllocateThreads(void);
	bool ChangeThreadsAffinity(uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,bool SetAffinity);
	bool ChangeThreadsLevel(ThreadLevelName priority);
	bool RequestThreadPool(uint8_t thread_number,Public_MT_Data_Thread *Data,ThreadLevelName priority);
	bool RequestThreadPool(uint8_t thread_number,Public_MT_Data_Thread *Data)
		{return(RequestThreadPool(thread_number,Data,NoneThreadLevel));}
	bool ReleaseThreadPool(bool sleep);
	bool StartThreads(void);
	bool WaitThreadsEnd(void);
	bool GetThreadPoolStatus(void) {return(Status_Ok);}
	uint8_t GetCurrentThreadAllocated(void) {return(CurrentThreadsAllocated);}
	uint8_t GetCurrentThreadUsed(void) {return(CurrentThreadsUsed);}
	uint8_t GetLogicalCPUNumber(void) {return(CPU.NbLogicCPU);}
	uint8_t GetPhysicalCoreNumber(void) {return(CPU.NbPhysCore);}

protected :

	MT_Data_Thread MT_Thread[MAX_MT_THREADS];
	HANDLE nextJob[MAX_MT_THREADS],jobFinished[MAX_MT_THREADS];
	HANDLE thds[MAX_MT_THREADS];
	DWORD tids[MAX_MT_THREADS];
	ULONG_PTR ThreadMask[MAX_MT_THREADS];
	bool ThreadSleep[MAX_MT_THREADS];
	ThreadLevelName nPriority;

	bool Status_Ok;
	uint8_t TotalThreadsRequested,CurrentThreadsAllocated,CurrentThreadsUsed;

	void FreeThreadPool(void);
	void DestroyThreadPool(void);
	void CreateThreadPool(uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,bool SetAffinity,
		bool sleep,ThreadLevelName priority);

private :
	static DWORD WINAPI StaticThreadpool(LPVOID lpParam);
	ThreadPool (const ThreadPool &other);
	ThreadPool& operator = (const ThreadPool &other);
	bool operator == (const ThreadPool &other) const;
	bool operator != (const ThreadPool &other) const;
#endif

public:
  void QueueJob(ThreadWorkerFuncPtr clb, void* params, InternalEnvironment* env, JobCompletion* tc);
  size_t NumThreads() const;

private:
  ThreadPoolPimpl* const _pimpl;
  static void ThreadFunc(size_t thread_id, ThreadPoolPimpl* const _pimpl, InternalEnvironment* env);

public:
  std::vector<void*> Finish();
  void Join();
};


#endif  // _AVS_THREADPOOL_H
