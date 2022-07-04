#pragma once
#include "VkEngine/Systems/SceneSystem.h"
#include "Entities/Tile.h"

namespace game
{
	class GameScene : public vke::Scene
	{
	protected:
		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

		[[nodiscard]] virtual size_t DefineLevelSize(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) const;
		virtual void GenerateLevel(jlb::ArrayView<Tile> level, const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) const = 0;

		[[nodiscard]] jlb::ArrayView<Tile> GetGrid() const;
		// Returns SIZE_MAX if out of bounds.
		[[nodiscard]] size_t ToGridIndex(glm::vec2 pos) const;
		[[nodiscard]] glm::vec2 ToWorldPos(size_t index) const;

	private:
		jlb::Array<Tile> _grid{};

		void RenderLevel(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) const;
	};
}
