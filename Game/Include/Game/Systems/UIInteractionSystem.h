#pragma once
#include "Bounds.h"
#include "SwapChain.h"
#include "VkEngine/Systems/JobSystem.h"

namespace game
{
	struct UIInteractionJob final
	{
		jlb::FBounds bounds{};
	};

	class UIInteractionSystem final : public vke::JobSystem<UIInteractionJob>
	{
	public:
		[[nodiscard]] size_t GetHoveredObject();

	private:
		jlb::SwapChain<size_t, 2> _hovered{};

		void Allocate(const vke::EngineData& info) override;
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<UIInteractionJob>&) override;
	};
}
