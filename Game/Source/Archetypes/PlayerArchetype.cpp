#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Graphics/RenderConventions.h"
#include "Systems/AnimationSystem.h"

void game::PlayerArchetype::Allocate(jlb::StackAllocator& allocator, const size_t size, const Player& fillValue)
{
	Archetype<Player, PlayerArchetypeInfo>::Allocate(allocator, size, fillValue);
	_testAnim.frames.Allocate(allocator, 2);
	
}

void game::PlayerArchetype::Free(jlb::StackAllocator& allocator)
{
	_testAnim.frames.Free(allocator);
	Archetype<Player, PlayerArchetypeInfo>::Free(allocator);
}

void game::PlayerArchetype::DefineResourceUsage(PlayerArchetypeInfo& info)
{
	info.renderSystem->IncreaseRequestedLength(GetLength());
	info.animationSystem->IncreaseRequestedLength(GetLength());
}

void game::PlayerArchetype::Start(PlayerArchetypeInfo& info)
{
	const auto& texture = info.renderSystem->GetTexture();
	_testAnim.frames[0].subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, 0);
	_testAnim.frames[1].subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, 1);
}

void game::PlayerArchetype::OnUpdate(Player& entity, PlayerArchetypeInfo& info)
{
	auto& transform = entity.transform;
	auto& renderer = entity.renderer;

	// Temp.
	entity.animator.animation = &_testAnim;

	info.animationSystem->Add(AnimationSystem::CreateDefaultTask(entity.renderer, entity.animator));

	static float f = 0;
	f += 0.0002f;

	transform.position.y = -96 + cos(f) * 32;

	RenderTask task{};
	auto& taskTransform = task.transform;
	taskTransform = transform;
	taskTransform.scale = RenderConventions::ENTITY_SIZE;
	//taskTransform.position = info.renderSystem->AlignPixelCoordinates(taskTransform.position);
	task.subTexture = renderer.subTexture;
	info.renderSystem->Add(task);
}
