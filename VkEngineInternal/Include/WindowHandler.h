#pragma once
#include "StringView.h"
#include "VkRenderer/VkBootstrap.h"

namespace vke
{
	/// <summary>
	/// Handles window creation by using the GLFW library.
	/// </summary>
	class WindowHandler final : public vk::IWindowHandler
	{
	public:
		struct Info final
		{
			glm::ivec2 resolution{ 800, 600 };
			jlb::StringView name = "My Window";
			bool allowResizing = false;

			jlb::SystemManager<EngineData>* systemManager = nullptr;
			EngineData* engineData = nullptr;
		};

		void Allocate(const Info& info);
		void Free() const;

		/// <summary>
		/// Creates a vulkan surface, used by the renderer.
		/// </summary>
		/// <param name="instance">Vulkan instance to create the surface for.</param>
		/// <returns>Vulkan surface.</returns>
		[[nodiscard]] VkSurfaceKHR CreateSurface(VkInstance instance) override;
		/// <summary>
		/// Returns the extensions needed by this window to communicate to vulkan.
		/// </summary>
		/// <returns>Extensions needed to communicate with vulkan.</returns>
		[[nodiscard]] jlb::Array<jlb::StringView> GetRequiredExtensions(jlb::StackAllocator& allocator) override;
		/// <summary>
		/// Returns the amount of vulkan extensions needed by this class.
		/// </summary>
		/// <returns>Amount of vulkan extensions needed by this class.</returns>
		[[nodiscard]] size_t GetRequiredExtensionsCount() override;
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

		/// <summary>
		/// Returns the window resolution.
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] glm::ivec2 GetResolution() override;
		/// <summary>
		/// Returns the GLFW window.
		/// </summary>
		[[nodiscard]] GLFWwindow* GetGLFWWindow() const;
		/// <summary>
		/// Returns the system manager.
		/// </summary>
		[[nodiscard]] jlb::SystemManager<EngineData>& GetSystemManager() const;
		/// <summary>
		/// Returns the engine's OutData. Needed for free function callbacks.
		/// </summary>
		[[nodiscard]] EngineData& GetOutData() const;

	private:
		GLFWwindow* _window = nullptr;
		bool _resized = false;
		glm::ivec2 _resolution{};

		jlb::SystemManager<EngineData>* _systemManager = nullptr;
		EngineData* _outData = nullptr;

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}