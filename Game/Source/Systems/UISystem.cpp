#include "pch.h"
#include "Systems/UISystem.h"
#include "Graphics/TextureHandler.h"

namespace game
{
	void UISystem::Allocate(const EngineOutData& engineOutData)
	{
		RenderSystem::CreateInfo createInfo{};
		createInfo.vertPath = "Shaders/ui-vert.spv";
		createInfo.fragPath = "Shaders/ui-frag.spv";
		createInfo.atlasTexturePath = "Textures/ui-atlas.png";
		TaskSystem<UITask>::Allocate(*engineOutData.allocator);
		_renderSystem.IncreaseRequestedLength(_renderSizeUsage);
		_renderSystem.Allocate(engineOutData, createInfo);
	}

	void UISystem::Free(const EngineOutData& engineOutData)
	{
		_renderSystem.Free(engineOutData);
		TaskSystem<UITask>::Free(*engineOutData.allocator);
	}

	void UISystem::Update(const EngineOutData& engineOutData)
	{
		const auto& texture = _renderSystem.GetTexture();
		RenderTask renderTask{};
		Transform transform{};
		transform.scale = .2f;

		for (auto& task : *this)
		{
			const char* strLit = task.text;
			const size_t s = strlen(task.text);

			auto& origin = task.leftTop;

			for (size_t i = 0; i < s; ++i)
			{
				const char c = strLit[i];
				const int index = static_cast<int>(c -'a');

				// temp.
				transform.position = origin + glm::vec2(.3f * i, 0);
				renderTask.transform = transform;
				renderTask.subTexture = TextureHandler::GenerateSubTexture(texture, _charSize, index);
				_renderSystem.Add(renderTask);
			}
		}

		SetCount(0);
		_renderSystem.Update(engineOutData);
	}
}
