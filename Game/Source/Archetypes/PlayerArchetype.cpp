#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/CameraSystem.h"
#include "Systems/CollisionSystem.h"
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
		const auto collisionSys = systems.GetSystem<CollisionSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);

		const bool isPaused = turnSys->GetIsPaused();
		const bool isTickEvent = turnSys->GetIfTickEvent();

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexturesDivided[0];

		MovementTask movementTask{};
		glm::vec2 cameraCenter{};

		for (auto& input : _movementInput)
			input.valid = isPaused ? input.pressed : input.valid;

		if (isTickEvent)
		{
			for (auto& entity : entities)
			{
				auto& movementComponent = entity.movementComponent;
				auto& transform = entity.transform;

				auto& movementUserDefined = movementComponent.userDefined;

				// Collision task.
				{
					CollisionTask task{};
					task.position = glm::ivec2(transform.position);
					task.scale = glm::vec2(transform.scale);
					const auto result = collisionSys->TryAdd(info, task);
					assert(result != SIZE_MAX);
				}

				// Update movement task with new input.
				if (movementUserDefined.remaining == 0)
				{
					glm::ivec2 dir{};
					dir.x = static_cast<int32_t>(_movementInput[3].valid) - _movementInput[1].valid;
					dir.y = dir.x == 0 ? static_cast<int32_t>(_movementInput[2].valid) - _movementInput[0].valid : 0;

					for (auto& input : _movementInput)
					{
						input.pressedSinceStartOfFrame = input.pressed;
						input.valid = input.pressed;
					}

					if (dir.x != 0 || dir.y != 0)
					{
						// Round the from position.
						const glm::vec2 from = glm::vec2(glm::ivec2(entity.transform.position));
						const glm::vec2 delta = glm::vec2(dir);

						movementUserDefined.from = from;
						movementUserDefined.to = from + delta;
						movementUserDefined.remaining = _movementDuration;
						movementUserDefined.rotation = transform.rotation;
					}
				}
			}
		}

		for (auto& entity : entities)
		{
			auto& movementComponent = entity.movementComponent;
			auto& transform = entity.transform;

			renderTask.transform = transform;
			renderTask.transform.scale *= movementComponent.systemDefined.scaleMultiplier;

			const auto result = entityRenderSys->TryAdd(info, renderTask);
			assert(result != SIZE_MAX);

			cameraCenter += entity.transform.position;
			movementTask.component = movementComponent;
			entity.movementTaskId = movementSys->TryAdd(info, movementTask);
		}

		cameraCenter /= entities.length;
		cameraSys->settings.target = cameraCenter;
	}

	void PlayerArchetype::EndFrame(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::EndFrame(info, systems, entities);

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

		if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			for (auto& movementInput : _movementInput)
				movementInput.valid = movementInput.pressed;
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
