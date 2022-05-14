#include "pch.h"
#include "Systems/TextSystem.h"
#include "Graphics/TextureHandler.h"
#include "Graphics/RenderConventions.h"

namespace game
{
	void TextSystem::CreateSwapChainAssets(const EngineOutData& engineOutData)
	{
		_renderSystem.CreateSwapChainAssets(engineOutData);
	}

	void TextSystem::DestroySwapChainAssets(const EngineOutData& engineOutData) const
	{
		_renderSystem.DestroySwapChainAssets(engineOutData);
	}

	void TextSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		const auto& texture = _renderSystem.GetTexture();
		InternalRenderTask renderTask{};
		Transform transform{};
		transform.scale = RenderConventions::TEXT_SIZE;

		for (auto& task : *this)
		{
			const char* strLit = task.text;
			const size_t s = strlen(task.text);

			auto& origin = task.leftTop;

			for (size_t i = 0; i < s; ++i)
			{
				const char c = strLit[i];
				const int index = static_cast<int>(c - 'a');

				transform.position = origin + glm::vec2(transform.scale * (1.f + task.spacingPct) * i, 0);
				//transform.position = _renderSystem.AlignPixelCoordinates(transform.position);
				renderTask.transform = transform;
				renderTask.subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::TEXT_SIZE, index);
				_renderSystem.Add(renderTask);
			}
		}

		SetCount(0);
		_renderSystem.Update(outData, chain);
	}

	void TextSystem::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		RenderSystem<InternalRenderTask>::CreateInfo createInfo{};
		createInfo.vertPath = "Shaders/ui-vert.spv";
		createInfo.fragPath = "Shaders/ui-frag.spv";
		createInfo.atlasTexturePath = "Textures/ui-atlas.png";
		TaskSystem<TextTask>::Allocate(outData, chain);
		_renderSystem.IncreaseRequestedLength(_renderSizeUsage);
		_renderSystem.Allocate(outData, createInfo);
	}

	void TextSystem::Free(const EngineOutData& outData, SystemChain& chain)
	{
		_renderSystem.Free(outData);
		TaskSystem<TextTask>::Free(outData, chain);
	}
}
