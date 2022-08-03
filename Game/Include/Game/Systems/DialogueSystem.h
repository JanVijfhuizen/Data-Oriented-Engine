#pragma once
#include "StringView.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class DialogueSystem final : public vke::GameSystem
	{
	public:
		void DisplayText(jlb::StringView text);

	private:
		jlb::StringView _text;
		size_t _charIndex = 0;

		void PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) override;
	};
}
