#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "JlbMath.h"
#include "Systems/CameraSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/MenuSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

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
		const auto menuSys = systems.GetSystem<MenuSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);
		
		const bool isTickEvent = turnSys->GetIfTickEvent();

		vke::EntityRenderTask renderTask{};
		renderTask.subTexture = subTexturesDivided[0];
		
		glm::vec2 cameraCenter{};
		
		const auto subTextureDirArrow = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::directionalArrow);
		glm::vec2 inputDirs[4]
		{
			glm::vec2(0, -1),
			glm::vec2(-1, 0),
			glm::vec2(0, 1),
			glm::vec2(1, 0)
		};

		const size_t hoveredObj = mouseSys->GetHoveredObject();

		for (auto& entity : entities)
		{
			const auto& movementComponent = entity.movementComponent;
			const auto& transform = entity.transform;

			renderTask.transform = transform;
			renderTask.transform.scale *= movementComponent.systemDefined.scaleMultiplier;
			const bool hovered = hoveredObj == entity.collisionTaskId && hoveredObj != SIZE_MAX;
			renderTask.transform.scale *= 1.f + _scalingOnSelected * static_cast<float>(hovered);

			const auto result = entityRenderSys->TryAdd(info, renderTask);
			assert(result != SIZE_MAX);

			const bool mouseAction = mouseSys->GetPressedThisTurn() && !mouseSys->GetIsUIBlocking();
			_menuOpen = mouseAction ? _menuOpen ? false : hovered : _menuOpen;

			// Render Player Menu.
			if(_menuOpen)
			{
				MenuCreateInfo menuCreateInfo{};
				menuCreateInfo.interactable = true;
				menuCreateInfo.origin = transform.position;
				menuCreateInfo.entityCamera = &entityRenderSys->camera;
				menuCreateInfo.uiCamera = &uiRenderSys->camera;

				jlb::Array<jlb::StringView> strs{};
				jlb::StackArray<size_t, 5> outIds{};
				strs.Allocate(*info.dumpAllocator, 5);
				strs[0] = "inventory";
				strs[1] = "social";
				strs[2] = "test";
				strs[3] = "social";
				strs[4] = "test";
				menuCreateInfo.width = 4;
				menuCreateInfo.content = strs;
				menuCreateInfo.outInteractIds = outIds;
				menuSys->CreateMenu(info, systems, menuCreateInfo);
			}

			cameraCenter += entity.transform.position;
			entity.movementTaskId = movementSys->TryAdd(info, movementComponent);

			renderTask.subTexture = subTextureDirArrow;
			for (size_t i = 0; i < 4; ++i)
			{
				auto& input = _movementInput[i];
				renderTask.transform.position = transform.position + inputDirs[i];
				renderTask.transform.rotation = -jlb::math::PI * i * .5f;
				input.valid ? entityRenderSys->TryAdd(info, renderTask) : SIZE_MAX;
			}
		}

		if (isTickEvent)
			for (auto& entity : entities)
			{
				// Collision task.
				auto& transform = entity.transform;
				CollisionTask task{};
				task = entity.movementTaskId == SIZE_MAX ? glm::ivec2(transform.position) : glm::ivec2(entity.movementComponent.userDefined.to);
				task.layers = collisionLayerMain | collisionLayerInteractable;
				entity.collisionTaskId = collisionSys->TryAdd(task);
				assert(entity.collisionTaskId != SIZE_MAX);
			}

		cameraCenter /= entities.length;
		cameraSys->settings.target = cameraCenter;
	}

	void PlayerArchetype::EndFrame(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::EndFrame(info, systems, entities);

		const auto collisionSys = systems.GetSystem<CollisionSystem>();
		const auto movementSys = systems.GetSystem<MovementSystem>();
		const auto& movementOutputs = movementSys->GetOutput();
		const auto turnSys = systems.GetSystem<TurnSystem>();

		for (auto& entity : entities)
		{
			if (entity.movementTaskId == SIZE_MAX)
				continue;

			auto& transform = entity.transform;
			const auto& movementOutput = movementOutputs[entity.movementTaskId];

			entity.movementComponent.Update(movementOutput);
			transform.position = movementOutput.position;
			transform.rotation = movementOutput.rotation;
		}

		if (turnSys->GetIfTickEvent())
			for (auto& entity : entities)
			{
				auto& movementComponent = entity.movementComponent;
				const auto& transform = entity.transform;

				auto& movementUserDefined = movementComponent.userDefined;
				const auto& movementSystemDefined = movementComponent.systemDefined;

				entity.movementTileReservation = SIZE_MAX;

				// Update movement task with new input.
				if (movementSystemDefined.remaining == 0)
				{
					glm::ivec2 dir{};
					dir.x = static_cast<int32_t>(_movementInput[3].valid) - _movementInput[1].valid;
					dir.y = static_cast<int32_t>(_movementInput[2].valid) - _movementInput[0].valid;

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
						const glm::vec2 to = from + delta;

						if (collisionSys->CheckIfTilesAreReserved(glm::ivec2(to)) != SIZE_MAX)
							continue;

						uint32_t outCollision;
						const size_t collided = collisionSys->GetIntersections(glm::ivec2(to), outCollision);
						
						if(collided)
							continue;

						entity.movementTileReservation = collisionSys->ReserveTiles(glm::ivec2(to));
						movementUserDefined.from = from;
						movementUserDefined.to = to;
						movementUserDefined.rotation = transform.rotation;
						movementComponent.Build();
					}
				}
			}
	}

	void PlayerArchetype::OnKeyInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
		HandleKeyDirectionInput(GLFW_KEY_W, key, action, _movementInput[0], _movementInput[2]);
		HandleKeyDirectionInput(GLFW_KEY_A, key, action, _movementInput[1], _movementInput[3]);
		HandleKeyDirectionInput(GLFW_KEY_S, key, action, _movementInput[2], _movementInput[0]);
		HandleKeyDirectionInput(GLFW_KEY_D, key, action, _movementInput[3], _movementInput[1]);

		if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			for (auto& movementInput : _movementInput)
				movementInput.valid = movementInput.pressed;
	}

	void PlayerArchetype::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
	}

	void PlayerArchetype::HandleKeyDirectionInput(const int targetKey, const int activatedKey, const int action, Input& input, Input& opposite)
	{
		if (targetKey != activatedKey)
			return;

		if (action == GLFW_PRESS)
		{
			input.pressed = true;
			input.valid = true;
			opposite.valid = false;
			opposite.pressedSinceStartOfFrame = false;
		}
			
		if (action == GLFW_RELEASE)
		{
			input.pressed = false;
			if(input.pressedSinceStartOfFrame)
				input.valid = false;
			input.pressedSinceStartOfFrame = false;
		}
	}
}
