#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/MovementSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void PlayerArchetype::PreUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::PreUpdate(info, systems, entities);

		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexturesDivided[0];

		MovementTask movementTask{};
		movementTask.duration = 4;

		const bool isTickEvent = turnSys->GetIfTickEvent();

		for (auto& entity : entities)
		{
			auto& movementComponent = entity.movementComponent;
			entity.movementTaskId = SIZE_MAX;

			renderTask.transform = entity.transform;
			renderTask.transform.scale *= movementComponent.systemDefined.scaleMultiplier;

			const auto result = entityRenderSys->TryAdd(renderTask);
			assert(result != SIZE_MAX);

			auto& movementUserDefined = movementComponent.userDefined;

			// Testing.
			if(isTickEvent && movementUserDefined.remaining == 0)
			{
				glm::ivec2 dir{};
				dir.x = static_cast<int32_t>(_wasdKeysInput[3]) - _wasdKeysInput[1];
				dir.y = dir.x == 0 ? static_cast<int32_t>(_wasdKeysInput[2]) - _wasdKeysInput[0] : 0;
				if (dir.x == 0 && dir.y == 0)
					continue;
				
				// Round the from position.
				const glm::vec2 from = glm::vec2(glm::ivec2(entity.transform.position));
				const glm::vec2 delta = glm::vec2(dir * static_cast<int32_t>(vke::PIXEL_SIZE_ENTITY));

				movementUserDefined.from = from;
				movementUserDefined.to = from + delta;
				movementUserDefined.remaining = movementTask.duration;
				movementUserDefined.rotation = entity.transform.rotation;
			}

			movementTask.component = movementComponent;
			entity.movementTaskId = movementSys->TryAdd(movementTask);
		}
	}

	void PlayerArchetype::PostUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::PostUpdate(info, systems, entities);

		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto& movementOutputs = movementSys->GetOutput();
		
		for (auto& entity : entities)
		{
			if (entity.movementTaskId == SIZE_MAX)
				continue;

			const auto& movementOutput = movementOutputs[entity.movementTaskId];
			MovementSystem::UpdateComponent(entity.movementComponent, movementOutput);
			entity.transform.position = movementOutput.position;
			entity.transform.rotation = movementOutput.rotation;
		}
	}

	void PlayerArchetype::OnKeyInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
		HandleKeyDirectionInput(GLFW_KEY_W, key, action, _wasdKeysInput[0]);
		HandleKeyDirectionInput(GLFW_KEY_A, key, action, _wasdKeysInput[1]);
		HandleKeyDirectionInput(GLFW_KEY_S, key, action, _wasdKeysInput[2]);
		HandleKeyDirectionInput(GLFW_KEY_D, key, action, _wasdKeysInput[3]);
	}

	void PlayerArchetype::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
	}

	void PlayerArchetype::HandleKeyDirectionInput(const int targetKey, const int activatedKey, const int action, bool& keyPressed)
	{
		keyPressed = targetKey != activatedKey ? keyPressed : action == GLFW_PRESS ? true : action == GLFW_RELEASE ? false : keyPressed;
	}
}
