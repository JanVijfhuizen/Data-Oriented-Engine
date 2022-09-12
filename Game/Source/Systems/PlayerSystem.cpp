#include "pch.h"
#include "Systems/PlayerSystem.h"
#include "JlbMath.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	bool PlayerSystem::IsPlayerOccupiedNextTurn() const
	{
		return pickupEntity;
	}

	void PlayerSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);

		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto turnSys = systems.Get<TurnSystem>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();
		
		const auto subTextureDirArrow = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::moveArrow);
		constexpr glm::ivec2 inputDirs[4]
		{
			glm::ivec2(0, -1),
			glm::ivec2(-1, 0),
			glm::ivec2(0, 1),
			glm::ivec2(1, 0)
		};

		// Draw the directional arrows based on where the player wants to go.
		vke::UIRenderJob renderJob{};

		// Calculate screen position for the render job.
		const auto& uiCamera = uiRenderSys->camera;
		const auto worldPos = -entityRenderSys->camera.position;

		for (size_t i = 0; i < 4; ++i)
		{
			const auto& input = _updateInfo.keyArrowInput[i];
			renderJob.position = worldPos + _updateInfo.position + glm::vec2(inputDirs[i]);
			renderJob.position = vke::UIRenderSystem::WorldToScreenPos(renderJob.position, uiCamera, info.swapChainData->resolution);
			renderJob.scale = glm::vec2(1.f / vke::PIXEL_SIZE_UI);
			renderJob.subTexture = vke::texture::GetSubTexture(subTextureDirArrow, 4, i);
			const auto result = input ? uiRenderSys->TryAdd(info, renderJob) : SIZE_MAX;
		}

		if(turnSys->GetIfBeginTickEvent())
			Reset();
	}

	void PlayerSystem::OnKeyInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		System<vke::EngineData>::OnKeyInput(info, systems, key, action);
	}

	void PlayerSystem::Reset()
	{
		pickupEntity = {};
	}
}
