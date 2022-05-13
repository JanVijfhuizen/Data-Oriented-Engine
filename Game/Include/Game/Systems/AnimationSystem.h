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

	class AnimationSystem final : public jlb::TaskSystem<AnimationTask>
	{
	public:
		float frameDuration = 200;

		void Update(const EngineOutData& engineOutData);

		[[nodiscard]] static AnimationTask CreateDefaultTask(Renderer& renderer, Animator& animator);
	private:
		float _frame = FLT_MAX;
	};
}
