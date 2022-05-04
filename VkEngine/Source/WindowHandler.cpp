#include "precomp.h"
#include "WindowHandler.h"

namespace vke
{
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

	void WindowHandler::Cleanup() const
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	VkSurfaceKHR WindowHandler::CreateSurface(const VkInstance instance)
	{
		VkSurfaceKHR surface;
		const auto result = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
		assert(!result);
		return surface;
	}

	jlb::Array<jlb::StringView> WindowHandler::GetRequiredExtensions(jlb::StackAllocator& allocator)
	{
		uint32_t glfwExtensionCount = 0;
		const auto buffer = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// Copy data from buffer into the array.
		jlb::Array<jlb::StringView> extensions{};
		extensions.Allocate(allocator, glfwExtensionCount);
		memcpy(extensions.GetData(), buffer, sizeof(jlb::StringView) * glfwExtensionCount);
		return extensions;
	}

	size_t WindowHandler::GetRequiredExtensionsCount()
	{
		uint32_t glfwExtensionCount = 0;
		glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return glfwExtensionCount;
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

	glm::ivec2 WindowHandler::GetResolution()
	{
		return _resolution;
	}

	void WindowHandler::FramebufferResizeCallback(GLFWwindow* window, const int width, const int height)
	{
		auto self = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		self->_resized = true;
		self->_resolution = { width, height };
	}

	GLFWwindow* WindowHandler::GetGLFWWIndow() const
	{
		return _window;
	}
}