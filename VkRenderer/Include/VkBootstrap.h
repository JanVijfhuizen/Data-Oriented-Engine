#pragma once
#include "Array.h"
#include "StringView.h"
#include "VkApp.h"

namespace jlb 
{
	class LinearAllocator;
}

namespace vk
{
	class IWindowHandler
	{
	public:
		[[nodiscard]] virtual VkSurfaceKHR CreateSurface(VkInstance instance) = 0;
		[[nodiscard]] virtual jlb::Array<jlb::StringView> GetRequiredExtensions(jlb::LinearAllocator& allocator) = 0;
		[[nodiscard]] virtual size_t GetRequiredExtensionsCount() = 0;
	};

	struct AppInfo final
	{
		struct PhysicalDeviceInfo final
		{
			VkPhysicalDevice device;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
		};

		jlb::StringView name;
		jlb::Array<jlb::StringView> validationLayers{};
		jlb::Array<jlb::StringView> deviceExtensions{};

		IWindowHandler* windowHandler = nullptr;

		bool(*isPhysicalDeviceValid)(PhysicalDeviceInfo& info) = nullptr;
		size_t(*getPhysicalDeviceRating)(PhysicalDeviceInfo& info) = nullptr;

		void Free(jlb::LinearAllocator& tempAllocator);
	};

	class Bootstrap final
	{
	public:
		[[nodiscard]] static AppInfo CreateInfo(jlb::LinearAllocator& tempAllocator);
		[[nodiscard]] static App CreateApp(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void DestroyApp(const App& app);

	private:
		struct QueueFamilies final
		{
			union
			{
				struct
				{
					size_t graphics;
					size_t present;
				};

				size_t values[2]
				{
					SIZE_MAX,
					SIZE_MAX
				};
			};

			[[nodiscard]] operator bool() const;
		};

		struct SupportDetails final
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			jlb::Array<VkSurfaceFormatKHR> formats{};
			jlb::Array<VkPresentModeKHR> presentModes{};

			[[nodiscard]] explicit operator bool() const;
			[[nodiscard]] uint32_t GetRecommendedImageCount() const;
			void Free(jlb::LinearAllocator& tempAllocator);
		};

		static void CheckValidationSupport(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void CreateInstance(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app);
		static void EnableValidationLayers(
			AppInfo& info,
			VkDebugUtilsMessengerCreateInfoEXT& debugInfo,
			VkInstanceCreateInfo& instanceInfo);

		static void SelectPhysicalDevice(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app);
		[[nodiscard]] static QueueFamilies GetQueueFamilies(jlb::LinearAllocator& tempAllocator, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
		[[nodiscard]] static bool CheckDeviceExtensionSupport(jlb::LinearAllocator& tempAllocator, VkPhysicalDevice device, jlb::Array<jlb::StringView>& extensions);
		[[nodiscard]] static SupportDetails QuerySwapChainSupport(jlb::LinearAllocator& allocator, App& app, VkPhysicalDevice device);

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
