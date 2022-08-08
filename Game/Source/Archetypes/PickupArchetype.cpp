﻿#include "pch.h"
#include "Archetypes/PickupArchetype.h"
#include "Bounds.h"
#include "JlbMath.h"
#include "Systems/CollisionSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "VkEngine/Graphics/CameraUtils.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void PickupArchetype::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Pickup> entities)
	{
		Archetype<Pickup>::PreUpdate(info, systems, entities);

		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto menuSys = systems.GetSystem<MenuSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto& camera = entityRenderSys->camera;

		auto& dumpAllocator = *info.dumpAllocator;

		const bool leftPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left) && !mouseSys->GetIsUIBlocking();
		const bool rightPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::right);
		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::pickup);
		vke::EntityRenderTask task{};
		task.subTexture = subTexture;

		const auto hoveredObj = mouseSys->GetHoveredObject();
		bool resetMenu = true;

		for (auto& entity : entities)
		{
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
			const auto result = entityRenderSys->TryAdd(info, task);

			// Update menu if available.
			const bool menuOpen = leftPressedThisTurn ? _menuUpdateInfo.opened ? false : hovered : rightPressedThisTurn ? false : _menuUpdateInfo.opened;
			if(menuOpen)
			{
				MenuCreateInfo menuCreateInfo{};
				menuCreateInfo.interactable = true;
				menuCreateInfo.origin = transform.position;
				menuCreateInfo.entityCamera = &entityRenderSys->camera;
				menuCreateInfo.uiCamera = &uiRenderSys->camera;
				menuCreateInfo.interactIds = _menuInteractIds;
				menuCreateInfo.maxLength = _menuInteractIds.GetLength() + 1;

				jlb::Array<MenuCreateInfo::Content> content{};
				content.Allocate(dumpAllocator, 2);
				content[0].string = "item name";
				content[1].string = "pickup";
				menuCreateInfo.content = content;

				menuSys->CreateMenu(info, systems, menuCreateInfo, _menuUpdateInfo);
				resetMenu = false;
			}
		}

		_menuUpdateInfo = resetMenu ? MenuUpdateInfo() : _menuUpdateInfo;

		if(turnSys->GetIfTickEvent())
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
			}
		}
	}
}
