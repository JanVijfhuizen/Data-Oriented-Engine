#pragma once
#include "TaskSystem.h"
#include "Graphics/SubTexture.h"

namespace game
{
	struct Texture;
	struct HumanoidBodyVisuals;
	struct HumanoidBodyInstance;

	struct HumanoidBodyTask final
	{
		HumanoidBodyVisuals* visuals;
		HumanoidBodyInstance* instance;
	};

	class HumanoidBodySystem final : public TaskSystem<HumanoidBodyTask>
	{
	public:
		[[nodiscard]] static HumanoidBodyVisuals CreateVisualsFromSubTexture(const Texture& texture, glm::vec2 leftTop);
		static void MirrorLeftSide(HumanoidBodyVisuals& visuals);
		[[nodiscard]] static SubTexture MirrorLeftSide(SubTexture& lSide);

	private:
		void Update(const EngineOutData& outData, SystemChain& chain) override;
	};
}
