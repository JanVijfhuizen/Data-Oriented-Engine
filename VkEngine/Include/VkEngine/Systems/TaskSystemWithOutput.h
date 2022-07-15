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

		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::Vector<Task>& tasks, jlb::Vector<Output>& taskOutputs){}

		[[nodiscard]] jlb::Vector<Output>& GetOutputVector();
		
	private:
		jlb::Vector<Output> _output{};
		
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::Vector<Task>& tasks) override;
	};

	template <typename Task, typename Output>
	jlb::ArrayView<Output> TaskSystemWithOutput<Task, Output>::GetOutput() const
	{
		return _output;
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::Allocate(const EngineData& info)
	{
		TaskSystem<Task>::Allocate(info);
		_output.Allocate(*info.allocator, TaskSystem<Task>::GetLength());
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::Free(const EngineData& info)
	{
		_output.Free(*info.allocator);
		TaskSystem<Task>::Free(info);
	}

	template <typename Task, typename Output>
	jlb::Vector<Output>& TaskSystemWithOutput<Task, Output>::GetOutputVector()
	{
		return _output;
	}

	template <typename Task, typename Output>
	void TaskSystemWithOutput<Task, Output>::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::Vector<Task>& tasks)
	{
		_output.SetCount(0);
		OnUpdate(info, systems, tasks, _output);
	}
}
