#pragma once
#include "GameSystem.h"
#include "NestedVector.h"

namespace vke
{
	template <typename T>
	class TaskSystem : public GameSystem
	{
	public:
		// Returns MAX value if it couldn't add the task.
		[[nodiscard]] size_t TryAdd(const EngineData& info, const T& task);
		[[nodiscard]] const jlb::NestedVector<T>& GetTasks() const;

	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;

		[[nodiscard]] virtual size_t DefineCapacity(const EngineData& info);
		[[nodiscard]] virtual size_t DefineNestedCapacity(const EngineData& info);
		
		virtual void OnPreUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::NestedVector<T>& tasks){}
		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::NestedVector<T>& tasks){}
		virtual void OnPostUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::NestedVector<T>& tasks){}
		[[nodiscard]] virtual bool ValidateOnTryAdd(const T& task);

		[[nodiscard]] virtual bool AutoClearOnFrameEnd();
		void ClearTasks();

		[[nodiscard]] size_t GetLength() const;
		[[nodiscard]] size_t GetCount() const;

	private:
		jlb::NestedVector<T> _tasks{};

		void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Update(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
	};

	template <typename T>
	size_t TaskSystem<T>::TryAdd(const EngineData& info, const T& task)
	{
		if (!ValidateOnTryAdd(task))
			return SIZE_MAX;
		_tasks.Add(*info.dumpAllocator, task);
		return _tasks.GetCount() - 1;
	}

	template <typename T>
	void TaskSystem<T>::Allocate(const EngineData& info)
	{
		System<EngineData>::Allocate(info);
		_tasks.Allocate(*info.allocator, DefineCapacity(info), DefineNestedCapacity(info));
	}

	template <typename T>
	void TaskSystem<T>::Free(const EngineData& info)
	{
		_tasks.DetachNested();
		_tasks.Free(*info.allocator);
		System<EngineData>::Free(info);
	}

	template <typename T>
	size_t TaskSystem<T>::DefineCapacity(const EngineData& info)
	{
		return 0;
	}

	template <typename T>
	size_t TaskSystem<T>::DefineNestedCapacity(const EngineData& info)
	{
		return 8;
	}

	template <typename T>
	bool TaskSystem<T>::ValidateOnTryAdd(const T& task)
	{
		return true;
	}

	template <typename T>
	bool TaskSystem<T>::AutoClearOnFrameEnd()
	{
		return true;
	}

	template <typename T>
	void TaskSystem<T>::ClearTasks()
	{
		_tasks.DetachNested();
		_tasks.Clear();
	}

	template <typename T>
	size_t TaskSystem<T>::GetLength() const
	{
		return _tasks.GetLength();
	}

	template <typename T>
	size_t TaskSystem<T>::GetCount() const
	{
		return _tasks.GetCount();
	}

	template <typename T>
	const jlb::NestedVector<T>& TaskSystem<T>::GetTasks() const
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
		if(AutoClearOnFrameEnd())
			ClearTasks();
	}
}
