#include "pch.h"
#include "Systems/PlayerSystem.h"
#include "JlbMath.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

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
		const auto resourceSys = systems.Get<ResourceManager>();
		const auto turnSys = systems.Get<TurnSystem>();
		
		const auto subTextureDirArrow = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::directionalArrow);
		constexpr glm::ivec2 inputDirs[4]
		{
			glm::ivec2(0, -1),
			glm::ivec2(-1, 0),
			glm::ivec2(0, 1),
			glm::ivec2(1, 0)
		};

		// Draw the directional arrows based on where the player wants to go.
		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTextureDirArrow;

		for (size_t i = 0; i < 4; ++i)
		{
			const auto& input = _updateInfo.keyArrowInput[i];
			renderTask.transform.position = _updateInfo.position + glm::vec2(inputDirs[i]);
			renderTask.transform.rotation = -jlb::math::PI * static_cast<float>(i) * .5f;
			const auto result = input ? entityRenderSys->TryAdd(info, renderTask) : SIZE_MAX;
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
