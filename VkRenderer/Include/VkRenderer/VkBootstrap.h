#pragma once
#include "Array.h"
#include "StringView.h"

namespace jlb 
{
	class LinearAllocator;
}

namespace vk
{
	struct App;

	/// <summary>
	/// Interface for communicating with the Vulkan Bootstrap class.
	/// </summary>
	class IWindowHandler
	{
	public:
		virtual ~IWindowHandler() = default;
		/// <summary>
		/// Create a vulkan surface to render to.
		/// </summary>
		/// <param name="instance">Vulkan application instance.</param>
		[[nodiscard]] virtual VkSurfaceKHR CreateSurface(VkInstance instance) = 0;
		/// <summary>
		/// Gets the required vulkan extensions needed to render to this window.
		/// </summary>
		/// <param name="allocator">Allocator used to create the array.</param>
		/// <returns>Allocated array with the required extensions.</returns>
		[[nodiscard]] virtual jlb::Array<jlb::StringView> GetRequiredExtensions(jlb::LinearAllocator& allocator) = 0;
		/// <summary>
		/// Returns the required extension count.
		/// </summary>
		[[nodiscard]] virtual size_t GetRequiredExtensionsCount() = 0;
		/// <summary>
		/// Returns the window resolution.
		/// </summary>
		[[nodiscard]] virtual glm::ivec2 GetResolution() = 0;
	};

	/// <summary>
	/// Information from which the Vulkan Bootstrap class can create a vulkan application.
	/// </summary>
	struct AppInfo final
	{
		/// <summary>
		/// Contains GPU hardware information.
		/// </summary>
		struct PhysicalDeviceInfo final
		{
			VkPhysicalDevice device;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
		};

		// Name of the application.
		jlb::StringView name;
		// Layers used to debug vulkan. Only enabled during debugging.
		jlb::Array<jlb::StringView> validationLayers{};
		// Required hardware extensions.
		jlb::Array<jlb::StringView> deviceExtensions{};
		// Class that manages the window.
		IWindowHandler* windowHandler = nullptr;
		// Returns whether or not the physical device can be used by the program.
		bool(*isPhysicalDeviceValid)(PhysicalDeviceInfo& info) = nullptr;
		// Returns how qualified the physical device is.
		size_t(*getPhysicalDeviceRating)(PhysicalDeviceInfo& info) = nullptr;
		// Returns the features needed to be enabled for the program to work.
		VkPhysicalDeviceFeatures(*getPhysicalDeviceFeatures)() = nullptr;

		void Free(jlb::LinearAllocator& tempAllocator);
	};

	/// <summary>
	/// Class that simplifies creating a Vulkan application by abstracting the boilerplate code.
	/// </summary>
	class Bootstrap final
	{
	public:
		/// <summary>
		/// Contains swap chain support details for a Vulkan physical device.
		/// </summary>
		struct SwapChainSupportDetails final
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			jlb::Array<VkSurfaceFormatKHR> formats{};
			jlb::Array<VkPresentModeKHR> presentModes{};

			[[nodiscard]] explicit operator bool() const;
			[[nodiscard]] size_t GetRecommendedImageCount() const;
			void Free(jlb::LinearAllocator& tempAllocator);
		};

		/// <summary>
		/// Contains the standard render queue families.
		/// </summary>
		struct QueueFamilies final
		{
			union
			{
				struct
				{
					size_t graphics;
					size_t present;
					size_t transfer;
				};

				size_t values[3]
				{
					SIZE_MAX,
					SIZE_MAX,
					SIZE_MAX
				};
			};

			[[nodiscard]] operator bool() const;
		};

		/// <summary>
		/// Create a default info struct that will be good enough for most Vulkan applications.<br>
		/// For more advanced use, it is adviced to create one yourself.
		/// </summary>
		/// <returns>Info struct from which a Vulkan application can be created.</returns>
		[[nodiscard]] static AppInfo CreateDefaultInfo(jlb::LinearAllocator& tempAllocator);
		/// <summary>
		/// Creates a new Vulkan application.
		/// </summary>
		/// <param name="info">Info struct from which to create the application.</param>
		/// <returns>The created Vulkan application.</returns>
		[[nodiscard]] static App CreateApp(jlb::LinearAllocator& tempAllocator, AppInfo info);
		
		/// <summary>
		/// Destroys a Vulkan application.
		/// </summary>
		/// <param name="app">Application to be destroyed.</param>
		static void DestroyApp(App& app);

		/// <summary>
		/// Returns a struct containing information about the GPU hardware's swap chain support.
		/// </summary>
		/// <param name="app">Vulkan Application.</param>
		[[nodiscard]] static SwapChainSupportDetails QuerySwapChainSupport(jlb::LinearAllocator& allocator, App& app);
		/// <summary>
		/// Returns the GPU's render queue families.
		/// </summary>
		[[nodiscard]] static QueueFamilies GetQueueFamilies(jlb::LinearAllocator& tempAllocator, App& app);

	private:
		static void CheckValidationSupport(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void CreateInstance(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app);
		static void EnableValidationLayers(
			AppInfo& info,
			VkDebugUtilsMessengerCreateInfoEXT& debugInfo,
			VkInstanceCreateInfo& instanceInfo);

		static void SelectPhysicalDevice(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app);
		[[nodiscard]] static QueueFamilies GetQueueFamilies(jlb::LinearAllocator& tempAllocator, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
		[[nodiscard]] static bool CheckDeviceExtensionSupport(jlb::LinearAllocator& tempAllocator, VkPhysicalDevice device, jlb::Array<jlb::StringView>& extensions);
		[[nodiscard]] static SwapChainSupportDetails QuerySwapChainSupport(jlb::LinearAllocator& allocator, App& app, VkPhysicalDevice device);

		static void CreateLogicalDevice(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app);
		static void CreateCommandPool(jlb::LinearAllocator& tempAllocator, App& app);

		[[nodiscard]] static VkApplicationInfo CreateApplicationInfo(AppInfo& info);
		[[nodiscard]] static jlb::Array<jlb::StringView> GetExtensions(jlb::LinearAllocator& allocator, AppInfo& info);

		static void CreateDebugger(App& app);
		static VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);
		static void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		static VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo();
		static VkBool32 DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	};
}
