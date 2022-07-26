#pragma once
#include "Bounds.h"
#include "StringView.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	struct MenuCreateInfo final
	{
		glm::vec2 position{};
		float width = 1;
		jlb::ArrayView<jlb::StringView> content{};
		bool interactable = false;
	};

	struct Menu final
	{
		jlb::FBounds bounds{};
	};

	class MenuSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] Menu CreateMenu(const vke::EngineData& info,
			jlb::Systems<vke::EngineData> systems, const MenuCreateInfo& createInfo);
	};
}
