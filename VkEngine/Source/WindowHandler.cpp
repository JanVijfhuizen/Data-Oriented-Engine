#include "precomp.h"
#include "WindowHandler.h"

namespace vke
{
	int GLFWwindowMouseState;

	void GLFWKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && GLFWwindowMouseState == GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, 0, 0);
			GLFWwindowMouseState = GLFW_CURSOR_NORMAL;
		}

		const auto self = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		auto& outData = self->GetOutData();
		self->GetSystemManager().OnKeyInput(outData, key, action);
	}

	void GLFWMouseKeyCallback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && GLFWwindowMouseState == GLFW_CURSOR_NORMAL)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, 0, 0);
			GLFWwindowMouseState = GLFW_CURSOR_DISABLED;
		}

		const auto self = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		auto& outData = self->GetOutData();
		self->GetSystemManager().OnMouseInput(outData, button, action);
	}

	void WindowHandler::Allocate(const Info& info)
	{
		// Initialize GLFW for Vulkan.
		const int result = glfwInit();
		assert(result);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, info.allowResizing);

		// Create window.
		const auto& resolution = info.resolution;
		_window = glfwCreateWindow(resolution.x, resolution.y, info.name.GetData(), nullptr, nullptr);
		assert(_window);
		glfwSetWindowUserPointer(_window, this);

		// Set callback for resize.
		glfwSetFramebufferSizeCallback(_window, FramebufferResizeCallback);
		_resolution = info.resolution;
		_systemManager = info.systemManager;
		_outData = info.outData;

		// Set input callback.
		glfwSetKeyCallback(_window, GLFWKeyCallback);
		glfwSetMouseButtonCallback(_window, GLFWMouseKeyCallback);
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		GLFWwindowMouseState = GLFW_CURSOR_DISABLED;
	}

	void WindowHandler::Free() const
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
		// Check for events.
		glfwPollEvents();

		// Check if the user pressed the close button.
		outQuit = glfwWindowShouldClose(_window);
		if (outQuit)
			return;

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

	GLFWwindow* WindowHandler::GetGLFWWindow() const
	{
		return _window;
	}

	jlb::SystemManager<game::EngineOutData>& WindowHandler::GetSystemManager() const
	{
		return *_systemManager;
	}

	const game::EngineOutData& WindowHandler::GetOutData() const
	{
		return *_outData;
	}
}
