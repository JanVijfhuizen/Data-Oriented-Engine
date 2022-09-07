﻿#pragma once
#include "Bounds.h"
#include "StringView.h"
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace vke
{
	struct EntityCamera;
	struct UICamera;
}

namespace game
{
	struct MenuTask;

	struct MenuTaskUpdateInfo final
	{
		bool opened = false;
		bool hovered = false;
		bool centerHovered = false;
		float duration = 0;
		bool right = false;
		size_t scrollIdx = 0;
		float scrollPos = 0;
		float scrollArrowsLerp[2]{ 1, 1 };
		size_t interactedIndex = SIZE_MAX;

		// Excluding title.
		[[nodiscard]] size_t GetInteractedColumnIndex(const MenuTask& task) const;
		[[nodiscard]] size_t GetContentIndex(const MenuTask& task, size_t columnIndex) const;
	};

	struct MenuTask final
	{
		struct Content final
		{
			jlb::StringView string{};
			bool interactable = true;
			size_t amount = SIZE_MAX;
		};

		glm::vec2 origin{};
		float xOffset = 0;
		bool reverseXAxis = false;
		size_t width = 5;
		size_t maxLength = 4;
		size_t textScale = 10;
		size_t titleScale = 12;
		size_t usedSpace = SIZE_MAX;
		size_t capacity = SIZE_MAX;
		// First entry is used as the title.
		jlb::ArrayView<Content> content{};
		bool interactable = false;
		vke::EntityCamera* entityCamera = nullptr;
		vke::UICamera* uiCamera = nullptr;
		// Gets updated from this system as well.
		jlb::ArrayView<size_t> interactIds{};

		// Returns the amount of columns rendered.
		[[nodiscard]] size_t GetColumnCount() const;

		MenuTaskUpdateInfo updateInfo{};
	};

	class MenuSystem final : public vke::TaskSystemWithOutput<MenuTask, MenuTaskUpdateInfo>
	{
	public:
		float openDuration = 1.f;
		float openTabDelay = .4f;
		float openWriteTextDuration = 2;
		float scrollAnimDuration = 1;
		float scrollAnimScaleMultiplier = .4f;

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<MenuTask>& tasks) override;
		void OnPostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<MenuTask>& tasks) override;

	private:
		float _scrollDir = 0;

		void OnScrollInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			float xOffset, float yOffset) override;
	};
}
