#include "pch.h"
#include "Handlers/InputHandler.h"

namespace game
{
	void InputHandler::UpdateAxis(int& value, const int lKey, const int rKey, const int actionKey, const int action)
	{
		int interValue;
		bool updated = UpdateKey(interValue, lKey, actionKey, action);
		value -= updated ? interValue * 2 - 1 : 0;

		interValue = 0;
		updated = UpdateKey(interValue, rKey, actionKey, action);
		value += updated ? interValue * 2 - 1 : 0;
	}

	bool InputHandler::UpdateKey(int& value, const int key, const int actionKey, const int action)
	{
		if (key != actionKey)
			return false;

		const int dir = action == GLFW_PRESS ? 1 : action == GLFW_RELEASE ? 0 : -1;
		value = dir == -1 ? value : dir;
		return dir != -1;
	}
}
