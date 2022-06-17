#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Handlers/InputHandler.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/HumanoidBodySystem.h"

namespace game
{
	void PlayerArchetype::OnKeyInput(const int key, const int action, PlayerArchetype& instance)
	{
		auto& playerController = instance._playerController;
		auto& direction = playerController.direction;

		InputHandler::UpdateAxis(direction.x, GLFW_KEY_A, GLFW_KEY_D, key, action);
		InputHandler::UpdateAxis(direction.y, GLFW_KEY_W, GLFW_KEY_S, key, action);
	}

	void PlayerArchetype::Allocate(const EngineData& EngineData, SystemChain& chain)
	{
		IncreaseRequestedLength(1);

		Archetype<Player, PlayerUpdateInfo>::Allocate(EngineData, chain);

		const size_t length = GetLength();

		chain.Get<CollisionSystem>()->IncreaseRequestedLength(length, true);
		chain.Get<EntityRenderSystem>()->IncreaseRequestedLength(length);
		chain.Get<MovementSystem>()->IncreaseRequestedLength(length);
	}

	void PlayerArchetype::Free(const EngineData& EngineData, SystemChain& chain)
	{
		Archetype<Player, PlayerUpdateInfo>::Free(EngineData, chain);
	}

	void PlayerArchetype::Awake(const EngineData& EngineData, SystemChain& chain)
	{
		const auto& texture = chain.Get<EntityRenderSystem>()->GetTexture();
		const auto coords = texture::IndexToCoordinates(texture, renderConventions::ENTITY_SIZE, renderConventions::Player);
		_bodyVisuals = HumanoidBodySystem::CreateVisualsFromSubTexture(texture, coords);
	}

	PlayerUpdateInfo PlayerArchetype::OnPreEntityUpdate(const EngineData& EngineData, SystemChain& chain)
	{
		PlayerUpdateInfo info{};
		info.entityRenderSystem = chain.Get<EntityRenderSystem>();
		info.movementSystem = chain.Get<MovementSystem>();
		info.mousePosition = EngineData.mousePos;
		return info;
	}

	void PlayerArchetype::OnEntityUpdate(Player& entity, PlayerUpdateInfo& info)
	{
		auto& transform = entity.body.torso;

		MovementTask movementTask{};
		movementTask.dir = _playerController.direction;
		movementTask.collider = &entity.collider;
		movementTask.transform = &transform;
		info.movementSystem->Add(movementTask);

		EntityRenderTask renderTask{};
		auto& taskTransform = renderTask.transform;
		taskTransform = transform;
		renderTask.subTexture = _bodyVisuals.head;
		info.entityRenderSystem->Add(renderTask);
	}
}
