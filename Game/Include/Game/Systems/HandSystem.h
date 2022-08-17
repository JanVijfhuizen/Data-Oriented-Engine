#pragma once
#include "Components/HandComponent.h"
#include "VkEngine/Systems/TaskSystemWithOutput.h"

namespace game
{
	class HandSystem final : public vke::TaskSystemWithOutput<HandComponent, HandComponent>
	{
	public:
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<HandComponent>& tasks) override;
	};
}
