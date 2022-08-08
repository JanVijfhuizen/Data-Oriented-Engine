#include "pch.h"
#include "Archetypes/PickupArchetype.h"
#include "Bounds.h"
#include "JlbMath.h"
#include "Systems/CameraSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InteractSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Graphics/CameraUtils.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	static void TryPickup(EntityData& target, EntityData& src, void* userPtr)
	{
		// TODO: Add to inventory.
		target.markedForDelete = true;
	}

	void PickupArchetype::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<Pickup>& entities)
	{
		Archetype<Pickup>::PreUpdate(info, systems, entities);

		const auto cameraSys = systems.GetSystem<CameraSystem>();
		const auto cardSys = systems.GetSystem<CardSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto interactSys = systems.GetSystem<InteractSystem>();
		const auto menuSys = systems.GetSystem<MenuSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto& camera = entityRenderSys->camera;

		auto& dumpAllocator = *info.dumpAllocator;

		const bool leftPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left);
		const bool rightPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::right);
		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::pickup);
		vke::EntityRenderTask task{};
		task.subTexture = subTexture;

		const auto hoveredObj = mouseSys->GetHoveredObject();
		bool resetMenu = true;

		const auto& camTarget = cameraSys->settings.target;

		if (turnSys->GetIfTickEvent())
		{
			const auto collisionSys = systems.GetSystem<CollisionSystem>();

			for (auto& entity : entities)
			{
				const auto& transform = entity.transform;

				// Collision task.
				CollisionTask collisionTask{};
				collisionTask.bounds = jlb::math::RoundNearest(transform.position);
				collisionTask.bounds.layers = collisionLayerMain | collisionLayerInteractable;
				entity.collisionTaskId = collisionSys->TryAdd(collisionTask);
				assert(entity.collisionTaskId != SIZE_MAX);
				
				entity.interacted = false;
			}
		}

		for (auto& entity : entities)
		{
			assert(entity.cardId != SIZE_MAX);

			const auto& transform = entity.transform;
			const bool culls = vke::Culls(camera.position, camera.pixelSize, transform.position, glm::vec2(transform.scale));
			entity.mouseTaskId = SIZE_MAX;
			if (!culls)
			{
				jlb::FBounds bounds{ transform.position, glm::vec2(transform.scale) };
				entity.mouseTaskId = mouseSys->TryAdd(info, bounds);
			}

			// Render.
			task.transform = transform;
			const bool hovered = hoveredObj == entity.mouseTaskId && hoveredObj != SIZE_MAX;
			task.transform.scale *= 1.f + scalingOnSelected * static_cast<float>(hovered);
			auto result = entityRenderSys->TryAdd(info, task);

			// Update menu if available.
			const bool menuOpen = leftPressedThisTurn && !mouseSys->GetIsUIBlocking() ? 
				_menuUpdateInfo.opened ? false : hovered : rightPressedThisTurn ? false : _menuUpdateInfo.opened;
			if(menuOpen)
			{
				const auto card = cardSys->GetCard(entity.cardId);

				MenuCreateInfo menuCreateInfo{};
				menuCreateInfo.interactable = true;
				menuCreateInfo.origin = transform.position;
				menuCreateInfo.entityCamera = &entityRenderSys->camera;
				menuCreateInfo.uiCamera = &uiRenderSys->camera;
				menuCreateInfo.interactIds = _menuInteractIds;
				menuCreateInfo.maxLength = _menuInteractIds.GetLength() + 1;
				menuCreateInfo.xOffset += 1;

				const float dis = glm::distance(camTarget, transform.position);
				const bool inRange = dis < 1.25f;

				jlb::Array<MenuCreateInfo::Content> content{};
				content.Allocate(dumpAllocator, 2);
				content[0].string = card.name;
				content[1].string = "pick up";
				content[1].interactable = inRange;
				menuCreateInfo.content = content;

				if (!entity.interacted && _menuUpdateInfo.hovered && leftPressedThisTurn)
				{
					// There is only one column so we know the pick up option is pressed.
					InteractionTask interactionTask{};
					interactionTask.src = 0; // Picked up by the player.
					interactionTask.target = entity.entityTaskId;
					interactionTask.interaction = TryPickup;

					result = interactSys->TryAdd(info, interactionTask);
					assert(result != SIZE_MAX);
					entity.interacted = true;
				}

				menuSys->CreateMenu(info, systems, menuCreateInfo, _menuUpdateInfo);
				
				CardMenuCreateInfo cardMenuCreateInfo{};
				cardMenuCreateInfo.origin = transform.position;
				cardMenuCreateInfo.cardIndex = entity.cardId;
				menuSys->CreateCardMenu(info, systems, cardMenuCreateInfo, _cardMenuUpdateInfo);
				resetMenu = false;
			}
		}

		_menuUpdateInfo = resetMenu ? MenuUpdateInfo() : _menuUpdateInfo;
		_cardMenuUpdateInfo = resetMenu ? CardMenuUpdateInfo() : _cardMenuUpdateInfo;
	}
}
