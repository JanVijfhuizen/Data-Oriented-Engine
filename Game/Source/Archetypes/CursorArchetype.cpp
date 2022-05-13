#include "pch.h"
#include "Archetypes/CursorArchetype.h"
#include "Graphics/RenderConventions.h"
#include "Handlers/InputHandler.h"

namespace game
{
	void CursorArchetype::OnMouseKeyInput(const int key, const int action, CursorArchetype& instance)
	{
		InputHandler::UpdateKey(instance._lMouseKey, GLFW_MOUSE_BUTTON_LEFT, key, action);
		InputHandler::UpdateKey(instance._rMouseKey, GLFW_MOUSE_BUTTON_RIGHT, key, action);
	}

	void CursorArchetype::Allocate(jlb::StackAllocator& allocator, const size_t size, const Cursor& fillValue)
	{
		Base::Allocate(allocator, size, fillValue);
		_idleAnim.frames.Allocate(allocator, 2);
	}

	void CursorArchetype::Free(jlb::StackAllocator& allocator)
	{
		_idleAnim.frames.Free(allocator);
		Base::Free(allocator);
	}

	void CursorArchetype::DefineResourceUsage(CursorArchetypeCreateInfo& info)
	{
		info.renderSystem->IncreaseRequestedLength(GetLength());
		info.animationSystem->IncreaseRequestedLength(GetLength());
	}

	void CursorArchetype::Start(CursorArchetypeCreateInfo& info)
	{
		const auto& texture = info.renderSystem->GetTexture();

		auto subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Cursor);
		subTexture.rightBot = subTexture.leftTop;

		auto& f1 = _idleAnim.frames[0];
		auto& f2 = _idleAnim.frames[1];
		auto& s1 = f1.subTexture;
		auto& s2 = f2.subTexture;
		s1 = subTexture + TextureHandler::GenerateSubTexture(texture, RenderConventions::CURSOR_SIZE, 0);
		s2 = subTexture + TextureHandler::GenerateSubTexture(texture, RenderConventions::CURSOR_SIZE, 1);
		f1.delay = 4;
		f2.delay = 4;
	}

	void CursorArchetype::OnAdd(Cursor& entity)
	{
		entity.animator.animation = &_idleAnim;
	}

	void CursorArchetype::OnEntityUpdate(Cursor& entity, CursorArchetypeUpdateInfo& info)
	{
		auto& transform = entity.transform;
		auto& renderer = entity.renderer;

		info.animationSystem->Add(AnimationSystem::CreateDefaultTask(entity.renderer, entity.animator));

		transform.position = glm::vec2(-1) + info.mousePosition * 2.f * sensitivity;

		RenderTask task{};
		auto& taskTransform = task.transform;
		taskTransform = transform;
		taskTransform.scale = RenderConventions::CURSOR_SIZE;
		task.subTexture = renderer.subTexture;
		info.renderSystem->Add(task);
	}
}
