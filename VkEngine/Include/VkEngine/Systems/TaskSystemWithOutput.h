#pragma once
#include "TaskSystem.h"

namespace vke
{
	template <typename Task, typename Output>
	class TaskSystemWithOutput : public TaskSystem<Task>
	{
	public:
		[[nodiscard]] jlb::ArrayView<Output> GetOutput() const;

	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;

		void OnPreUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestableVector<Task>& tasks) override;
		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestableVector<Task>&, jlb::Vector<Output>& taskOutputs){}

		[[nodiscard]] jlb::Vector<Output>& GetOutputVector();
		
	private:
		jlb::Vector<Output> _outputs{};

		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestableVector<Task>& tasks) override;
	};

	template <typename Task, typename Output>
	jlb::ArrayView<Output> TaskSystemWithOutput<Task, Output>::GetOutput() const
	{
		return _outputs;
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::Allocate(const EngineData& info)
	{
		TaskSystem<Task>::Allocate(info);
		_outputs.Allocate(*info.allocator, TaskSystem<Task>::GetLength());
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::Free(const EngineData& info)
	{
		_outputs.Free(*info.allocator);
		TaskSystem<Task>::Free(info);
	}

	template <typename Task, typename Output>
	jlb::Vector<Output>& TaskSystemWithOutput<Task, Output>::GetOutputVector()
	{
		return _outputs;
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::OnPreUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestableVector<Task>& tasks)
	{
		_outputs.SetCount(0);
		TaskSystem<Task>::OnPreUpdate(info, systems, tasks);
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestableVector<Task>& tasks)
	{
		TaskSystem<Task>::OnUpdate(info, systems, tasks);
		OnUpdate(info, systems, tasks, _outputs);
	}
}
