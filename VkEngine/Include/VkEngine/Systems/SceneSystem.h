#pragma once
#include "GameSystem.h"
#include "NestedVector.h"
#include "VkEngine/Scenes/Scene.h"

namespace vke
{
	class SceneSystem : public GameSystem
	{
	protected:
		struct Initializer final
		{
			friend SceneSystem;

			template <typename T>
			void Add() const;

		private:
			SceneSystem* _sys = nullptr;
			jlb::StackAllocator* _allocator = nullptr;
			jlb::StackAllocator* _tempAllocator = nullptr;
		};

		void Allocate(const EngineData& info) override;
		virtual void DefineScenes(const Initializer& initializer) = 0;

		void Load(size_t id);
		void Unload(size_t id);

	private:
		struct SceneData final
		{
			bool loaded = false;
			jlb::Allocation<Scene> allocation{};
		};

		jlb::Vector<SceneData> _scenes{};
	};

	template <typename T>
	void SceneSystem::Initializer::Add() const
	{
		auto allocation = _allocator->New<T>();
		assert(static_cast<Scene*>(allocation.ptr));

		SceneData data{};
		data.allocation = allocation;
		_sys->_scenes.Add(data, _allocator, _tempAllocator);
	}
}
