#pragma once
#include "Archetype.h"
#include "Archetypes/EntityArchetype.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game
{
	class GameScene : public vke::Scene
	{
	protected:
		struct ArchetypeInitializer final
		{
			friend GameScene;

			template <typename T>
			void Add(size_t capacity);

		private:
			GameScene* _scene = nullptr;
		};

		friend ArchetypeInitializer;

		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		virtual void DefineArchetypes(ArchetypeInitializer& initializer) = 0;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		
		[[nodiscard]] jlb::StackAllocator& GetAllocator();

		void OnKeyInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
		
	private:
		jlb::StackAllocator _allocator{};
		jlb::NestedVector<jlb::IArchetype*> _archetypes{};
	};

	template <typename T>
	void GameScene::ArchetypeInitializer::Add(const size_t capacity)
	{
		T* ptr = _scene->_allocator.New<T>().ptr;
		static_assert(dynamic_cast<jlb::IArchetype*>(ptr));
		ptr->Allocate(_scene->_allocator, capacity):
		_scene->_archetypes.Add(_scene->_allocator, ptr);
	}
}
