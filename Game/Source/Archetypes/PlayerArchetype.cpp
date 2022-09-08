﻿#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "JlbMath.h"
#include "JlbString.h"
#include "Systems/CameraSystem.h"
#include "Systems/CardSystem.h"
#include "Systems/MenuSystem.h"
#include "Systems/MouseSystem.h"
#include "Systems/PickupSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TurnSystem.h"
#include "Systems/UIInteractionSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void PlayerArchetype::OnPreUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes,
		jlb::NestedVector<Player>& entities)
	{
		auto& systems = info.systems;
		auto& vkeInfo = *info.vkeInfo;
		const auto cameraSys = systems.Get<CameraSystem>();
		const auto cardPreviewSys = systems.Get<CardPreviewSystem>();
		const auto cardSys = systems.Get<CardSystem>();
		const auto entityRenderSys = systems.Get<vke::EntityRenderSystem>();
		const auto menuSys = systems.Get<MenuSystem>();
		const auto mouseSys = systems.Get<MouseSystem>();
		const auto playerSys = systems.Get<PlayerSystem>();
		const auto turnSys = systems.Get<TurnSystem>();
		const auto uiRenderSys = systems.Get<vke::UIRenderSystem>();
		const auto uiInteractSys = systems.Get<UIInteractionSystem>();

		// Assure that the player is always present.
		assert(entities.GetCount() == 1);

		auto& entity = entities[0];
		auto& characterInput = entity.input;
		
		const bool ifBeginTickEvent = turnSys->GetIfBeginTickEvent();
		if (playerSys->IsPlayerOccupiedNextTurn())
		{
			Reset();
			if (playerSys->pickupEntity && ifBeginTickEvent)
			{
				auto& pickupComponent = entity.pickupComponent;
				pickupComponent.inInstanceId = entity.id;
				pickupComponent.inPickupId = playerSys->pickupEntity;
				pickupComponent.active = true;
			}
		}

		bool occupied = false;
		if (entity.movementComponent.active)
			occupied = true;
		if (entity.pickupComponent.active)
			occupied = true;

		if (occupied)
		{
			_menuUpdateInfo = {};
			_secondMenuUpdateInfo = {};
		}

		// Calculate movement direction, if any.
		if (ifBeginTickEvent)
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

		CharacterArchetype<Player>::OnPreUpdate(info, archetypes, entities);

		auto& dumpAllocator = *vkeInfo.dumpAllocator;
		auto& tempAllocator = *vkeInfo.tempAllocator;

		const size_t uiHoveredObj = uiInteractSys->GetHoveredObject();

		const bool leftPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left);
		const bool rightPressedThisTurn = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::right);
		const bool mouseAction = mouseSys->GetIsPressedThisTurn(MouseSystem::Key::left) && !mouseSys->GetIsUIBlocking();

		const auto& transform = entity.transform;

		const auto hoveredObj = mouseSys->GetHoveredObject();
		const bool hovered = hoveredObj == entity.mouseJobId && hoveredObj != SIZE_MAX;
		const bool menuOpen = mouseAction ? _menuUpdateInfo.opened ? false : hovered : _menuUpdateInfo.opened;

		// Render Player Menu.
		_menuIndex = menuOpen ? _menuIndex : MenuIndex::main;
		if (!occupied && menuOpen)
		{
			MenuJob menuJob{};
			menuJob.interactable = true;
			menuJob.origin = transform.position;
			menuJob.entityCamera = &entityRenderSys->camera;
			menuJob.uiCamera = &uiRenderSys->camera;
			menuJob.interactIds = _menuInteractIds;
			menuJob.maxLength = _menuInteractIds.GetLength() + 1;
			menuJob.width = 7;

			auto secondMenuJob = menuJob;
			secondMenuJob.reverseXAxis = true;
			secondMenuJob.interactIds = _secondMenuInteractIds;
			secondMenuJob.capacity = SIZE_MAX;
			secondMenuJob.usedSpace = SIZE_MAX;
			bool drawSecondWindow = false;

			jlb::Array<MenuJob::Content> content{};
			const auto& inventory = entity.inventory;
			const size_t inventoryCount = inventory.GetCount();

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
				content.Allocate(dumpAllocator, inventoryCount + 1);
				content[0].string = "inventory";
				for (size_t i = 0; i < inventoryCount; ++i)
				{
					const auto card = cardSys->GetCard(inventory[i].index);
					content[i + 1].string = card.name;
				}
				break;
			}
			if (_menuIndex == MenuIndex::deck)
				for (size_t i = 0; i < inventoryCount; ++i)
					content[i + 1].amount = MAX_COPIES_CARD_IN_DECK - inventory[i].amount;

			menuJob.content = content;

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
				cardIndex = _menuUpdateInfo.GetInteractedColumnIndex(menuJob);
				cardIndex = cardIndex == SIZE_MAX ? SIZE_MAX : inventory[cardIndex].index;
				_cardActivated = leftPressedThisTurn && _menuUpdateInfo.hovered ? _cardHovered : oldCardActivated;

				if (_cardActivated != SIZE_MAX)
				{
					const auto hoveredCard = cardSys->GetCard(_cardActivated);

					jlb::Array<MenuJob::Content> deckContent{};
					deckContent.Allocate(dumpAllocator, 3);
					deckContent[0].string = hoveredCard.name;
					deckContent[1].string = "use";
					deckContent[2].string = "drop";
					secondMenuJob.content = deckContent;
					drawSecondWindow = true; 
				}
				if (_cardActivated != oldCardActivated)
					_secondMenuUpdateInfo = {};

				break;
			case MenuIndex::deck:
				renderCard = true;
				// Get what cards are being used in the deck.
				size_t deckSize = 0;
				for (size_t i = 0; i < inventoryCount; ++i)
				{
					const auto& src = content[i + 1];
					deckSize += src.amount != MAX_COPIES_CARD_IN_DECK;
				}

				menuJob.usedSpace = inventoryCount;
				menuJob.capacity = inventory.GetLength();

				// Get all cards in deck.
				jlb::Vector<size_t> cardIndexes{};
				cardIndexes.Allocate(tempAllocator, deckSize);
				for (size_t i = 0; i < inventoryCount; ++i)
				{
					const auto& src = content[i + 1];
					if (src.amount != MAX_COPIES_CARD_IN_DECK)
						cardIndexes.Add(i);
				}

				// Try and add a card to the deck.
				bool deckResized = false;
				if (leftPressedThisTurn && _menuUpdateInfo.hovered)
				{
					const size_t interactIndex = _menuUpdateInfo.GetInteractedColumnIndex(menuJob);
					if (interactIndex != SIZE_MAX)
					{
						auto& slot = inventory[interactIndex];
						slot.amount = jlb::math::Min(slot.amount + 1, MAX_COPIES_CARD_IN_DECK);
						deckResized = slot.amount == 1;
					}
				}

				// Create deck menu.
				{
					jlb::Array<MenuJob::Content> deckContent{};
					deckContent.Allocate(dumpAllocator, deckSize + 1);
					deckContent[0].string = "deck";

					size_t deckIndex = 1;
					for (size_t i = 0; i < inventoryCount; ++i)
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

					secondMenuJob.content = deckContent;

					// Try and remove a card from the deck.
					if (leftPressedThisTurn && _secondMenuUpdateInfo.hovered && deckSize > 0)
					{
						const size_t interactIndex = _secondMenuUpdateInfo.GetInteractedColumnIndex(secondMenuJob);
						if (interactIndex != SIZE_MAX)
						{
							auto& slot = inventory[cardIndexes[interactIndex]];
							--slot.amount;
							slot.amount = slot.amount == SIZE_MAX ? 0 : slot.amount;
							deckResized = slot.amount == 0;
						}
					}

					// Define what card to draw, if hovered over the deck menu.
					{
						const size_t inventoryCardIndex = _menuUpdateInfo.GetInteractedColumnIndex(menuJob);
						size_t deckCardIndex = deckSize == 0 ? SIZE_MAX : _secondMenuUpdateInfo.GetInteractedColumnIndex(secondMenuJob);
						deckCardIndex = deckCardIndex == SIZE_MAX ? SIZE_MAX : cardIndexes[deckCardIndex];
						cardIndex = _secondMenuUpdateInfo.interactedIndex == SIZE_MAX ? _menuUpdateInfo.interactedIndex == SIZE_MAX ? SIZE_MAX :
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
				menuJob.xOffset = 1;
				secondMenuJob.xOffset = 1;
			}

			if (changePage || rightPressedThisTurn || close)
				_secondMenuUpdateInfo = {};

			secondMenuJob.updateInfo = _secondMenuUpdateInfo;
			if (!close && drawSecondWindow)
				_secondMenuJobId = menuSys->TryAdd(vkeInfo, secondMenuJob);

			if (_menuIndex != MenuIndex::main)
				_menuIndex = rightPressedThisTurn ? MenuIndex::main : _menuIndex;
			if (changePage || rightPressedThisTurn || close)
				_menuUpdateInfo = {};

			menuJob.updateInfo = _menuUpdateInfo;
			if (!close)
				_menuJobId = menuSys->TryAdd(vkeInfo, menuJob);

			if (renderCard)
			{
				const size_t oldCardHovered = _cardHovered;
				_cardHovered = _cardHovered == SIZE_MAX ? SIZE_MAX : _menuUpdateInfo.centerHovered ? _cardHovered : SIZE_MAX;
				cardIndex = cardIndex == SIZE_MAX ? _cardHovered : cardIndex;

				// Draw card.
				{
					const bool newCardHovered = oldCardHovered != cardIndex;
					_cardHovered = cardIndex;
					_cardPreviewUpdateInfo = newCardHovered ? CardPreviewJobUpdateInfo() : _cardPreviewUpdateInfo;

					CardPreviewJob cardJob{};
					cardJob.origin = transform.position;
					cardJob.cardIndex = cardIndex;
					cardJob.updateInfo = _cardPreviewUpdateInfo;

					_cardPreviewJobId = cardPreviewSys->TryAdd(vkeInfo, cardJob);
				}
			}
		}
		else
			_menuUpdateInfo = {};

		cameraSys->settings.target = transform.position;

		// Update player system.
		auto& playerSysUpdateInfo = playerSys->_updateInfo;
		for (size_t i = 0; i < 4; ++i)
			playerSysUpdateInfo.keyArrowInput[i] = _movementInput[i].valid;
		playerSysUpdateInfo.position = transform.position;
	}

	void PlayerArchetype::OnPostUpdate(const EntityArchetypeInfo& info, 
		const jlb::Systems<EntityArchetypeInfo> archetypes,
		jlb::NestedVector<Player>& entities)
	{
		CharacterArchetype<Player>::OnPostUpdate(info, archetypes, entities);

		const auto cardPreviewSys = info.systems.Get<CardPreviewSystem>();
		const auto menuSys = info.systems.Get<MenuSystem>();

		const auto& cardPreviewOutput = cardPreviewSys->GetOutput();
		const auto& menuOutput = menuSys->GetOutput();

		if(_menuJobId != SIZE_MAX)
		{
			_menuUpdateInfo = menuOutput[_menuJobId];
			_menuJobId = SIZE_MAX;
		}
		if(_secondMenuJobId != SIZE_MAX)
		{
			_secondMenuUpdateInfo = menuOutput[_secondMenuJobId];
			_secondMenuJobId = SIZE_MAX;
		}
		if(_cardPreviewJobId != SIZE_MAX)
		{
			_cardPreviewUpdateInfo = cardPreviewOutput[_cardPreviewJobId];
			_cardPreviewJobId = SIZE_MAX;
		}
	}

	void PlayerArchetype::OnKeyInput(const EntityArchetypeInfo& info, const jlb::Systems<EntityArchetypeInfo> systems,
		const int key, const int action)
	{
		CharacterArchetype<Player>::OnKeyInput(info, systems, key, action);

		HandleKeyDirectionInput(GLFW_KEY_W, key, action, _movementInput[0], _movementInput[2]);
		HandleKeyDirectionInput(GLFW_KEY_A, key, action, _movementInput[1], _movementInput[3]);
		HandleKeyDirectionInput(GLFW_KEY_S, key, action, _movementInput[2], _movementInput[0]);
		HandleKeyDirectionInput(GLFW_KEY_D, key, action, _movementInput[3], _movementInput[1]);

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			Reset();
	}

	vke::SubTexture PlayerArchetype::DefineSubTextureSet(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto subTexture = resourceSys->GetSubTexture(ResourceSystem::EntitySubTextures::humanoid);
		return subTexture;
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

	void PlayerArchetype::Reset()
	{
		for (auto& movementInput : _movementInput)
			movementInput.valid = movementInput.pressed;
	}
}
