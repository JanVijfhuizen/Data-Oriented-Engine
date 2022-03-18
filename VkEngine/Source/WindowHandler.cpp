#include "pch.h"
#include "WindowHandler.h"

void WindowHandler::Construct(const Info& info)
{
	// Initialize GLFW for Vulkan.
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Create window.
	const auto& resolution = info.resolution;
	_window = glfwCreateWindow(resolution.x, resolution.y, info.name.GetData(), nullptr, nullptr);
	assert(_window);
	glfwSetWindowUserPointer(_window, this);

	// Set callback for resize.
	glfwSetFramebufferSizeCallback(_window, FramebufferResizeCallback);
	_resolution = info.resolution;
}

void WindowHandler::Cleanup()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}

VkSurfaceKHR WindowHandler::CreateSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	const auto result = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
	assert(!result);
	return surface;
}

void WindowHandler::BeginFrame(bool& outQuit) const
{
	// Check if the user pressed the close button.
	outQuit = glfwWindowShouldClose(_window);
	if (outQuit)
		return;

	// Check for events.
	glfwPollEvents();

	int32_t width = 0, height = 0;
	glfwGetFramebufferSize(_window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(_window, &width, &height);
		glfwWaitEvents();
	}
}

bool WindowHandler::QueryHasResized()
{
	const bool resized = _resized;
	_resized = false;
	return resized;
}

void WindowHandler::FramebufferResizeCallback(GLFWwindow* window, const int width, const int height)
{
	auto self = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
	self->_resized = true;
	self->_resolution = {width, height};
}
