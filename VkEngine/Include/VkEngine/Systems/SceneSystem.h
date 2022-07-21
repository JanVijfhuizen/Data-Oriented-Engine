#pragma once
#include "GameSystem.h"

namespace vke
{
	class Scene
	{
		friend class SceneSystem;

	protected:
		virtual void Allocate(const EngineData& info, jlb::Systems<EngineData> systems){}
		virtual void Free(const EngineData& info, jlb::Systems<EngineData> systems){}
		virtual void BeginFrame(const EngineData& info, const jlb::Systems<EngineData> systems) {}
		virtual void PreUpdate(const EngineData& info, const jlb::Systems<EngineData> systems) {}
		virtual void PostUpdate(const EngineData& info, const jlb::Systems<EngineData> systems) {}
		virtual void EndFrame(const EngineData& info, const jlb::Systems<EngineData> systems) {}
		virtual void OnKeyInput(const EngineData& info, const jlb::Systems<EngineData> systems, const int key, const int action){}
		virtual void OnMouseInput(const EngineData& info, const jlb::Systems<EngineData> systems, const int key, const int action){}

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
		void BeginFrame(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void EndFrame(const EngineData& info, jlb::Systems<EngineData> systems) override;

		void OnKeyInput(const EngineData& info, jlb::Systems<EngineData> systems, int key, int action) override;
		void OnMouseInput(const EngineData& info, jlb::Systems<EngineData> systems, int key, int action) override;
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
