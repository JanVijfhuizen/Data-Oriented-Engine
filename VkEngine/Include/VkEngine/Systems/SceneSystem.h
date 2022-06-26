#pragma once
#include "GameSystem.h"

namespace vke
{
	class Scene
	{
		friend class SceneSystem;

	private:
		Scene* _next = nullptr;
		jlb::AllocationID _allocationId{};

		virtual void Allocate(const EngineData& info, jlb::Systems<EngineData> systems) = 0;
		virtual void Free(const EngineData& info, jlb::Systems<EngineData> systems) = 0;
		virtual void Update(const EngineData& info, jlb::Systems<EngineData> systems) = 0;
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
		void Update(const EngineData& info, jlb::Systems<EngineData> systems) override;
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
