#pragma once
#include "Archetype.h"
#include "Components/Transform.h"
#include "Components/Renderer.h"
#include "Components/Animator.h"
#include "Systems/AnimationSystem.h"
#include "Graphics/Animation.h"
#include "Graphics/RenderTask.h"

namespace game
{
	struct Cursor final
	{
		Animator animator{};
		Renderer renderer{};
		Transform transform{};
	};

	struct CursorUpdateInfo
	{
		AnimationSystem* animationSystem;
		EntityRenderSystem* entityRenderSystem;
		glm::vec2 mousePosition;
	};

	class CursorArchetype final : public Archetype<Cursor, CursorUpdateInfo>
	{
	public:
		float sensitivity = 50;

		static void OnMouseKeyInput(int key, int action, CursorArchetype& instance);

	private:
		int _lMouseKey = 0;
		int _rMouseKey = 0;
		Animation _idleAnim{};
		Animation _pressedAnim{};

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;

		void Start(const EngineOutData& outData, SystemChain& chain) override;

		void OnAdd(Cursor& entity) override;
		CursorUpdateInfo OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain) override;
		void OnEntityUpdate(Cursor& entity, CursorUpdateInfo& updateInfo) override;
	};
}
