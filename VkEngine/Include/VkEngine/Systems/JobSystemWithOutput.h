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
			const jlb::NestedVector<Job>& jobs) override;
		virtual void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<Job>&, jlb::NestedVector<Output>& jobs){}

		[[nodiscard]] jlb::NestedVector<Output>& GetOutputEditable();
		
	private:
		jlb::NestedVector<Output> _outputs{};

		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::NestedVector<Job>& jobs) override;
	};

	template <typename Job, typename Output>
	const jlb::NestedVector<Output>& JobSystemWithOutput<Job, Output>::GetOutput() const
	{
		return _outputs;
	}

	template <typename Job, typename Output>
	void JobSystemWithOutput<Job, Output>::Allocate(const EngineData& info)
	{
		JobSystem<Job>::Allocate(info);
		_outputs.Allocate(*info.allocator, JobSystem<Job>::GetLength(),
			JobSystem<Job>::DefineNestedCapacity(info));
	}

	template <typename Job, typename Output>
	void JobSystemWithOutput<Job, Output>::Free(const EngineData& info)
	{
		_outputs.Free(*info.allocator);
		JobSystem<Job>::Free(info);
	}

	template <typename Job, typename Output>
	void JobSystemWithOutput<Job, Output>::OnPreUpdate(const EngineData& info,
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<Job>& jobs)
	{
		_outputs.DetachNested();
		_outputs.Clear();

		JobSystem<Job>::OnPreUpdate(info, systems, jobs);
	}

	template <typename Job, typename Output>
	jlb::NestedVector<Output>& JobSystemWithOutput<Job, Output>::GetOutputEditable()
	{
		return _outputs;
	}

	template <typename Job, typename Output>
	void JobSystemWithOutput<Job, Output>::OnUpdate(const EngineData& info,
		const jlb::Systems<EngineData> systems,
		const jlb::NestedVector<Job>& jobs)
	{
		JobSystem<Job>::OnUpdate(info, systems, jobs);
		OnUpdate(info, systems, jobs, _outputs);
	}
}
