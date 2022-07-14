#include "Curve.h"
#include "JlbMath.h"

namespace jlb
{
	float Curve::Evaluate(float lerp)
	{
		lerp = math::Clamp<float>(lerp, 0, 1);
        
        glm::vec2 points[4]{};
        points[1] = operator[](0);
        points[2] = operator[](1);
        points[3] = glm::vec2(1);

		const float y = powf(1 - lerp, 3) * points[0].y + 3 * lerp * powf(1 - lerp, 2) *
            points[1].y + 3 * powf(lerp, 2) * (1 - lerp) * points[2].y
            + powf(lerp, 3) * points[3].y;

        return y;
	}

    Curve::Curve(const glm::vec2& a, const glm::vec2& b) : StackArray<glm::vec2, 2>()
	{
		StackArray<glm::vec2, 2>::operator[](0) = a;
		StackArray<glm::vec2, 2>::operator[](1) = b;
	}

    float DoubleCurveEvaluate(const float lerp, Curve& up, Curve& down)
    {
        return lerp > .5f ? 1.f - down.Evaluate((lerp - .5f) * 2) : up.Evaluate(lerp * 2);
    }

    Curve CreateCurveOvershooting()
    {
        return
        {
            glm::vec2(.41f, .17f),
            glm::vec2(.34f, 1.37f)
        };
    }

    Curve CreateCurveDecelerate()
    {
        return
        {
            glm::vec2(.23f, 1.05f),
            glm::vec2(1)
        };
    }
}
