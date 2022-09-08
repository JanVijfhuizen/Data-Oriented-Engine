#include "VkEngine/pch.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "JlbMath.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace vke
{
	void ThreadPoolSystem::ThreadObj::operator()(ThreadPoolSystem* sys) const
	{
		auto& jobs = sys->GetJobs();

		while(!sys->_stopThreads)
		{
			if(sys->_jobssRemaining == 0)
			{
				Sleep(0);
				continue;
			}

			// Get job.
			sys->_getNextJobsMutex.lock();
			// When another thread has stolen the job already.
			if(sys->_jobssRemaining == 0)
			{
				sys->_getNextJobsMutex.unlock();
				continue;
			}
			const auto job = jobs[--sys->_jobssRemaining];
			sys->_getNextJobsMutex.unlock();

			job.func(*sys->_threadSharedInfo.info, sys->_threadSharedInfo.systems, job.userPtr);
			--sys->_jobsUnfinished;
		}
	}

	void ThreadPoolSystem::Allocate(const EngineData& info)
	{
		JobSystem<ThreadPoolJob>::Allocate(info);

		_threadCount = GetThreadCount();
		_threads = info.allocator->New<std::thread>(_threadCount, ThreadObj(), this);
	}

	void ThreadPoolSystem::Free(const EngineData& info)
	{
		info.allocator->MFree(_threads.id);
		JobSystem<ThreadPoolJob>::Free(info);
	}

	void ThreadPoolSystem::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<ThreadPoolJob>& jobs)
	{
		JobSystem<ThreadPoolJob>::OnUpdate(info, systems, jobs);

		// Continue the threads.
		_threadSharedInfo.info = &info;
		_threadSharedInfo.systems = systems;
		_jobssRemaining = _jobsUnfinished = jobs.GetCount();
	}

	void ThreadPoolSystem::OnPostUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<ThreadPoolJob>& jobs)
	{
		JobSystem<ThreadPoolJob>::OnPostUpdate(info, systems, jobs);
		
		// Wait for the threads to finish.
		while (_jobsUnfinished > 0)
			Sleep(0);
	}

	void ThreadPoolSystem::Exit(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		_stopThreads = true;
		for (int i = 0; i < _threadCount; ++i)
			_threads.ptr[i].join();
		JobSystem<ThreadPoolJob>::Exit(info, systems);
	}

	size_t ThreadPoolSystem::DefineCapacity(const EngineData& info)
	{
		return THREAD_POOL_SYSTEM_CAPACITY;
	}

	size_t ThreadPoolSystem::DefineNestedCapacity(const EngineData& info)
	{
		return THREAD_POOL_SYSTEM_NESTED_CAPACITY;
	}

	size_t ThreadPoolSystem::GetThreadCount()
	{
		return jlb::math::Max<size_t>(1, jlb::math::Min<size_t>(std::thread::hardware_concurrency() - 1, 2));
	}

	size_t ThreadPoolSystem::GetFreeThreadSlots() const
	{
		return _threadCount < _jobsUnfinished ? 0 : _threadCount - _jobsUnfinished;
	}
}
