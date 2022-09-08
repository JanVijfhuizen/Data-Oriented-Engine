#pragma once
#include "JobSystem.h"

namespace vke
{
	template <typename Job, typename Output>
	class JobSystemWithOutput : public JobSystem<Job>
	{
	public:
		[[nodiscard]] const jlb::NestedVector<Output>& GetOutput() const;

	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;

		void OnPreUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<Job>& tasks) override;
		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<Job>&, jlb::NestedVector<Output>& taskOutputs){}

		[[nodiscard]] jlb::NestedVector<Output>& GetOutputEditable();
		
	private:
		jlb::NestedVector<Output> _outputs{};

		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<Job>& tasks) override;
	};

	template <typename Task, typename Output>
	const jlb::NestedVector<Output>& JobSystemWithOutput<Task, Output>::GetOutput() const
	{
		return _outputs;
	}

	template <typename Task, typename Output>
	void JobSystemWithOutput<Task, Output>::Allocate(const EngineData& info)
	{
		JobSystem<Task>::Allocate(info);
		_outputs.Allocate(*info.allocator, JobSystem<Task>::GetLength(), 
			JobSystem<Task>::DefineNestedCapacity(info));
	}

	template <typename Task, typename Output>
	void JobSystemWithOutput<Task, Output>::Free(const EngineData& info)
	{
		_outputs.Free(*info.allocator);
		JobSystem<Task>::Free(info);
	}

	template <typename Task, typename Output>
	void JobSystemWithOutput<Task, Output>::OnPreUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<Task>& tasks)
	{
		_outputs.DetachNested();
		_outputs.Clear();

		JobSystem<Task>::OnPreUpdate(info, systems, tasks);
	}

	template <typename Task, typename Output>
	jlb::NestedVector<Output>& JobSystemWithOutput<Task, Output>::GetOutputEditable()
	{
		return _outputs;
	}

	template <typename Task, typename Output>
	void JobSystemWithOutput<Task, Output>::OnUpdate(const EngineData& info, 
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<Task>& tasks)
	{
		JobSystem<Task>::OnUpdate(info, systems, tasks);
		OnUpdate(info, systems, tasks, _outputs);
	}
}
