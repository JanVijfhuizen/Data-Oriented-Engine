#pragma once
#include "StringView.h"
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct TextBoxTask final
	{
		glm::vec2 screenOrigin{ 0, .6f };
		jlb::StringView text{};
		size_t maxWidth = 24;
		size_t scale = 12;
		glm::vec2 borderSize{ 16, 8 };
		bool center = true;
	};

	class TextBoxSystem final : public vke::TaskSystem<TextBoxTask>
	{
	public:
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<TextBoxTask>& tasks) override;
	};
}
