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
		const auto cardSystem = systems.GetSystem<CardSystem>();
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

		for (auto& entity : entities)
		{
			auto& character = entity.character;
			PreUpdateCharacter(info, character, characterUpdateInfo, subTexturesDivided[0], characterInput);

			const auto& transform = character.transform;

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
				menuCreateInfo.interactIds = entity.menuInteractIds;
				menuCreateInfo.maxLength = entity.menuInteractIds.GetLength() + 1;
				menuCreateInfo.width = 7;

				jlb::Array<MenuCreateInfo::Content> content{};
				const jlb::ArrayView<InventorySlot> inventory = entity.inventory;

				// Create menu content.
				switch (entity.menuIndex)
				{
				case Player::MenuIndex::main:
					content.Allocate(dumpAllocator, 3);
					content[0].string = "player";
					content[1].string = "inventory";
					content[2].string = "deck";
					break;
				case Player::MenuIndex::inventory:
				case Player::MenuIndex::deck:
					content.Allocate(dumpAllocator, inventory.length + 1);
					content[0].string = "inventory";
					for (size_t i = 0; i < inventory.length; ++i)
					{
						const auto card = cardSystem->GetCard(inventory[i].index);
						content[i + 1].string = card.name;
					}
					break;
				}
				if(entity.menuIndex == Player::MenuIndex::deck)
					for (size_t i = 0; i < inventory.length; ++i)
						content[i + 1].amount = MAX_COPIES_CARD_IN_DECK - inventory[i].amount;

				menuCreateInfo.content = content;

				bool changePage = false;
				bool close = false;

				size_t cardIndex = SIZE_MAX;
				bool renderCard = false;

				// Handle interaction.
				switch (entity.menuIndex)
				{
				case Player::MenuIndex::main:
					if (leftPressedThisTurn)
						for (size_t i = 0; i < 2; ++i)
						{
							const bool columnHovered = uiHoveredObj == entity.menuInteractIds[i];
							changePage = changePage ? true : columnHovered;
							entity.menuIndex = changePage ? static_cast<Player::MenuIndex>(i + 1) : entity.menuIndex;
							i = changePage ? 2 : i;
						}
					close = rightPressedThisTurn;
					break;
				case Player::MenuIndex::inventory:
					renderCard = true;
					entity.menuIndex = rightPressedThisTurn ? Player::MenuIndex::main : entity.menuIndex;
					cardIndex = menuCreateInfo.GetInteractedColumnIndex(entity.menuUpdateInfo);
					cardIndex = cardIndex == SIZE_MAX ? SIZE_MAX : inventory[cardIndex].index;
					break;
				case Player::MenuIndex::deck:
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

					const auto& menuUpdateInfo = entity.menuUpdateInfo;
					const auto& deckMenuUpdateInfo = entity.secondMenuUpdateInfo;

					// Try and add a card to the deck.
					bool deckResized = false;
					if (leftPressedThisTurn && entity.menuUpdateInfo.hovered)
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

						MenuCreateInfo deckMenuCreateInfo = menuCreateInfo;
						deckMenuCreateInfo.reverseXAxis = true;
						deckMenuCreateInfo.content = deckContent;
						deckMenuCreateInfo.interactIds = entity.secondMenuInteractIds;
						deckMenuCreateInfo.capacity = SIZE_MAX;
						deckMenuCreateInfo.usedSpace = SIZE_MAX;
						deckMenuCreateInfo.xOffset = 1;

						// Try and remove a card from the deck.
						if (leftPressedThisTurn && entity.secondMenuUpdateInfo.hovered && deckSize > 0)
						{
							const size_t interactIndex = deckMenuCreateInfo.GetInteractedColumnIndex(deckMenuUpdateInfo);
							if(interactIndex != SIZE_MAX)
							{
								auto& slot = inventory[cardIndexes[interactIndex]];
								--slot.amount;
								slot.amount = slot.amount == SIZE_MAX ? 0 : slot.amount;
								deckResized = slot.amount == 0;
							}
						}

						// Draw card, if applicable.
						{
							const size_t inventoryCardIndex = menuCreateInfo.GetInteractedColumnIndex(menuUpdateInfo);
							size_t deckCardIndex = deckSize == 0 ? SIZE_MAX : deckMenuCreateInfo.GetInteractedColumnIndex(deckMenuUpdateInfo);
							deckCardIndex = deckCardIndex == SIZE_MAX ? SIZE_MAX : cardIndexes[deckCardIndex];
							cardIndex = deckMenuUpdateInfo.interactedIndex == SIZE_MAX ? menuUpdateInfo.interactedIndex == SIZE_MAX ? SIZE_MAX :
								inventory[inventoryCardIndex].index : deckCardIndex == SIZE_MAX ? SIZE_MAX : inventory[deckCardIndex].index;
						}

						if (changePage || rightPressedThisTurn || close || deckResized)
							entity.secondMenuUpdateInfo.Reset();
						if (!close)
							menuSys->CreateMenu(info, systems, deckMenuCreateInfo, entity.secondMenuUpdateInfo);
					}

					entity.menuIndex = rightPressedThisTurn ? Player::MenuIndex::main : entity.menuIndex;
					cardIndexes.Free(tempAllocator);
					break;
				}
				
				if(changePage || rightPressedThisTurn || close)
					entity.menuUpdateInfo.Reset();
				if(renderCard)
					menuCreateInfo.xOffset = 1;
				if(!close)
					menuSys->CreateMenu(info, systems, menuCreateInfo, entity.menuUpdateInfo);
				if (renderCard)
				{
					const auto worldPos = transform.position - entityRenderSys->camera.position;
					const auto screenPos = vke::UIRenderSystem::WorldToScreenPos(worldPos, cardRenderSys->camera, info.swapChainData->resolution);

					const size_t oldCardHovered = entity.cardHovered;
					entity.cardHovered = entity.cardHovered == SIZE_MAX ? SIZE_MAX : entity.menuUpdateInfo.centerHovered ? entity.cardHovered : SIZE_MAX;
					cardIndex = cardIndex == SIZE_MAX ? entity.cardHovered : cardIndex;

					// Draw card.
					{
						_animLerp = oldCardHovered == cardIndex ? _animLerp : 0;
						entity.cardHovered = cardIndex;
						
						const auto cardBorder = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::border);
						const auto& pixelSize = cardRenderSys->camera.pixelSize;

						vke::UIRenderTask cardRenderTask{};
						cardRenderTask.scale = pixelSize * glm::vec2(static_cast<float>(vke::PIXEL_SIZE_ENTITY * 4));
						cardRenderTask.subTexture = cardBorder;
						cardRenderTask.position = screenPos;
						auto result = cardRenderSys->TryAdd(info, cardRenderTask);
						assert(result != SIZE_MAX);

						vke::SubTexture cardSubTexture = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::idle);
						if (cardIndex != SIZE_MAX)
						{
							const Card hoveredCard = cardSystem->GetCard(cardIndex);
							cardSubTexture = hoveredCard.art;

							jlb::String str{};
							str.AllocateFromNumber(dumpAllocator, hoveredCard.cost);

							TextRenderTask textCostTask{};
							textCostTask.center = true;
							textCostTask.origin = screenPos;
							textCostTask.origin.y += cardRenderTask.scale.y * .5f;
							textCostTask.text = str;
							textCostTask.scale = vke::PIXEL_SIZE_ENTITY;
							textCostTask.padding = static_cast<int32_t>(textCostTask.scale) / -2;
							result = textRenderSys->TryAdd(info, textCostTask);
							assert(result != SIZE_MAX);

							TextBoxCreateInfo cardTextBox;
							cardTextBox.origin = screenPos + glm::vec2(0, .5f);
							cardTextBox.text = hoveredCard.text;
							MenuSystem::CreateTextBox(info, systems, cardTextBox);
						}

						_animLerp += info.deltaTime * 0.001f * _animSpeed / CARD_ANIM_LENGTH;
						_animLerp = fmodf(_animLerp, 1);

						vke::Animation cardAnim{};
						cardAnim.lerp = _animLerp;
						cardAnim.width = CARD_ANIM_LENGTH;
						auto sub = cardAnim.Evaluate(cardSubTexture, 0);

						cardRenderTask.subTexture = sub;
						result = cardRenderSys->TryAdd(info, cardRenderTask);
						assert(result != SIZE_MAX);
					}
				}
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
