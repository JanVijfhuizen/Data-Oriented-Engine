#pragma once
#include "Bounds.h"
#include "CardSystem.h"
#include "StringView.h"
#include "VkEngine/Systems/GameSystem.h"

namespace vke
{
	struct EntityCamera;
	struct UICamera;
}

namespace game
{
	struct MenuUpdateInfo;

	struct MenuCreateInfo final
	{
		struct Content final
		{
			jlb::StringView string{};
			bool active = true;
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
		// Excluding title.
		[[nodiscard]] size_t GetInteractedColumnIndex(const MenuUpdateInfo& updateInfo) const;
		[[nodiscard]] size_t GetContentIndex(const MenuUpdateInfo& updateInfo, size_t columnIndex) const;
	};

	struct MenuUpdateInfo final
	{
		bool opened = false;
		bool hovered = false;
		bool centerHovered = false;
		float duration = 0;
		bool right = false;
		size_t scrollIdx = 0;
		float scrollPos = 0;
		float scrollArrowsLerp[2]{1, 1};
		size_t interactedIndex = SIZE_MAX;
	};

	struct TextBoxCreateInfo final
	{
		glm::vec2 origin{};
		jlb::StringView text{};
		size_t maxWidth = 24;
		size_t scale = 12;
		glm::vec2 borderSize{16, 8};
		bool center = true;
	};

	struct CardMenuCreateInfo final
	{
		size_t cardIndex = SIZE_MAX;
		glm::vec2 origin{};
	};

	struct CardMenuUpdateInfo final
	{
		float animLerp = 0;
	};

	class MenuSystem final : public vke::GameSystem
	{
	public:
		float openDuration = 1.f;
		float openTabDelay = .4f;
		float openWriteTextDuration = 2;
		float scrollAnimDuration = 1;
		float scrollAnimScaleMultiplier = .4f;
		float cardAnimSpeed = 5;

		void CreateMenu(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const MenuCreateInfo& createInfo, MenuUpdateInfo& updateInfo) const;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		static void CreateTextBox(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const TextBoxCreateInfo& createInfo);
		void CreateCardMenu(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const CardMenuCreateInfo& createInfo, CardMenuUpdateInfo& updateInfo) const;

	private:
		float _scrollDir = 0;

		void OnScrollInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			float xOffset, float yOffset) override;
	};
}
