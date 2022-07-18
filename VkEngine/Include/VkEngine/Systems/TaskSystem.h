#pragma once
#include "GameSystem.h"
#include "NestableVector.h"

namespace vke
{
	template <typename T>
	class TaskSystem : public GameSystem
	{
	public:
		// Returns MAX value if it couldn't add the task.
		[[nodiscard]] size_t TryAdd(const EngineData& info, const T& task);
		[[nodiscard]] size_t GetCount();
		[[nodiscard]] size_t GetLength();
		[[nodiscard]] jlb::ArrayView<T> GetTasks() const;

	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;

		[[nodiscard]] virtual size_t DefineMinimalUsage(const EngineData& info);
		[[nodiscard]] virtual size_t DefineNestedChunkSize(const EngineData& info);

		void BeginFrame(const EngineData& info, jlb::Systems<EngineData> systems) override;
		virtual void OnPreUpdate(const EngineData& info, jlb::Systems<EngineData> systems, jlb::ArrayView<T> tasks){}
		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems, jlb::ArrayView<T> tasks){}
		virtual void OnPostUpdate(const EngineData& info, jlb::Systems<EngineData> systems, jlb::ArrayView<T> tasks){}
		[[nodiscard]] virtual bool ValidateOnTryAdd(const T& task);

	private:
		jlb::Vector<T> _tasks{};
		jlb::NestableVector<T> _overflowingTasks{};

		void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Update(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
	};

	template <typename T>
	size_t TaskSystem<T>::TryAdd(const EngineData& info, const T& task)
	{
		const bool overflow = _tasks.GetLength() == _tasks.GetCount();
		if (overflow && _overflowingTasks.GetLength() == 0)
			return SIZE_MAX;
		if (!ValidateOnTryAdd(task))
			return SIZE_MAX;

		if (overflow)
			_overflowingTasks.Add(*info.dumpAllocator, task);
		else
			_tasks.Add(task);

		return _tasks.GetCount() - 1;
	}

	template <typename T>
	size_t TaskSystem<T>::GetCount()
	{
		return _tasks.GetCount();
	}

	template <typename T>
	size_t TaskSystem<T>::GetLength()
	{
		return _tasks.GetLength();
	}

	template <typename T>
	void TaskSystem<T>::Allocate(const EngineData& info)
	{
		System<EngineData>::Allocate(info);
		_tasks.Allocate(*info.allocator, DefineMinimalUsage(info));
	}

	template <typename T>
	void TaskSystem<T>::Free(const EngineData& info)
	{
		_tasks.Free(*info.allocator);
		System<EngineData>::Free(info);
	}

	template <typename T>
	size_t TaskSystem<T>::DefineMinimalUsage(const EngineData& info)
	{
		return 0;
	}

	template <typename T>
	size_t TaskSystem<T>::DefineNestedChunkSize(const EngineData& info)
	{
		return 8;
	}

	template <typename T>
	void TaskSystem<T>::BeginFrame(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::BeginFrame(info, systems);
		_overflowingTasks = {};
		_overflowingTasks.Allocate(*info.dumpAllocator, DefineNestedChunkSize(info));
	}

	template <typename T>
	bool TaskSystem<T>::ValidateOnTryAdd(const T& task)
	{
		return true;
	}

	template <typename T>
	jlb::ArrayView<T> TaskSystem<T>::GetTasks() const
	{
		return _tasks;
	}

	template <typename T>
	void TaskSystem<T>::PreUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PreUpdate(info, systems);
		OnPreUpdate(info, systems, _tasks);
	}

	template <typename T>
	void TaskSystem<T>::Update(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		GameSystem::Update(info, systems);
		OnUpdate(info, systems, _tasks);
	}

	template <typename T>
	void TaskSystem<T>::PostUpdate(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		System<EngineData>::PostUpdate(info, systems);
		OnPostUpdate(info, systems, _tasks);
		_tasks.SetCount(0);
	}
}
