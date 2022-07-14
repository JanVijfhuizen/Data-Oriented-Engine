#pragma once
#include "GameSystem.h"

namespace vke
{
	class Scene
	{
		friend class SceneSystem;

	protected:
		virtual void Allocate(const EngineData& info, jlb::Systems<EngineData> systems) = 0;
		virtual void Free(const EngineData& info, jlb::Systems<EngineData> systems) = 0;
		virtual void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) {}
		virtual void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) {}

	private:
		Scene* _next = nullptr;
		jlb::AllocationID _allocationId{};
	};

	class SceneSystem final : public GameSystem
	{
	public:
		template <typename T>
		void AddScene(const EngineData& info, jlb::Systems<EngineData> systems);
		void PopScene(const EngineData& info, jlb::Systems<EngineData> systems);

	private:
		Scene* _latestScene = nullptr;

		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
	};

	template <typename T>
	void SceneSystem::AddScene(const EngineData& info, const jlb::Systems<EngineData> systems)
	{
		auto allocation = info.allocator->New<T>();
		Scene* scene = allocation.ptr;

		scene->_allocationId = allocation.id;
		scene->_next = _latestScene;

		_latestScene = scene;
		_latestScene->Allocate(info, systems);
	}
}
