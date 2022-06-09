﻿#include "pch.h"
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

	void CursorArchetype::Start(const EngineOutData& outData, SystemChain& chain)
	{
		const auto& texture = chain.Get<EntityRenderSystem>()->GetTexture();

		auto subTexture = texture::GenerateSubTexture(texture, renderConventions::ENTITY_SIZE, renderConventions::Cursor);
		subTexture.rightBot = subTexture.leftTop;

		// Idle animation.
		{
			const size_t delay = 4;
			auto& f1 = _idleAnim.frames[0];
			auto& f2 = _idleAnim.frames[1];
			auto& s1 = f1.subTexture;
			auto& s2 = f2.subTexture;
			s1 = subTexture + texture::GenerateSubTexture(texture, renderConventions::CURSOR_SIZE, 0);
			s2 = subTexture + texture::GenerateSubTexture(texture, renderConventions::CURSOR_SIZE, 1);
			f1.delay = delay;
			f2.delay = delay;
		}

		// Pressed animation.
		{
			auto& f1 = _pressedAnim.frames[0];
			auto& f2 = _pressedAnim.frames[1];
			auto& s1 = f1.subTexture;
			auto& s2 = f2.subTexture;
			s1 = subTexture + texture::GenerateSubTexture(texture, renderConventions::CURSOR_SIZE, 2);
			s2 = subTexture + texture::GenerateSubTexture(texture, renderConventions::CURSOR_SIZE, 3);
		}
	}

	Cursor& CursorArchetype::Add(const Cursor& task)
	{
		auto& entity = Archetype<Cursor, CursorUpdateInfo>::Add(task);
		entity.animator.animation = &_idleAnim;
		return entity;
	}

	void CursorArchetype::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		IncreaseRequestedLength(1);

		auto& allocator = *outData.allocator;
		Archetype<Cursor, CursorUpdateInfo>::Allocate(outData, chain);
		_idleAnim.frames.Allocate(allocator, 2);
		_pressedAnim.frames.Allocate(allocator, 2);

		chain.Get<EntityRenderSystem>()->IncreaseRequestedLength(GetLength());
		chain.Get<AnimationSystem>()->IncreaseRequestedLength(GetLength() * 2);
	}

	void CursorArchetype::Free(const EngineOutData& outData, SystemChain& chain)
	{
		auto& allocator = *outData.allocator;
		_pressedAnim.frames.Free(allocator);
		_idleAnim.frames.Free(allocator);
		Archetype<Cursor, CursorUpdateInfo>::Free(outData, chain);
	}

	CursorUpdateInfo CursorArchetype::OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain)
	{
		CursorUpdateInfo info{};
		info.animationSystem = chain.Get<AnimationSystem>();
		info.entityRenderSystem = chain.Get<EntityRenderSystem>();
		info.mousePosition = outData.mousePos;
		return info;
	}

	void CursorArchetype::OnEntityUpdate(Cursor& entity, CursorUpdateInfo& updateInfo)
	{
		auto& transform = entity.transform;
		auto& renderer = entity.renderer;

		updateInfo.animationSystem->Add(AnimationSystem::CreateDefaultTask(entity.renderer, entity.animator));

		transform.position = glm::vec2(-1) + updateInfo.mousePosition * 2.f * sensitivity;

		EntityRenderTask task{};
		auto& taskTransform = task.transform;
		taskTransform = transform;
		taskTransform.scale = renderConventions::CURSOR_SIZE;
		task.subTexture = renderer.subTexture;
		updateInfo.entityRenderSystem->Add(task);
	}
}
