#include "pch.h"
#include "Systems/HumanoidBodySystem.h"
#include "Components/HumanoidBody.h"
#include "Graphics/Texture.h"

namespace game
{
	HumanoidBodyVisuals HumanoidBodySystem::CreateVisualsFromSubTexture(const Texture& texture,
		const glm::vec2 leftTop)
	{
		HumanoidBodyVisuals visuals{};
		auto& head = visuals.head;
		head.leftTop = { 5, 0 };
		head.rightBot = head.leftTop + glm::vec2(6);
		auto& torso = visuals.torso;
		torso.leftTop = head.leftTop + glm::vec2(0, 6);
		torso.rightBot = torso.leftTop + glm::vec2(6);
		auto& lArm = visuals.lArm;
		lArm.leftTop = { 0, 5 };
		lArm.rightBot = lArm.leftTop + glm::vec2(4);
		auto& lLeg = visuals.lLeg;
		lLeg.leftTop = torso.leftTop + glm::vec2(-1, 6);
		lLeg.rightBot = lLeg.leftTop + glm::vec2(4);

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
