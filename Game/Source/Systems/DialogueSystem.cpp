#include "pch.h"
#include "Systems/DialogueSystem.h"
#include "Systems/TextRenderHandler.h"

namespace game
{
	void DialogueSystem::DisplayText(const jlb::StringView text)
	{
		_text = text;
		_charIndex = 0;
	}

	void DialogueSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);

		if (!_text)
			return;

		const auto textRenderSys = systems.GetSystem<TextRenderHandler>();

		TextRenderTask task{};
		task.text = _text;
		task.center = true;
		task.origin.y += 0.5f;
		task.scale = 12;
		task.padding = static_cast<int32_t>(task.scale) / -2;
		const auto result = textRenderSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}
}
