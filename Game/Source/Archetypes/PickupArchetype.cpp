#include "pch.h"
#include "Archetypes/PickupArchetype.h"
#include "Bounds.h"
#include "JlbMath.h"
#include "Systems/CameraSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Graphics/CameraUtils.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void PickupArchetype::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		jlb::Vector<Pickup>& entities)
	{
		EntityArchetype<Pickup>::PreUpdate(info, systems, entities);

		const auto cameraSys = systems.GetSystem<CameraSystem>();
		const auto cardSys = systems.GetSystem<CardSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto menuSys = systems.GetSystem<MenuSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto playerSys = systems.GetSystem<PlayerSystem>();
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

		if (turnSys->GetIfBeginTickEvent())
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

		if(playerSys->IsPlayerOccupiedNextTurn())
		{
			_menuUpdateInfo = {};
			_cardMenuUpdateInfo = {};
		}

		for (auto& entity : entities)
		{
			const auto cardId = entity.data.pickup.cardId;
			assert(cardId != SIZE_MAX);

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
				const auto card = cardSys->GetCard(cardId);

				MenuCreateInfo menuCreateInfo{};
				menuCreateInfo.interactable = true;
				menuCreateInfo.origin = transform.position;
				menuCreateInfo.entityCamera = &entityRenderSys->camera;
				menuCreateInfo.uiCamera = &uiRenderSys->camera;
				menuCreateInfo.interactIds = _menuInteractIds;
				menuCreateInfo.maxLength = _menuInteractIds.GetLength() + 1;

				const float dis = glm::distance(camTarget, transform.position);
				const bool inRange = dis < 1.25f;

				jlb::Array<MenuCreateInfo::Content> content{};
				content.Allocate(dumpAllocator, 2);
				content[0].string = card.name;
				content[1].string = "pick up";
				content[1].interactable = inRange;
				menuCreateInfo.content = content;

				if (!entity.interacted && _menuUpdateInfo.hovered && leftPressedThisTurn && inRange)
				{
					playerSys->pickupEntity = entity.id;
					turnSys->SkipToNextTick();
				}

				menuSys->CreateMenu(info, systems, menuCreateInfo, _menuUpdateInfo);
				
				CardMenuCreateInfo cardMenuCreateInfo{};
				cardMenuCreateInfo.origin = transform.position + glm::vec2(1.75f, 0) * (static_cast<float>(!_menuUpdateInfo.right) * 2 - 1);
				cardMenuCreateInfo.cardIndex = cardId;
				menuSys->CreateCardMenu(info, systems, cardMenuCreateInfo, _cardMenuUpdateInfo);
				resetMenu = false;
			}
		}

		_menuUpdateInfo = resetMenu ? MenuUpdateInfo() : _menuUpdateInfo;
		_cardMenuUpdateInfo = resetMenu ? CardMenuUpdateInfo() : _cardMenuUpdateInfo;
	}
}
