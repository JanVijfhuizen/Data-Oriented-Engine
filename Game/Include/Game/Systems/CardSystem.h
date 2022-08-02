#pragma once
#include "StringView.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	struct Entity;

	struct Card final
	{
		jlb::StringView name{};
		jlb::StringView text{};

		size_t cost = 1;
		void (*effect)(Entity& target, Entity& src, size_t multiplier);
		uint32_t tags = 0;

		vke::SubTexture art{};
	};

	class CardSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] Card GetCard(size_t index) const;
		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;

	private:
		jlb::Array<Card> _cards{};
	};
}
