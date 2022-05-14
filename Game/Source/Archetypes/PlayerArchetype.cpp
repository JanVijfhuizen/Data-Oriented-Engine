#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Graphics/RenderConventions.h"
#include "Systems/AnimationSystem.h"
#include "Handlers/InputHandler.h"
#include "Systems/MovementSystem.h"

void game::PlayerArchetype::OnKeyInput(const int key, const int action, PlayerArchetype& instance)
{
	auto& playerController = instance._playerController;
	auto& direction = playerController.direction;

	InputHandler::UpdateAxis(direction.x, GLFW_KEY_A, GLFW_KEY_D, key, action);
	InputHandler::UpdateAxis(direction.y, GLFW_KEY_W, GLFW_KEY_S, key, action);
}

void game::PlayerArchetype::Allocate(jlb::StackAllocator& allocator, const size_t size, const Player& fillValue)
{
	Base::Allocate(allocator, size, fillValue);
	_testAnim.frames.Allocate(allocator, 2);
}

void game::PlayerArchetype::Free(jlb::StackAllocator& allocator)
{
	_testAnim.frames.Free(allocator);
	Base::Free(allocator);
}

void game::PlayerArchetype::DefineResourceUsage(PlayerArchetypeCreateInfo& info)
{
	info.renderSystem->IncreaseRequestedLength(GetLength());
	info.animationSystem->IncreaseRequestedLength(GetLength());
	info.movementSystem->IncreaseRequestedLength(GetLength());
}

void game::PlayerArchetype::Start(PlayerArchetypeCreateInfo& info)
{
	const auto& texture = info.renderSystem->GetTexture();
	_testAnim.frames[0].subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Player);
	_testAnim.frames[1].subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Player + 1);
}

void game::PlayerArchetype::OnPreEntityUpdate(PlayerArchetypeUpdateInfo& info)
{
	_playerController.lookDir = info.mousePosition;
}

void game::PlayerArchetype::OnEntityUpdate(Player& entity, PlayerArchetypeUpdateInfo& info)
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
		movementTask.transform = &entity.transform;
		info.movementSystem->Add(movementTask);
	}

	RenderTask renderTask{};
	auto& taskTransform = renderTask.transform;
	taskTransform = transform;
	taskTransform.scale = RenderConventions::ENTITY_SIZE;
	renderTask.subTexture = renderer.subTexture;
	info.renderSystem->Add(renderTask);
}
