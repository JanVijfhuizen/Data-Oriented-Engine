#include "pch.h"
#include "Archetypes/DummyArchetype.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Graphics/TextureUtils.h"

namespace game::demo
{
	void DummyArchetype::PreUpdate(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<DummyEntity> entities)
	{
		CharacterArchetype<DummyEntity>::PreUpdate(info, systems, entities);

		const auto resourceSys = systems.GetSystem<ResourceManager>();

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);

		const auto characterUpdateInfo = CreateCharacterPreUpdateInfo(info, systems);

		for (auto& entity : entities)
		{
			auto& character = entity.character;
			PreUpdateCharacter(info, character, characterUpdateInfo, subTexturesDivided[0]);
		}
	}

	void DummyArchetype::EndFrame(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<DummyEntity> entities)
	{
		CharacterArchetype<DummyEntity>::EndFrame(info, systems, entities);

		const auto characterUpdateInfo = CreateCharacterPreUpdateInfo(info, systems);
		CharacterInput characterInput{};
		characterInput.movementDir.x = rand() % 3 - 1;
		characterInput.movementDir.y = rand() % 3 - 1;

		for (auto& entity : entities)
		{
			auto& character = entity.character;
			EndFrameCharacter(info, character, characterUpdateInfo, characterInput);
		}
	}
}
