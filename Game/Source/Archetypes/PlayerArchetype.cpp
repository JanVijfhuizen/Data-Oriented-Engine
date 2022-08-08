#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "JlbMath.h"
#include "JlbString.h"
#include "Systems/CameraSystem.h"
#include "Systems/CardRenderSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/MenuSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "Systems/TurnSystem.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Graphics/Animation.h"
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
		const auto cardSys = systems.GetSystem<CardSystem>();
		const auto cardRenderSys = systems.GetSystem<CardRenderSystem>();
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto menuSys = systems.GetSystem<MenuSystem>();
		const auto mouseSys = systems.GetSystem<MouseSystem>();
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto textRenderSys = systems.GetSystem<TextRenderHandler>();
		const auto turnSys = systems.GetSystem<TurnSystem>();
		const auto uiRenderSys = systems.GetSystem<vke::UIRenderSystem>();
		const auto uiInteractSys = systems.GetSystem<UIInteractionSystem>();

		auto& dumpAllocator = *info.dumpAllocator;
		auto& tempAllocator = *info.tempAllocator;

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

		// Calculate movement direction, if any.
		if (turnSys->GetIfTickEvent())
		{
			auto& dir = characterInput.movementDir;
			dir.x = static_cast<int32_t>(_movementInput[3].valid) - _movementInput[1].valid;
			dir.y = static_cast<int32_t>(_movementInput[2].valid) - _movementInput[0].valid;

			// Reset input.
			for (auto& input : _movementInput)
			{
				input.pressedSinceStartOfFrame = input.pressed;
				input.valid = input.pressed;
			}
		}

		const bool leftPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left);
		const bool rightPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::right);
		const bool mouseAction = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left) && !mouseSys->GetIsUIBlocking();

		assert(entities.length <= 1);

		for (auto& entity : entities)
		{
			auto& character = entity.character;
			PreUpdateCharacter(info, character, characterUpdateInfo, subTexturesDivided[0], characterInput);

			const auto& transform = character.transform;

			const bool hovered = characterUpdateInfo.GetIsHovered(character);
			const bool menuOpen = mouseAction ? _menuUpdateInfo.opened ? false : hovered : _menuUpdateInfo.opened;

			// Render Player Menu.
			_menuIndex = menuOpen ? _menuIndex : MenuIndex::main;
			if (menuOpen)
			{
				MenuCreateInfo menuCreateInfo{};
				menuCreateInfo.interactable = true;
				menuCreateInfo.origin = transform.position;
				menuCreateInfo.entityCamera = &entityRenderSys->camera;
				menuCreateInfo.uiCamera = &uiRenderSys->camera;
				menuCreateInfo.interactIds = _menuInteractIds;
				menuCreateInfo.maxLength = _menuInteractIds.GetLength() + 1;
				menuCreateInfo.width = 7;

				MenuCreateInfo secondMenuCreateInfo = menuCreateInfo;
				secondMenuCreateInfo.reverseXAxis = true;
				secondMenuCreateInfo.interactIds = _secondMenuInteractIds;
				secondMenuCreateInfo.capacity = SIZE_MAX;
				secondMenuCreateInfo.usedSpace = SIZE_MAX;
				bool drawSecondWindow = false;

				jlb::Array<MenuCreateInfo::Content> content{};
				const jlb::ArrayView<InventorySlot> inventory = entity.inventory;

				auto& menuUpdateInfo = _menuUpdateInfo;
				auto& secondMenuUpdateInfo = _secondMenuUpdateInfo;

				// Create menu content.
				switch (_menuIndex)
				{
				case MenuIndex::main:
					content.Allocate(dumpAllocator, 3);
					content[0].string = "player";
					content[1].string = "inventory";
					content[2].string = "deck";
					break;
				case MenuIndex::inventory:
				case MenuIndex::deck:
					content.Allocate(dumpAllocator, inventory.length + 1);
					content[0].string = "inventory";
					for (size_t i = 0; i < inventory.length; ++i)
					{
						const auto card = cardSys->GetCard(inventory[i].index);
						content[i + 1].string = card.name;
					}
					break;
				}
				if(_menuIndex == MenuIndex::deck)
					for (size_t i = 0; i < inventory.length; ++i)
						content[i + 1].amount = MAX_COPIES_CARD_IN_DECK - inventory[i].amount;

				menuCreateInfo.content = content;

				bool changePage = false;
				bool close = false;

				size_t cardIndex = SIZE_MAX;
				bool renderCard = false;

				const auto oldCardActivated = _cardActivated;
				_cardActivated = 0;

				// Handle interaction.
				switch (_menuIndex)
				{
				case MenuIndex::main:
					if (leftPressedThisTurn)
						for (size_t i = 0; i < 2; ++i)
						{
							const bool columnHovered = uiHoveredObj == _menuInteractIds[i];
							changePage = changePage ? true : columnHovered;
							_menuIndex = changePage ? static_cast<MenuIndex>(i + 1) : _menuIndex;
							i = changePage ? 2 : i;
						}
					close = rightPressedThisTurn;
					break;
				case MenuIndex::inventory:
					renderCard = true;
					cardIndex = menuCreateInfo.GetInteractedColumnIndex(_menuUpdateInfo);
					cardIndex = cardIndex == SIZE_MAX ? SIZE_MAX : inventory[cardIndex].index;
					_cardActivated = leftPressedThisTurn && menuUpdateInfo.hovered ? _cardHovered : oldCardActivated;

					if(_cardActivated != SIZE_MAX)
					{
						const auto hoveredCard = cardSys->GetCard(_cardActivated);

						jlb::Array<MenuCreateInfo::Content> deckContent{};
						deckContent.Allocate(dumpAllocator, 3);
						deckContent[0].string = hoveredCard.name;
						deckContent[1].string = "use";
						deckContent[2].string = "drop";
						secondMenuCreateInfo.content = deckContent;
						drawSecondWindow = true;
					}
					if (_cardActivated != oldCardActivated)
						secondMenuUpdateInfo = {};
					
					break;
				case MenuIndex::deck:
					renderCard = true;
					// Get what cards are being used in the deck.
					size_t deckSize = 0;
					for (size_t i = 0; i < inventory.length; ++i)
					{
						const auto& src = content[i + 1];
						deckSize += src.amount != MAX_COPIES_CARD_IN_DECK;
					}

					menuCreateInfo.usedSpace = inventory.length;
					menuCreateInfo.capacity = entity.inventory.GetLength();

					// Get all cards in deck.
					jlb::Vector<size_t> cardIndexes{};
					cardIndexes.Allocate(tempAllocator, deckSize);
					for (size_t i = 0; i < inventory.length; ++i)
					{
						const auto& src = content[i + 1];
						if (src.amount != MAX_COPIES_CARD_IN_DECK)
							cardIndexes.Add(i);
					}

					// Try and add a card to the deck.
					bool deckResized = false;
					if (leftPressedThisTurn && _menuUpdateInfo.hovered)
					{
						const size_t interactIndex = menuCreateInfo.GetInteractedColumnIndex(menuUpdateInfo);
						if (interactIndex != SIZE_MAX)
						{
							auto& slot = inventory[interactIndex];
							slot.amount = jlb::math::Min(slot.amount + 1, MAX_COPIES_CARD_IN_DECK);
							deckResized = slot.amount == 1;
						}
					}
						
					// Create deck menu.
					{
						jlb::Array<MenuCreateInfo::Content> deckContent{};
						deckContent.Allocate(dumpAllocator, deckSize + 1);
						deckContent[0].string = "deck";

						size_t deckIndex = 1;
						for (size_t i = 0; i < inventory.length; ++i)
						{
							const auto& src = content[i + 1];
							if (src.amount != MAX_COPIES_CARD_IN_DECK)
							{
								auto& slot = deckContent[deckIndex];
								slot = src;
								slot.amount = MAX_COPIES_CARD_IN_DECK - slot.amount;
								++deckIndex;
							}
						}

						secondMenuCreateInfo.content = deckContent;

						// Try and remove a card from the deck.
						if (leftPressedThisTurn && secondMenuUpdateInfo.hovered && deckSize > 0)
						{
							const size_t interactIndex = secondMenuCreateInfo.GetInteractedColumnIndex(secondMenuUpdateInfo);
							if(interactIndex != SIZE_MAX)
							{
								auto& slot = inventory[cardIndexes[interactIndex]];
								--slot.amount;
								slot.amount = slot.amount == SIZE_MAX ? 0 : slot.amount;
								deckResized = slot.amount == 0;
							}
						}

						// Define what card to draw, if hovered over the deck menu.
						{
							const size_t inventoryCardIndex = menuCreateInfo.GetInteractedColumnIndex(menuUpdateInfo);
							size_t deckCardIndex = deckSize == 0 ? SIZE_MAX : secondMenuCreateInfo.GetInteractedColumnIndex(secondMenuUpdateInfo);
							deckCardIndex = deckCardIndex == SIZE_MAX ? SIZE_MAX : cardIndexes[deckCardIndex];
							cardIndex = secondMenuUpdateInfo.interactedIndex == SIZE_MAX ? menuUpdateInfo.interactedIndex == SIZE_MAX ? SIZE_MAX :
								inventory[inventoryCardIndex].index : deckCardIndex == SIZE_MAX ? SIZE_MAX : inventory[deckCardIndex].index;
						}

						if (deckResized)
							_secondMenuUpdateInfo = {};
						drawSecondWindow = true;
					}
					
					cardIndexes.Free(tempAllocator);
					break;
				}

				if (renderCard)
				{
					menuCreateInfo.xOffset = 1;
					secondMenuCreateInfo.xOffset = 1;
				}
				if (changePage || rightPressedThisTurn || close)
					_secondMenuUpdateInfo = {};
				if (!close && drawSecondWindow)
					menuSys->CreateMenu(info, systems, secondMenuCreateInfo, _secondMenuUpdateInfo);
				if(_menuIndex != MenuIndex::main)
					_menuIndex = rightPressedThisTurn ? MenuIndex::main : _menuIndex;
				if(changePage || rightPressedThisTurn || close)
					menuUpdateInfo = {};
				if(!close)
					menuSys->CreateMenu(info, systems, menuCreateInfo, menuUpdateInfo);
				if (renderCard)
				{
					const auto worldPos = transform.position - entityRenderSys->camera.position;
					const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, cardRenderSys->camera, info.swapChainData->resolution);

					const size_t oldCardHovered = _cardHovered;
					_cardHovered = _cardHovered == SIZE_MAX ? SIZE_MAX : menuUpdateInfo.centerHovered ? _cardHovered : SIZE_MAX;
					cardIndex = cardIndex == SIZE_MAX ? _cardHovered : cardIndex;

					// Draw card.
					{
						const bool newCardHovered = oldCardHovered != cardIndex;
						_cardHovered = cardIndex;
						_cardMenuUpdateInfo = newCardHovered ? CardMenuUpdateInfo() : _cardMenuUpdateInfo;

						CardMenuCreateInfo createInfo{};
						createInfo.origin = transform.position;
						createInfo.cardIndex = cardIndex;

						menuSys->CreateCardMenu(info, systems, createInfo, _cardMenuUpdateInfo);
					}
				}
			}
			else
				_menuUpdateInfo = {};

			// Calculate the average camera center.
			cameraCenter += character.transform.position;

			// Draw the directional arrows based on where the player wants to go.
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
