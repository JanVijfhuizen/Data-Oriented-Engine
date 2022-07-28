﻿#pragma once
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
		size_t maxLength = 4;
		jlb::ArrayView<jlb::StringView> content{};
		bool interactable = false;
		vke::EntityCamera* entityCamera = nullptr;
		vke::UICamera* uiCamera = nullptr;

		jlb::ArrayView<size_t> outInteractIds{};
		size_t interactedIndex = SIZE_MAX;
	};

	struct MenuUpdateInfo final
	{
		bool opened = false;
		float duration = 0;
		bool right = false;
		size_t scrollIdx = 0;
		float scrollPos = 0;
		float scrollArrowsLerp[2]{1, 1};

		void Reset();
	};

	class MenuSystem final : public vke::GameSystem
	{
	public:;
		float openDuration = .5f;
		float openTabDelay = .2f;
		float openWriteTextDuration = 2;
		float scrollAnimDuration = 1;
		float scrollAnimScaleMultiplier = .4f;

		void CreateMenu(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const MenuCreateInfo& createInfo, MenuUpdateInfo& updateInfo) const;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

	private:
		float _scrollDir = 0;

		void OnScrollInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			float xOffset, float yOffset) override;
	};
}