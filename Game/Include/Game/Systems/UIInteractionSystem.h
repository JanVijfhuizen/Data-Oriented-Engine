#pragma once
#include "Bounds.h"
#include "SwapChain.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct UIInteractionTask final
	{
		jlb::FBounds bounds{};
	};

	class UIInteractionSystem final : public vke::TaskSystem<UIInteractionTask>
	{
	public:
		struct FrameData final
		{
			size_t index = SIZE_MAX;
		};

	private:
		jlb::SwapChain<FrameData, 2> _frameData{};

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<UIInteractionTask>&) override;
	};
}
