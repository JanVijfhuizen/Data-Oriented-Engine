﻿#pragma once
#include "TaskSystem.h"
#include "StringView.h"
#include "RenderSystem.h"

namespace game
{
	class RenderSystem;

	struct UITask
	{
		jlb::StringView text;
		glm::vec2 leftTop;
		glm::vec2 rightBot;
		bool autoResize = false;
	};

	class UISystem final : public jlb::TaskSystem<UITask>
	{
	public:
		void Allocate(const EngineOutData& engineOutData);
		void Free(const EngineOutData& engineOutData);
		void Update(const EngineOutData& engineOutData);

	private:
		using TaskSystem<UITask>::Allocate;

		const size_t _charSize = 6;
		const size_t _renderSizeUsage = 1000;
		RenderSystem _renderSystem{};
	};
}