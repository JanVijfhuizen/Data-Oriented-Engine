#include "pch.h"
#include "Archetypes/CursorArchetype.h"
#include "Graphics/RenderConventions.h"
#include "Handlers/InputHandler.h"

namespace game
{
	void CursorArchetype::OnMouseKeyInput(const int key, const int action, CursorArchetype& instance)
	{
		auto& lMouseKey = instance._lMouseKey;
		auto& rMouseKey = instance._rMouseKey;

		const bool lUpdated = InputHandler::UpdateKey(lMouseKey, GLFW_MOUSE_BUTTON_LEFT, key, action);
		const bool rUpdated = InputHandler::UpdateKey(rMouseKey, GLFW_MOUSE_BUTTON_RIGHT, key, action);

		if(lUpdated || rUpdated)
			for (auto& cursor : instance)
			{
				auto& animator = cursor.animator;
				animator.animation = lMouseKey ? &instance._pressedAnim : &instance._idleAnim;
				animator.loop = !lMouseKey;
				animator.index = 0;
			}
	}

	void CursorArchetype::Allocate(jlb::StackAllocator& allocator, const size_t size, const Cursor& fillValue)
	{
		Base::Allocate(allocator, size, fillValue);
		_idleAnim.frames.Allocate(allocator, 2);
		_pressedAnim.frames.Allocate(allocator, 2);
	}

	void CursorArchetype::Free(jlb::StackAllocator& allocator)
	{
		_pressedAnim.frames.Free(allocator);
		_idleAnim.frames.Free(allocator);
		Base::Free(allocator);
	}

	void CursorArchetype::DefineResourceUsage(CursorArchetypeCreateInfo& info)
	{
		info.renderSystem->IncreaseRequestedLength(GetLength());
		info.animationSystem->IncreaseRequestedLength(GetLength() * 2);
	}

	void CursorArchetype::Start(CursorArchetypeCreateInfo& info)
	{
		const auto& texture = info.renderSystem->GetTexture();

		auto subTexture = TextureHandler::GenerateSubTexture(texture, RenderConventions::ENTITY_SIZE, RenderConventions::Cursor);
		subTexture.rightBot = subTexture.leftTop;

		// Idle animation.
		{
			const size_t delay = 4;
			auto& f1 = _idleAnim.frames[0];
			auto& f2 = _idleAnim.frames[1];
			auto& s1 = f1.subTexture;
			auto& s2 = f2.subTexture;
			s1 = subTexture + TextureHandler::GenerateSubTexture(texture, RenderConventions::CURSOR_SIZE, 0);
			s2 = subTexture + TextureHandler::GenerateSubTexture(texture, RenderConventions::CURSOR_SIZE, 1);
			f1.delay = delay;
			f2.delay = delay;
		}

		// Pressed animation.
		{
			auto& f1 = _pressedAnim.frames[0];
			auto& f2 = _pressedAnim.frames[1];
			auto& s1 = f1.subTexture;
			auto& s2 = f2.subTexture;
			s1 = subTexture + TextureHandler::GenerateSubTexture(texture, RenderConventions::CURSOR_SIZE, 2);
			s2 = subTexture + TextureHandler::GenerateSubTexture(texture, RenderConventions::CURSOR_SIZE, 3);
		}
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
