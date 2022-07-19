#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/CameraSystem.h"
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

		const auto cameraSys = systems.GetSystem<CameraSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexturesDivided[0];

		const bool isPaused = turnSys->GetIsPaused();
		const bool isTickEvent = turnSys->GetIfTickEvent();

		MovementTask movementTask{};
		glm::vec2 cameraCenter{};

		for (auto& input : _movementInput)
			input.valid = isPaused ? input.pressed : input.valid;

		for (auto& entity : entities)
		{
			auto& movementComponent = entity.movementComponent;
			entity.movementTaskId = SIZE_MAX;

			renderTask.transform = entity.transform;
			renderTask.transform.scale *= movementComponent.systemDefined.scaleMultiplier;

			const auto result = entityRenderSys->TryAdd(info, renderTask);
			assert(result != SIZE_MAX);

			auto& movementUserDefined = movementComponent.userDefined;

			cameraCenter += entity.transform.position;

			if(isTickEvent && movementUserDefined.remaining == 0)
			{
				glm::ivec2 dir{};
				dir.x = static_cast<int32_t>(_movementInput[3].valid) - _movementInput[1].valid;
				dir.y = dir.x == 0 ? static_cast<int32_t>(_movementInput[2].valid) - _movementInput[0].valid : 0;

				for (auto& input : _movementInput)
				{
					input.pressedSinceStartOfFrame = input.pressed;
					input.valid = input.pressed;
				}

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
			entity.movementTaskId = movementSys->TryAdd(info, movementTask);
		}

		cameraCenter /= entities.length;
		cameraSys->settings.target = cameraCenter;
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
		HandleKeyDirectionInput(GLFW_KEY_W, key, action, _movementInput[0]);
		HandleKeyDirectionInput(GLFW_KEY_A, key, action, _movementInput[1]);
		HandleKeyDirectionInput(GLFW_KEY_S, key, action, _movementInput[2]);
		HandleKeyDirectionInput(GLFW_KEY_D, key, action, _movementInput[3]);
	}

	void PlayerArchetype::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
	}

	void PlayerArchetype::HandleKeyDirectionInput(const int targetKey, const int activatedKey, const int action, Input& input)
	{
		if (targetKey != activatedKey)
			return;

		if (action == GLFW_PRESS)
		{
			input.pressed = true;
			input.valid = true;
		}
			
		if (action == GLFW_RELEASE)
		{
			input.pressed = false;
			if(input.pressedSinceStartOfFrame)
				input.valid = false;
		}
	}
}
