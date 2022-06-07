#pragma once
#include "Graphics/SubTexture.h"
#include "Transform.h"

namespace game
{
	struct HumanoidBodyVisuals final
	{
		union
		{
			struct
			{
				SubTexture torso;
				SubTexture head;
				SubTexture lArm;
				SubTexture rArm;
				SubTexture lLeg;
				SubTexture rLeg;
			};
			SubTexture values[6]{};
		};
	};

	struct HumanoidBodyInstance final
	{
		union
		{
			struct
			{
				Transform torso;
				Transform head;
				Transform lArm;
				Transform rArm;
				Transform lLeg;
				Transform rLeg;
			};
			Transform transforms[6]{};
		};
	};
}
