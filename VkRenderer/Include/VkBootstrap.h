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
	struct AppInfo final
	{
		jlb::StringView name;
		jlb::Array<jlb::StringView> validationLayers{};
		jlb::Array<jlb::StringView> deviceExtensions{};
	};

	class Bootstrap final
	{
	public:
		[[nodiscard]] static App CreateApp(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void DestroyApp(const App& app);

	private:
		static void CheckValidationSupport(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void CreateInstance(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app);
		static void EnableValidationLayers(
			AppInfo& info,
			VkDebugUtilsMessengerCreateInfoEXT& debugInfo,
			VkInstanceCreateInfo& instanceInfo);

		[[nodiscard]] static VkApplicationInfo CreateApplicationInfo(AppInfo& info);
		[[nodiscard]] static jlb::Array<jlb::StringView> GetExtensions(jlb::LinearAllocator& tempAllocator, AppInfo& info);

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
