#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Graphics/RenderConventions.h"
#include "Systems/AnimationSystem.h"
#include "Handlers/InputHandler.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"

namespace game
{
	void PlayerArchetype::OnKeyInput(const int key, const int action, PlayerArchetype& instance)
	{
		auto& playerController = instance._playerController;
		auto& direction = playerController.direction;

		InputHandler::UpdateAxis(direction.x, GLFW_KEY_A, GLFW_KEY_D, key, action);
		InputHandler::UpdateAxis(direction.y, GLFW_KEY_W, GLFW_KEY_S, key, action);
	}

	void PlayerArchetype::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		IncreaseRequestedLength(1);

		Archetype<Player, PlayerUpdateInfo>::Allocate(outData, chain);
		_testAnim.frames.Allocate(*outData.allocator, 2);

		const size_t length = GetLength();

		chain.Get<CollisionSystem>()->IncreaseRequestedLength(length, true);
		chain.Get<EntityRenderSystem>()->IncreaseRequestedLength(length);
		chain.Get<AnimationSystem>()->IncreaseRequestedLength(length);
		chain.Get<MovementSystem>()->IncreaseRequestedLength(length);
	}

	void PlayerArchetype::Free(const EngineOutData& outData, SystemChain& chain)
	{
		_testAnim.frames.Free(*outData.allocator);
		Archetype<Player, PlayerUpdateInfo>::Free(outData, chain);
	}

	void PlayerArchetype::Start(const EngineOutData& outData, SystemChain& chain)
	{
		const auto& texture = chain.Get<EntityRenderSystem>()->GetTexture();
		_testAnim.frames[0].subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Player);
		_testAnim.frames[1].subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Player + 1);
	}

	PlayerUpdateInfo PlayerArchetype::OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain)
	{
		PlayerUpdateInfo info{};
		info.animationSystem = chain.Get<AnimationSystem>();
		info.entityRenderSystem = chain.Get<EntityRenderSystem>();
		info.movementSystem = chain.Get<MovementSystem>();
		info.mousePosition = outData.mousePos;
		return info;
	}

	void PlayerArchetype::OnEntityUpdate(Player& entity, PlayerUpdateInfo& info)
	{
		auto& transform = entity.transform;
		auto& renderer = entity.renderer;

		// Temp.
		entity.animator.animation = &_testAnim;

		info.animationSystem->Add(AnimationSystem::CreateDefaultTask(entity.renderer, entity.animator));

		if (_playerController.direction.x != 0 || _playerController.direction.y != 0)
		{
			MovementTask movementTask{};
			movementTask.speed = 0.02f;
			movementTask.dir = normalize(glm::vec2(_playerController.direction));
			movementTask.collider = &entity.collider;
			movementTask.transform = &entity.transform;
			info.movementSystem->Add(movementTask);
		}

		EntityRenderTask renderTask{};
		auto& taskTransform = renderTask.transform;
		taskTransform = transform;
		renderTask.subTexture = renderer.subTexture;
		info.entityRenderSystem->Add(renderTask);
	}
}
