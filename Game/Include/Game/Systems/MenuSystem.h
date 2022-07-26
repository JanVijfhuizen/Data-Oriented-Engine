#pragma once
#include "Bounds.h"
#include "StringView.h"
#include "VkEngine/Systems/GameSystem.h"

namespace vke
{
	struct EntityCamera;
	struct UICamera;
}

namespace game
{
	struct MenuCreateInfo final
	{
		glm::vec2 origin{};
		size_t width = 1;
		jlb::ArrayView<jlb::StringView> content{};
		bool interactable = false;
		vke::EntityCamera* entityCamera = nullptr;
		vke::UICamera* uiCamera = nullptr;
		jlb::ArrayView<size_t> outInteractIds{};
	};

	class MenuSystem final : public vke::GameSystem
	{
	public:
		static void CreateMenu(const vke::EngineData& info,
			jlb::Systems<vke::EngineData> systems, const MenuCreateInfo& createInfo);
	};
}
