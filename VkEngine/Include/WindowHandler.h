#pragma once
#include "StringView.h"

/// <summary>
/// Handles window creation by using the GLFW library.
/// </summary>
class WindowHandler final
{
public:
	struct Info final
	{
		glm::ivec2 resolution{800, 600};
		jlb::StringView name = "My Window";
	};

	void Construct(const Info& info);
	void Cleanup();

	/// <summary>
	/// Creates a vulkan surface, used by the renderer.
	/// </summary>
	/// <param name="instance">Vulkan instance to create the surface for.</param>
	/// <returns>Vulkan surface.</returns>
	[[nodiscard]] VkSurfaceKHR CreateSurface(VkInstance instance);
	/// <summary>
	/// Starts a new frame.
	/// </summary>
	/// <param name="outQuit">True if the window has been closed by the user.</param>
	void BeginFrame(bool& outQuit) const;
	/// <summary>
	/// If the window has resized, returns true.
	/// </summary>
	/// <returns>True if the window has resized.</returns>
	bool QueryHasResized();

private:
	GLFWwindow* _window = nullptr;
	bool _resized = false;
	glm::ivec2 _resolution{};

	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
};
