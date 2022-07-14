#pragma once
#include "GameSystem.h"

namespace vke
{
	template <typename T>
	class TaskSystem : public GameSystem
	{
	public:
		// Returns MAX value if it couldn't add the task.
		[[nodiscard]] size_t TryAdd(const T& task);
		[[nodiscard]] size_t GetCount();
		[[nodiscard]] size_t GetLength();

	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;

		[[nodiscard]] virtual size_t DefineMinimalUsage(const EngineData& info);

		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems, const jlb::Vector<T>& tasks) = 0;
		[[nodiscard]] virtual bool ValidateOnTryAdd(const T& task);

	private:
		jlb::Vector<T> _tasks{};

		void Update(const EngineData& info, jlb::Systems<EngineData> systems) final override;
	};

	template <typename T>
	size_t TaskSystem<T>::TryAdd(const T& task)
	{
		if (_tasks.GetLength() == _tasks.GetCount())
			return SIZE_MAX;
		if (!ValidateOnTryAdd(task))
			return SIZE_MAX;
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
		size_t usage = DefineMinimalUsage(info);
		_tasks.Allocate(*info.allocator, usage);
	}

	template <typename T>
	void TaskSystem<T>::Free(const EngineData& info)
	{
		_tasks.Free(*info.allocator);
	}

	template <typename T>
	size_t TaskSystem<T>::DefineMinimalUsage(const EngineData& info)
	{
		return 0;
	}

	template <typename T>
	bool TaskSystem<T>::ValidateOnTryAdd(const T& task)
	{
		return true;
	}

	template <typename T>
	void TaskSystem<T>::Update(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		GameSystem::Update(info, systems);
		OnUpdate(info, systems, _tasks);
		_tasks.SetCount(0);
	}
}
