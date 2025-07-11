#include "ThreadPool.h"
#include "internal.h"
#include <cassert>
#include <thread>

struct ThreadPoolGenericItemData
{
  ThreadWorkerFuncPtr Func;
  void* Params;
  AVSPromise* Promise;
  Device* device;
};

#include "mpmc_bounded_queue.h"
typedef mpmc_bounded_queue<ThreadPoolGenericItemData> MessageQueue;

class ThreadPoolPimpl
{
public:
  std::vector<std::thread> Threads;
  MessageQueue MsgQueue;
  std::mutex Mutex;
  std::condition_variable FinishCond;
  size_t NumRunning;

  ThreadPoolPimpl(size_t nThreads) :
    Threads(),
    MsgQueue(nThreads * 6)
  {}
};

void ThreadPool::ThreadFunc(size_t thread_id, ThreadPoolPimpl * const _pimpl, InternalEnvironment* env)
{
  auto EnvTLS = env->NewThreadScriptEnvironment((int)thread_id);
  PInternalEnvironment holder = PInternalEnvironment(EnvTLS);

  while (true)
  {
    ThreadPoolGenericItemData data;
    if (_pimpl->MsgQueue.pop_back(&data) == false) {
      // threadpool is canceled
      std::unique_lock<std::mutex> lock(_pimpl->Mutex);
      if (--_pimpl->NumRunning == 0) {
        _pimpl->FinishCond.notify_all();
      }
      return;
    }

    EnvTLS->SetCurrentDevice(data.device);
    EnvTLS->GetSupressCaching() = false;
    if (data.Promise != NULL)
    {
      try
      {
        data.Promise->set_value(data.Func(EnvTLS, data.Params));
      }
      catch (const AvisynthError&)
      {
        data.Promise->set_exception(std::current_exception());
      }
      catch (const std::exception&)
      {
        data.Promise->set_exception(std::current_exception());
      }
      catch (...)
      {
        data.Promise->set_exception(std::current_exception());
        //data.Promise->set_value(AVSValue("An unknown exception was thrown in the thread pool."));
      }
    }
    else
    {
      try
      {
        data.Func(EnvTLS, data.Params);
      }
      catch (...) {}
    }
  } //while
}

ThreadPool::ThreadPool(size_t nThreads, size_t nStartId, InternalEnvironment* env) :
  _pimpl(new ThreadPoolPimpl(nThreads))
{
  _pimpl->Threads.reserve(nThreads);

  std::unique_lock<std::mutex> lock(_pimpl->Mutex);

  // i is used as the thread id. Skip id zero because that is reserved for the main thread.
  // CUDA: thread id is controled by caller
  for (size_t i = 0; i < nThreads; ++i)
    _pimpl->Threads.emplace_back(ThreadFunc, i + nStartId, _pimpl, env);

  _pimpl->NumRunning = nThreads;
}

void ThreadPool::QueueJob(ThreadWorkerFuncPtr clb, void* params, InternalEnvironment* env, JobCompletion* tc)
{
  ThreadPoolGenericItemData itemData;
  itemData.Func = clb;
  itemData.Params = params;
  itemData.device = env->GetCurrentDevice();

  if (tc != NULL)
    itemData.Promise = tc->Add();
  else
    itemData.Promise = NULL;

  if (_pimpl->MsgQueue.push_front(std::move(itemData)) == false) {
    throw AvisynthError("Threadpool is cancelled");
  }
}

size_t ThreadPool::NumThreads() const
{
  return _pimpl->Threads.size();
}

std::vector<void*> ThreadPool::Finish()
{
  std::unique_lock<std::mutex> lock(_pimpl->Mutex);
  if (_pimpl->NumRunning > 0) {
    _pimpl->MsgQueue.finish();
    while (_pimpl->NumRunning > 0)
    {
      _pimpl->FinishCond.wait(lock);
    }
    std::vector<void*> ret;
    ThreadPoolGenericItemData item;
    while (_pimpl->MsgQueue.pop_remain(&item)) {
      ret.push_back(item.Params);
    }
    return ret;
  }
  return std::vector<void*>();
}

void ThreadPool::Join()
{
  if (_pimpl->Threads.size() > 0) {
    Finish();
    for (size_t i = 0; i < _pimpl->Threads.size(); ++i)
    {
      if (_pimpl->Threads[i].joinable())
        _pimpl->Threads[i].join();
    }
    _pimpl->Threads.clear();
  }
}

#if HAVE_RESAMPLEMT || HAVE_NNEDI3
static const int TabThreadLevel[8]={THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,
	THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,
	THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};

static uint8_t CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    uint8_t bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}

#define myfree(ptr) if (ptr!=NULL) { free(ptr); ptr=NULL;}
static void Get_CPU_Info(Arch_CPU& cpu)
{
    bool done = false;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer=NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr=NULL;
    DWORD returnLength=0;
    uint8_t logicalProcessorCount=0;
    uint8_t processorCoreCount=0;
    DWORD byteOffset=0;

	cpu.NbLogicCPU=0;
	cpu.NbPhysCore=0;
	cpu.FullMask=0;

    while (!done)
    {
        BOOL rc=GetLogicalProcessorInformation(buffer, &returnLength);

        if (rc==FALSE)
        {
            if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
            {
                myfree(buffer);
                buffer=(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);

                if (buffer==NULL) return;
            }
            else
			{
				myfree(buffer);
				return;
			}
        }
        else done=true;
    }

    ptr=buffer;

    while ((processorCoreCount<MAX_PHYSICAL_CORES) && ((byteOffset+sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION))<=returnLength))
    {
        switch (ptr->Relationship)
        {
			case RelationProcessorCore :
	            // A hyperthreaded core supplies more than one logical processor.
				cpu.NbHT[processorCoreCount]=CountSetBits(ptr->ProcessorMask);
		        logicalProcessorCount+=cpu.NbHT[processorCoreCount];
				cpu.ProcMask[processorCoreCount++]=ptr->ProcessorMask;
				cpu.FullMask|=ptr->ProcessorMask;
			    break;
			default : break;
        }
        byteOffset+=sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }
	free(buffer);

	cpu.NbPhysCore=processorCoreCount;
	cpu.NbLogicCPU=logicalProcessorCount;
}

static ULONG_PTR GetCPUMask(ULONG_PTR bitMask, uint8_t CPU_Nb)
{
    uint8_t LSHIFT=sizeof(ULONG_PTR)*8-1;
    uint8_t i=0,bitSetCount=0;
    ULONG_PTR bitTest=1;

	CPU_Nb++;
	while (i<=LSHIFT)
	{
		if ((bitMask & bitTest)!=0) bitSetCount++;
		if (bitSetCount==CPU_Nb) return(bitTest);
		else
		{
			i++;
			bitTest<<=1;
		}
	}
	return(0);
}

static void CreateThreadsMasks(Arch_CPU cpu, ULONG_PTR *TabMask,uint8_t NbThread,uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore)
{
	if (NbThread==0) return;

	memset(TabMask,0,NbThread*sizeof(ULONG_PTR));

	if ((cpu.NbLogicCPU==0) || (cpu.NbPhysCore==0)) return;

	uint8_t i_cpu=offset_core%cpu.NbPhysCore;
	uint8_t i_ht=offset_ht%cpu.NbHT[i_cpu];
	uint8_t current_thread=0,nb_cpu=0;

	if (cpu.NbPhysCore==cpu.NbLogicCPU)
	{
		while (NbThread>current_thread)
		{
			uint8_t Nb_Core_Th=NbThread/cpu.NbPhysCore+( ((NbThread%cpu.NbPhysCore)>nb_cpu) ? 1:0 );

			for(uint8_t i=0; i<Nb_Core_Th; i++)
				TabMask[current_thread++]=GetCPUMask(cpu.ProcMask[i_cpu],0);

			nb_cpu++;
			i_cpu=(i_cpu+1)%cpu.NbPhysCore;
		}
	}
	else
	{
		if (UseMaxPhysCore)
		{
			if (NbThread>cpu.NbPhysCore)
			{
				while (NbThread>current_thread)
				{
					uint8_t Nb_Core_Th=NbThread/cpu.NbPhysCore+( ((NbThread%cpu.NbPhysCore)>nb_cpu) ? 1:0 );

					for(uint8_t i=0; i<Nb_Core_Th; i++)
						TabMask[current_thread++]=GetCPUMask(cpu.ProcMask[i_cpu],(i+i_ht)%cpu.NbHT[i_cpu]);

					nb_cpu++;
					i_cpu=(i_cpu+1)%cpu.NbPhysCore;
				}
			}
			else
			{
				while (NbThread>current_thread)
				{
					TabMask[current_thread++]=GetCPUMask(cpu.ProcMask[i_cpu],i_ht);
					i_cpu=(i_cpu+1)%cpu.NbPhysCore;
				}
			}
		}
		else
		{
			while (NbThread>current_thread)
			{
				uint8_t Nb_Core_Th=NbThread/cpu.NbPhysCore+( ((NbThread%cpu.NbPhysCore)>nb_cpu) ? 1:0 );

				Nb_Core_Th=(Nb_Core_Th<(cpu.NbHT[i_cpu]-i_ht)) ? (cpu.NbHT[i_cpu]-i_ht):Nb_Core_Th;
				Nb_Core_Th=(Nb_Core_Th<=(NbThread-current_thread)) ? Nb_Core_Th:(NbThread-current_thread);

				for (uint8_t i=0; i<Nb_Core_Th; i++)
					TabMask[current_thread++]=GetCPUMask(cpu.ProcMask[i_cpu],i+i_ht);

				i_cpu=(i_cpu+1)%cpu.NbPhysCore;
				nb_cpu++;
				i_ht=0;
			}
		}
	}
}

DWORD WINAPI ThreadPool::StaticThreadpool(LPVOID lpParam )
{
	const MT_Data_Thread *data=(MT_Data_Thread *)lpParam;

	while (true)
	{
		WaitForSingleObject(data->nextJob,INFINITE);
		switch(data->f_process)
		{
			case 1 :
				if (data->MTData!=NULL)
				{
					data->MTData->thread_Id=data->thread_Id;
					if (data->MTData->pFunc!=NULL) data->MTData->pFunc(data->MTData);
				}
				break;
			case 255 : return(0); break;
			default : break;
		}
		ResetEvent(data->nextJob);
		SetEvent(data->jobFinished);
	}
}


ThreadPool::ThreadPool(void): Status_Ok(true), _pimpl(new ThreadPoolPimpl(0))
{
	int16_t i;

	for (i=0; i<MAX_MT_THREADS; i++)
	{
		jobFinished[i]=NULL;
		nextJob[i]=NULL;
		MT_Thread[i].MTData=NULL;
		MT_Thread[i].f_process=0;
		MT_Thread[i].thread_Id=(uint8_t)i;
		MT_Thread[i].jobFinished=NULL;
		MT_Thread[i].nextJob=NULL;
		thds[i]=NULL;
		tids[i]=0;
		ThreadMask[i]=0;
		ThreadSleep[i]=true;
	}
	nPriority=NormalThreadLevel;
	TotalThreadsRequested=0;
	CurrentThreadsAllocated=0;
	CurrentThreadsUsed=0;

	Get_CPU_Info(CPU);
	Status_Ok=!(((CPU.NbLogicCPU==0) || (CPU.NbPhysCore==0)));
}

#define myCloseHandle(ptr) if (ptr!=NULL) { CloseHandle(ptr); ptr=NULL;}
void ThreadPool::FreeThreadPool(void)
{
	int16_t i;

	if (TotalThreadsRequested>0)
	{
		const int nPr=TabThreadLevel[AboveThreadLevel];

		for (i=TotalThreadsRequested-1; i>=0; i--)
		{
			if (thds[i]!=NULL)
			{
				SetThreadPriority(thds[i],nPr);
				if (ThreadSleep[i]) ResumeThread(thds[i]);
				MT_Thread[i].f_process=255;
				SetEvent(nextJob[i]);
				WaitForSingleObject(thds[i],INFINITE);
				myCloseHandle(thds[i]);
				MT_Thread[i].f_process=0;
				MT_Thread[i].MTData=NULL;
				MT_Thread[i].jobFinished=NULL;
				MT_Thread[i].nextJob=NULL;
				ThreadSleep[i]=true;
			}
		}

		for (i=TotalThreadsRequested-1; i>=0; i--)
		{
			myCloseHandle(nextJob[i]);
			myCloseHandle(jobFinished[i]);
		}
	}

	nPriority=NormalThreadLevel;
	TotalThreadsRequested=0;
	CurrentThreadsAllocated=0;
	CurrentThreadsUsed=0;
}

void ThreadPool::DestroyThreadPool(void)
{
	int16_t i;

	if (TotalThreadsRequested>0)
	{
		for (i=TotalThreadsRequested-1; i>=0; i--)
		{
			if (thds[i]!=NULL)
			{
				TerminateThread(thds[i],0);
				myCloseHandle(thds[i]);
			}
		}

		for (i=TotalThreadsRequested-1; i>=0; i--)
		{
			myCloseHandle(nextJob[i]);
			myCloseHandle(jobFinished[i]);
		}
	}
}
#endif

ThreadPool::~ThreadPool()
{
#if HAVE_RESAMPLEMT || HAVE_NNEDI3
  DestroyThreadPool();
#endif
  Finish();
  Join();
  delete _pimpl;
}

#if HAVE_RESAMPLEMT || HAVE_NNEDI3
uint8_t ThreadPool::GetThreadNumber(uint8_t thread_number,bool logical)
{
	const uint8_t nCPU=(logical) ? CPU.NbLogicCPU:CPU.NbPhysCore;

	return((thread_number==0) ? ((nCPU>MAX_MT_THREADS) ? MAX_MT_THREADS:nCPU):thread_number);
}


bool ThreadPool::AllocateThreads(uint8_t thread_number,uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,
	bool SetAffinity,bool sleep,ThreadLevelName priority)
{
	if ((!Status_Ok) || (thread_number==0)) return(false);

	if (thread_number>CurrentThreadsAllocated)
	{
		TotalThreadsRequested=thread_number;
		CreateThreadPool(offset_core,offset_ht,UseMaxPhysCore,SetAffinity,sleep,priority);
	}

	return(Status_Ok);
}


bool ThreadPool::DeAllocateThreads(void)
{
	if (!Status_Ok) return(false);

	FreeThreadPool();

	return(true);
}


bool ThreadPool::ChangeThreadsAffinity(uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,bool SetAffinity)
{
	if ((!Status_Ok) || (CurrentThreadsAllocated==0)) return(false);

	CreateThreadsMasks(CPU,ThreadMask,TotalThreadsRequested,offset_core,offset_ht,UseMaxPhysCore);

	for(uint8_t i=0; i<CurrentThreadsAllocated; i++)
		SetThreadAffinityMask(thds[i],SetAffinity?ThreadMask[i]:CPU.FullMask);

	return(true);
}


bool ThreadPool::ChangeThreadsLevel(ThreadLevelName priority)
{
	if ((!Status_Ok) || (CurrentThreadsAllocated==0)) return(false);

	if (priority!=NoneThreadLevel)
	{
		const int nPr=TabThreadLevel[priority];

		nPriority=priority;
		for(int16_t i=0; i<(int16_t)CurrentThreadsUsed; i++)
			SetThreadPriority(thds[i],nPr);
	}

	return(true);
}


void ThreadPool::CreateThreadPool(uint8_t offset_core,uint8_t offset_ht,bool UseMaxPhysCore,
	bool SetAffinity,bool sleep,ThreadLevelName priority)
{
	int16_t i;

	CreateThreadsMasks(CPU,ThreadMask,TotalThreadsRequested,offset_core,offset_ht,UseMaxPhysCore);

	if (sleep)
	{
		for(i=0; i<(int16_t)CurrentThreadsAllocated; i++)
		{
			SetThreadAffinityMask(thds[i],SetAffinity?ThreadMask[i]:CPU.FullMask);
			if (!ThreadSleep[i])
			{
				SuspendThread(thds[i]);
				ThreadSleep[i]=true;
			}
		}
	}
	else
	{
		for(i=0; i<(int16_t)CurrentThreadsAllocated; i++)
		{
			SetThreadAffinityMask(thds[i],SetAffinity?ThreadMask[i]:CPU.FullMask);
			if (ThreadSleep[i])
			{
				ResumeThread(thds[i]);
				ThreadSleep[i]=false;
			}
		}
	}

	if (priority!=NoneThreadLevel) nPriority=priority;

	if (CurrentThreadsAllocated==TotalThreadsRequested) return;

	i=(int16_t)CurrentThreadsAllocated;
	while ((i<(int16_t)TotalThreadsRequested) && Status_Ok)
	{
		jobFinished[i]=CreateEvent(NULL,TRUE,TRUE,NULL);
		nextJob[i]=CreateEvent(NULL,TRUE,FALSE,NULL);
		MT_Thread[i].jobFinished=jobFinished[i];
		MT_Thread[i].nextJob=nextJob[i];
		Status_Ok=Status_Ok && ((MT_Thread[i].jobFinished!=NULL) && (MT_Thread[i].nextJob!=NULL));
		i++;
	}
	if (!Status_Ok)
	{
		FreeThreadPool();
		return;
	}

	const int nPr=TabThreadLevel[IdleThreadLevel];

	i=(int16_t)CurrentThreadsAllocated;
	while ((i<(int16_t)TotalThreadsRequested) && Status_Ok)
	{
		thds[i]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StaticThreadpool,&MT_Thread[i],CREATE_SUSPENDED,&tids[i]);
		Status_Ok=Status_Ok && (thds[i]!=NULL);
		if (Status_Ok)
		{
			SetThreadAffinityMask(thds[i],SetAffinity?ThreadMask[i]:CPU.FullMask);
			SetThreadPriority(thds[i],nPr);
			if (!sleep)
			{
				ResumeThread(thds[i]);
				ThreadSleep[i]=false;
			}
		}
		i++;
	}

	if (!Status_Ok) FreeThreadPool();
	else CurrentThreadsAllocated=TotalThreadsRequested;
}


bool ThreadPool::RequestThreadPool(uint8_t thread_number,Public_MT_Data_Thread *Data,ThreadLevelName priority)
{
	if ((!Status_Ok) || (thread_number>CurrentThreadsAllocated)) return(false);

	const int nPr=TabThreadLevel[(priority!=NoneThreadLevel)?priority:nPriority];

	for(uint8_t i=0; i<thread_number; i++)
	{
		MT_Thread[i].MTData=Data+i;
		SetThreadPriority(thds[i],nPr);
		if (ThreadSleep[i])
		{
			ResumeThread(thds[i]);
			ThreadSleep[i]=false;
		}
	}

	CurrentThreadsUsed=thread_number;

	return(true);
}


bool ThreadPool::ReleaseThreadPool(bool sleep)
{
	if (!Status_Ok) return(false);

	if (CurrentThreadsUsed>0)
	{
		const int nPr=TabThreadLevel[IdleThreadLevel];

		for(uint8_t i=0; i<CurrentThreadsUsed; i++)
		{
			if (sleep)
			{
				SuspendThread(thds[i]);
				ThreadSleep[i]=true;
			}
			SetThreadPriority(thds[i],nPr);
			MT_Thread[i].MTData=NULL;
		}
		CurrentThreadsUsed=0;
	}

	return(true);
}


bool ThreadPool::StartThreads(void)
{
	if ((!Status_Ok) || (CurrentThreadsUsed==0)) return(false);

	for(uint8_t i=0; i<CurrentThreadsUsed; i++)
	{
		MT_Thread[i].f_process=1;
		ResetEvent(jobFinished[i]);
		SetEvent(nextJob[i]);
	}

	return(true);
}


bool ThreadPool::WaitThreadsEnd(void)
{
	if ((!Status_Ok) || (CurrentThreadsUsed==0)) return(false);

	WaitForMultipleObjects(CurrentThreadsUsed,jobFinished,TRUE,INFINITE);

	for(uint8_t i=0; i<CurrentThreadsUsed; i++)
		MT_Thread[i].f_process=0;

	return(true);
}
#endif
