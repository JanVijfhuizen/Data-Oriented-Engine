#pragma once

namespace game
{
	class InputHandler final
	{
	public:
		/// <summary>
		/// Updates target key axis reference.
		/// </summary>
		/// <param name="value">Value to be updated.</param>
		/// <param name="lKey">Left key that corresponds to the value.</param>
		/// <param name="rKey">Right key that corresponds to the value.</param>
		/// <param name="actionKey">GLFW key that has been used.</param>
		/// <param name="action">GLFW key action that has been used.</param>
		static void UpdateAxis(int& value, int lKey, int rKey, int actionKey, int action);
		/// <summary>
		/// Updates target key reference.
		/// </summary>
		/// <param name="value">Value to be updated.</param>
		/// <param name="key">Key that corresponds to the value.</param>
		/// <param name="actionKey">GLFW key that has been used.</param>
		/// <param name="action">GLFW key action that has been used.</param>
		/// <returns>If the key was updated.</returns>
		static bool UpdateKey(int& value, int key, int actionKey, int action);
	};
}