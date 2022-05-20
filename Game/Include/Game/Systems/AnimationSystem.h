#pragma once
#include "TaskSystem.h"
#include "Components/Renderer.h"
#include "Components/Animator.h"

namespace game
{
	struct AnimationTask final
	{
		Renderer* renderer = nullptr;
		Animator* animator = nullptr;
	};

	class AnimationSystem final : public TaskSystem<AnimationTask>
	{
	public:
		float frameDuration = 200;

		[[nodiscard]] static AnimationTask CreateDefaultTask(Renderer& renderer, Animator& animator);

	private:
		float _frame = FLT_MAX;

		void Update(const EngineOutData& outData, SystemChain& chain) override;
	};
}
