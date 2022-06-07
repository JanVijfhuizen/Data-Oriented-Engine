#include "pch.h"
#include "Systems/HumanoidBodySystem.h"
#include "Components/HumanoidBody.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderConventions.h"

namespace game
{
	HumanoidBodyVisuals HumanoidBodySystem::CreateVisualsFromSubTexture(const Texture& texture,
		const glm::vec2 leftTop)
	{
		HumanoidBodyVisuals visuals{};
		auto& head = visuals.head;
		head.leftTop = renderConventions::ENTITY_HEAD_POS;
		head.rightBot = head.leftTop + glm::vec2(renderConventions::ENTITY_HEAD_SIZE);
		auto& torso = visuals.torso;
		torso.leftTop = renderConventions::ENTITY_TORSO_POS;
		torso.rightBot = torso.leftTop + glm::vec2(renderConventions::ENTITY_TORSO_SIZE);
		auto& lArm = visuals.lArm;
		lArm.leftTop = renderConventions::ENTITY_L_ARM_POS;
		lArm.rightBot = lArm.leftTop + glm::vec2(renderConventions::ENTITY_ARM_SIZE);
		auto& lLeg = visuals.lLeg;
		lLeg.leftTop = renderConventions::ENTITY_L_LEG_POS;
		lLeg.rightBot = lLeg.leftTop + glm::vec2(renderConventions::ENTITY_LEG_SIZE);

		MirrorLeftSide(visuals);

		// Make positions texture relative.
		const glm::vec2 mul = 1.f / glm::vec2(texture.resolution);
		for (auto& part : visuals.values)
			for (auto& vec2 : part.values)
				vec2 = leftTop + vec2 * mul;

		return visuals;
	}

	void HumanoidBodySystem::MirrorLeftSide(HumanoidBodyVisuals& visuals)
	{
		visuals.rArm = MirrorLeftSide(visuals.lArm);
		visuals.rLeg = MirrorLeftSide(visuals.lLeg);
	}

	SubTexture HumanoidBodySystem::MirrorLeftSide(SubTexture& lSide)
	{
		SubTexture rSide = lSide;
		for (auto& vec2 : rSide.values)
			vec2 = glm::vec2(renderConventions::ENTITY_SIZE - vec2.x, vec2.y);
		return rSide;
	}

	void HumanoidBodySystem::Update(const EngineOutData& outData, SystemChain& chain)
	{

	}
}
