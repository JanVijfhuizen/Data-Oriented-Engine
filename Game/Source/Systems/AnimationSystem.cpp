#include "pch.h"
#include "Systems/AnimationSystem.h"
#include "Graphics/Animation.h"

namespace game
{
	void AnimationSystem::Update(const EngineOutData& engineOutData)
	{
		const bool newFrame = abs(engineOutData.time - _frame) > frameDuration;
		if(newFrame)
		{
			_frame = engineOutData.time;

			for (auto& task : *this)
			{
				assert(task.animator);
				assert(task.renderer);

				auto& animator = *task.animator;
				if (!animator.animation)
					continue;

				++animator.rDelay;

				auto& animation = animator.animation;
				auto& frame = animation->frames[animator.index];
				task.renderer->subTexture = frame.subTexture;

				if (animator.rDelay > frame.delay)
				{
					animator.rDelay = 0;
					animator.index = (animator.index + 1) % animation->frames.GetLength();
				}
			}
		}

		SetCount(0);
	}

	AnimationTask AnimationSystem::CreateDefaultTask(Renderer& renderer, Animator& animator)
	{
		AnimationTask task{};
		task.renderer = &renderer;
		task.animator = &animator;
		return task;
	}
}
