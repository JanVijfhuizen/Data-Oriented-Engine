#pragma once
#include "GameSystem.h"
#include "NestedVector.h"

namespace vke
{
	template <typename T>
	class JobSystem : public GameSystem
	{
	public:
		// Returns MAX value if it couldn't add the job.
		[[nodiscard]] size_t TryAdd(const EngineData& info, const T& job);
		[[nodiscard]] const jlb::NestedVector<T>& GetJobs() const;

	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;

		[[nodiscard]] virtual size_t DefineCapacity(const EngineData& info);
		[[nodiscard]] virtual size_t DefineNestedCapacity(const EngineData& info);
		
		virtual void OnPreUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::NestedVector<T>& jobs){}
		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::NestedVector<T>& jobs){}
		virtual void OnPostUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::NestedVector<T>& jobs){}

		[[nodiscard]] virtual bool ValidateOnTryAdd(const T& job);

		[[nodiscard]] virtual bool AutoClearOnFrameEnd();
		void ClearJobs();

		[[nodiscard]] size_t GetLength() const;
		[[nodiscard]] size_t GetCount() const;

	private:
		jlb::NestedVector<T> _jobs{};

		void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Update(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
	};

	template <typename T>
	size_t JobSystem<T>::TryAdd(const EngineData& info, const T& job)
	{
		if (!ValidateOnTryAdd(job))
			return SIZE_MAX;
		_jobs.Add(*info.dumpAllocator, job);
		return _jobs.GetCount() - 1;
	}

	template <typename T>
	void JobSystem<T>::Allocate(const EngineData& info)
	{
		System<EngineData>::Allocate(info);
		_jobs.Allocate(*info.allocator, DefineCapacity(info), DefineNestedCapacity(info));
	}

	template <typename T>
	void JobSystem<T>::Free(const EngineData& info)
	{
		_jobs.DetachNested();
		_jobs.Free(*info.allocator);
		System<EngineData>::Free(info);
	}

	template <typename T>
	size_t JobSystem<T>::DefineCapacity(const EngineData& info)
	{
		return 8;
	}

	template <typename T>
	size_t JobSystem<T>::DefineNestedCapacity(const EngineData& info)
	{
		return 8;
	}

	template <typename T>
	bool JobSystem<T>::ValidateOnTryAdd(const T& job)
	{
		return true;
	}

	template <typename T>
	bool JobSystem<T>::AutoClearOnFrameEnd()
	{
		return true;
	}

	template <typename T>
	void JobSystem<T>::ClearJobs()
	{
		_jobs.DetachNested();
		_jobs.Clear();
	}

	template <typename T>
	size_t JobSystem<T>::GetLength() const
	{
		return _jobs.GetLength();
	}

	template <typename T>
	size_t JobSystem<T>::GetCount() const
	{
		return _jobs.GetCount();
	}

	template <typename T>
	const jlb::NestedVector<T>& JobSystem<T>::GetJobs() const
	{
		return _jobs;
	}

	template <typename T>
	void JobSystem<T>::PreUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PreUpdate(info, systems);
		OnPreUpdate(info, systems, _jobs);
	}

	template <typename T>
	void JobSystem<T>::Update(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		GameSystem::Update(info, systems);
		OnUpdate(info, systems, _jobs);
	}

	template <typename T>
	void JobSystem<T>::PostUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PostUpdate(info, systems);
		OnPostUpdate(info, systems, _jobs);
		if(AutoClearOnFrameEnd())
			ClearJobs();
	}
}
