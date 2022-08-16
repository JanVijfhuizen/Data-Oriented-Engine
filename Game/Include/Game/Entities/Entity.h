#pragma once
#include "Components/Inventory.h"
#include "VkEngine/Components/Transform.h"

namespace game
{
	struct EntityData final
	{
		union
		{
			struct Character final
			{
				Inventory inventory{};
			} character{};

			struct Pickup final
			{
				size_t cardId = SIZE_MAX;
			} pickup;
		};
		
		bool markedForDelete = false;
		glm::vec2 position{};
	};

	struct EntityId final
	{
		size_t index = SIZE_MAX;
		size_t id = SIZE_MAX;

		[[nodiscard]] bool operator==(const EntityId& other) const;
		[[nodiscard]] bool operator!=(const EntityId& other) const;
		[[nodiscard]] operator bool() const;
	};

	struct Entity
	{
		friend class EntitySystem;

		EntityData data{};
		EntityId id{};
		vke::Transform transform{};

	private:
		void BuildData();
	};
}
