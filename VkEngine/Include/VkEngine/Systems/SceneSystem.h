#pragma once
#include "GameSystem.h"

namespace vke
{
	class SceneSystem : public GameSystem
	{
	protected:
		void Allocate(const EngineData& info) override;
		void Free(const EngineData& info) override;
		void Awake(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Start(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void BeginFrame(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PreUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void Update(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void PostUpdate(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void EndFrame(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void OnRecreateSwapChainAssets(const EngineData& info, jlb::Systems<EngineData> systems) override;
		void OnKeyInput(const EngineData& info, jlb::Systems<EngineData> systems, int key, int action) override;
		void OnMouseInput(const EngineData& info, jlb::Systems<EngineData> systems, int key, int action) override;
		void OnScrollInput(const EngineData& info, jlb::Systems<EngineData> systems, float xOffset,
			float yOffset) override;
		void Exit(const EngineData& info, jlb::Systems<EngineData> systems) override;

		virtual void DefineScenes(const jlb::SystemsInitializer<EngineData>& initializer) = 0;

	private:
		jlb::SystemManager<EngineData> _sceneManager{};
	};
}
