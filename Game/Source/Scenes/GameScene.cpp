#include "pch.h"
#include "Scenes/GameScene.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	void GameScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		const size_t size = DefineLevelSize(info, systems);
		_grid.Allocate(*info.allocator, size * size);
		GenerateLevel(_grid, info, systems);
	}

	void GameScene::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		_grid.Free(*info.allocator);
	}

	void GameScene::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		RenderLevel(info, systems);
	}

	size_t GameScene::DefineLevelSize(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) const
	{
		return 16;
	}

	jlb::ArrayView<Tile> GameScene::GetGrid() const
	{
		return _grid;
	}

	size_t GameScene::ToGridIndex(const glm::vec2 pos) const
	{
		const size_t size = sqrt(_grid.GetLength());
		auto index = static_cast<size_t>(round(pos.x) + round(pos.y) * size);
		index = index < _grid.GetLength() ? index : SIZE_MAX;
		return index;
	}

	glm::vec2 GameScene::ToWorldPos(const size_t index) const
	{
		const size_t size = sqrt(_grid.GetLength());
		return {index % size, index / size};
	}

	void GameScene::RenderLevel(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) const
	{
		const auto sys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto posMul = static_cast<float>(vke::PIXEL_SIZE_ENTITY);

		vke::EntityRenderTask task{};

		const size_t size = sqrt(_grid.GetLength());

		for (size_t y = 0; y < size; ++y)
		{
			const size_t column = y * size;

			for (size_t x = 0; x < size; ++x)
			{
				const size_t index = column + x;
				const auto& tile = _grid[index];

				task.subTexture = tile.subTexture;
				task.transform.position = glm::vec2(x, y) * posMul;
				const auto result = sys->TryAdd(task);
				assert(result);
			}
		}
	}
}
