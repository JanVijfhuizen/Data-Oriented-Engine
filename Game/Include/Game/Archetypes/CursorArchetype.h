#pragma once
#include "Archetype.h"
#include "Components/Transform.h"
#include "Components/Renderer.h"
#include "Components/Animator.h"
#include "Systems/AnimationSystem.h"
#include "Graphics/RenderTask.h"
#include "Systems/RenderSystem.h"
#include "Graphics/Animation.h"

namespace game
{
	struct Cursor final
	{
		Animator animator{};
		Renderer renderer{};
		Transform transform{};
	};

	struct CursorArchetypeCreateInfo final
	{
		RenderSystem<RenderTask>* renderSystem;
		AnimationSystem* animationSystem;
	};

	struct CursorArchetypeUpdateInfo final
	{
		RenderSystem<RenderTask>* renderSystem;
		AnimationSystem* animationSystem;
		glm::vec2 mousePosition;
	};

	class CursorArchetype final : public jlb::Archetype<Cursor, CursorArchetypeCreateInfo, CursorArchetypeUpdateInfo>
	{
	public:
		float sensitivity = 50;

		static void OnMouseKeyInput(int key, int action, CursorArchetype& instance);

		void Allocate(jlb::StackAllocator& allocator, size_t size, const Cursor& fillValue = {}) override;
		void Free(jlb::StackAllocator& allocator) override;

		void DefineResourceUsage(CursorArchetypeCreateInfo& info) override;
		void Start(CursorArchetypeCreateInfo& info) override;

	private:
		int _lMouseKey = 0;
		int _rMouseKey = 0;
		Animation _idleAnim{};
		Animation _pressedAnim{};

		void OnAdd(Cursor& entity) override;
		void OnEntityUpdate(Cursor& entity, CursorArchetypeUpdateInfo& info) override;
	};
}
