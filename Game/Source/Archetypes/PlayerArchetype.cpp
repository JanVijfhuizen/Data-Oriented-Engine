#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "JlbMath.h"
#include "Systems/CameraSystem.h"
#include "Systems/MenuSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TurnSystem.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void PlayerArchetype::PreUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		CharacterArchetype<Player>::PreUpdate(info, systems, entities);

		const auto cameraSys = systems.GetSystem<CameraSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto menuSys = systems.GetSystem<MenuSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto uiInteractSys = systems.GetSystem<UIInteractionSystem>();

		auto& dumpAllocator = *info.dumpAllocator;

		const auto subTexture = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::humanoid);
		jlb::StackArray<vke::SubTexture, 2> subTexturesDivided{};
		vke::texture::Subdivide(subTexture, 2, subTexturesDivided);
		
		glm::vec2 cameraCenter{};
		
		const auto subTextureDirArrow = resourceSys->GetSubTexture(ResourceManager::EntitySubTextures::directionalArrow);
		glm::vec2 inputDirs[4]
		{
			glm::vec2(0, -1),
			glm::vec2(-1, 0),
			glm::vec2(0, 1),
			glm::vec2(1, 0)
		};
		
		const size_t uiHoveredObj = uiInteractSys->GetHoveredObject();
		const auto characterUpdateInfo = CreateCharacterPreUpdateInfo(info, systems);

		CharacterInput characterInput{};

		if (turnSys->GetIfTickEvent())
		{
			auto& dir = characterInput.movementDir;
			dir.x = static_cast<int32_t>(_movementInput[3].valid) - _movementInput[1].valid;
			dir.y = static_cast<int32_t>(_movementInput[2].valid) - _movementInput[0].valid;

			for (auto& input : _movementInput)
			{
				input.pressedSinceStartOfFrame = input.pressed;
				input.valid = input.pressed;
			}
		}

		for (auto& entity : entities)
		{
			auto& character = entity.character;
			PreUpdateCharacter(info, character, characterUpdateInfo, subTexturesDivided[0], characterInput);

			const auto& transform = character.transform;

			const bool mouseAction = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left) && !mouseSys->GetIsUIBlocking();
			const bool hovered = characterUpdateInfo.GetIsHovered(character);
			const bool menuOpen = mouseAction ? entity.menuUpdateInfo.opened ? false : hovered : entity.menuUpdateInfo.opened;

			// Render Player Menu.
			entity.menuIndex = menuOpen ? entity.menuIndex : Player::MenuIndex::main;
			if (menuOpen)
			{
				MenuCreateInfo menuCreateInfo{};
				menuCreateInfo.interactable = true;
				menuCreateInfo.origin = transform.position;
				menuCreateInfo.entityCamera = &entityRenderSys->camera;
				menuCreateInfo.uiCamera = &uiRenderSys->camera;

				jlb::Array<MenuCreateInfo::Content> content{};

				// Create menu content.
				switch (entity.menuIndex)
				{
				case Player::MenuIndex::main:
					content.Allocate(dumpAllocator, 2);
					content[0].string = "player";
					content[1].string = "cards";
					break;
				case Player::MenuIndex::cards:
					content.Allocate(dumpAllocator, 4);
					content[0].string = "cards";
					content[1].string = "a";
					content[2].string = "b";
					content[3].string = "c";
					content[3].active = false;
					break;
				}

				constexpr size_t MENU_MAX_LENGTH = 4;

				menuCreateInfo.width = 4;
				menuCreateInfo.maxLength = MENU_MAX_LENGTH;
				menuCreateInfo.content = content;
				menuCreateInfo.outInteractIds = entity.menuInteractIds;

				auto& idx = menuCreateInfo.interactedIndex;
				idx = SIZE_MAX;
				const auto length = jlb::math::Min<size_t>(MENU_MAX_LENGTH, content.GetLength() - 1);
				for (size_t i = 0; i < length; ++i)
					idx = uiHoveredObj == entity.menuInteractIds[i] ? i : idx;

				bool changePage = false;
				bool pressedBack = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::right);
				bool close = false;

				// Handle interaction.
				switch (entity.menuIndex)
				{
				case Player::MenuIndex::main:
					// If cards tab is pressed, go to card menu.
					changePage = idx == entity.menuInteractIds[0] && mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left);
					entity.menuIndex = changePage ? Player::MenuIndex::cards : entity.menuIndex;
					close = pressedBack;
					break;
				case Player::MenuIndex::cards:
					entity.menuIndex = pressedBack ? Player::MenuIndex::main : entity.menuIndex;
					break;
				}

				if(changePage || pressedBack || close)
					entity.menuUpdateInfo.Reset();
				if(!close)
					menuSys->CreateMenu(info, systems, menuCreateInfo, entity.menuUpdateInfo);
			}
			else
				entity.menuUpdateInfo.Reset();

			cameraCenter += character.transform.position;

			vke::EntityRenderTask renderTask{};
			renderTask.subTexture = subTextureDirArrow;
			for (size_t i = 0; i < 4; ++i)
			{
				auto& input = _movementInput[i];
				renderTask.transform.position = transform.position + inputDirs[i];
				renderTask.transform.rotation = -jlb::math::PI * i * .5f;
				input.valid ? entityRenderSys->TryAdd(info, renderTask) : SIZE_MAX;
			}
		}

		cameraCenter /= entities.length;
		cameraSys->settings.target = cameraCenter;
	}

	void PlayerArchetype::PostUpdate(const vke::EngineData& info,
		const jlb::Systems<vke::EngineData> systems,
		const jlb::ArrayView<Player> entities)
	{
		Archetype<Player>::PostUpdate(info, systems, entities);

		const auto characterUpdateInfo = CreateCharacterPreUpdateInfo(info, systems);

		for (auto& entity : entities)
		{
			auto& character = entity.character;
			PostUpdateCharacter(info, character, characterUpdateInfo);
		}
	}

	void PlayerArchetype::OnKeyInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
		HandleKeyDirectionInput(GLFW_KEY_W, key, action, _movementInput[0], _movementInput[2]);
		HandleKeyDirectionInput(GLFW_KEY_A, key, action, _movementInput[1], _movementInput[3]);
		HandleKeyDirectionInput(GLFW_KEY_S, key, action, _movementInput[2], _movementInput[0]);
		HandleKeyDirectionInput(GLFW_KEY_D, key, action, _movementInput[3], _movementInput[1]);

		if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			for (auto& movementInput : _movementInput)
				movementInput.valid = movementInput.pressed;
	}

	void PlayerArchetype::OnMouseInput(const vke::EngineData& info, 
		const jlb::Systems<vke::EngineData> systems, const int key, const int action)
	{
	}

	void PlayerArchetype::HandleKeyDirectionInput(const int targetKey, const int activatedKey, const int action, Input& input, Input& opposite)
	{
		if (targetKey != activatedKey)
			return;

		if (action == GLFW_PRESS)
		{
			input.pressed = true;
			input.valid = true;
			opposite.valid = false;
			opposite.pressedSinceStartOfFrame = false;
		}
			
		if (action == GLFW_RELEASE)
		{
			input.pressed = false;
			if(input.pressedSinceStartOfFrame)
				input.valid = false;
			input.pressedSinceStartOfFrame = false;
		}
	}
}
