#include "pch.h"
#include "Systems/TextRenderSystem.h"
#include "Graphics/TextureHandler.h"
#include "Graphics/RenderConventions.h"

namespace game
{
	void TextRenderSystem::AddAsCharRenderTasks(TextRenderTask& textRenderTask)
	{
		Transform transform{};
		transform.scale = RenderConventions::TEXT_SIZE;

		const char* strLit = textRenderTask.text;
		const size_t s = strlen(textRenderTask.text);

		auto& origin = textRenderTask.leftTop;
		auto& texture = GetTexture();

		for (size_t i = 0; i < s; ++i)
		{
			const char c = strLit[i];
			const int index = static_cast<int>(c - 'a');

			transform.position = origin + glm::vec2(transform.scale * (1.f + textRenderTask.spacingPct) * i, 0);

			CharRenderTask charRenderTask{};
			charRenderTask.transform = transform;
			charRenderTask.subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::TEXT_SIZE, index);
			Add(charRenderTask);
		}
	}

	RenderSystem<CharRenderTask>::CreateInfo TextRenderSystem::GetCreateInfo()
	{
		CreateInfo info{};
		info.atlasTexturePath = "Textures/ui-atlas.png";
		info.vertPath = "Shaders/ui-vert.spv";
		info.fragPath = "Shaders/ui-frag.spv";
		return info;
	}

	void TextRenderSystem::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		IncreaseRequestedLength(TEXT_RENDER_CHAR_CAPACITY);
		RenderSystem<CharRenderTask>::Allocate(outData, chain);
	}
}
