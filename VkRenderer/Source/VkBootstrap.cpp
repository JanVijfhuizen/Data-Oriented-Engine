#include "pch.h"
#include "VkBootstrap.h"
#include "Array.h"
#include <iostream>

namespace vk
{
	void AppInfo::Free(jlb::LinearAllocator& tempAllocator)
	{
		validationLayers.Free(tempAllocator);
		deviceExtensions.Free(tempAllocator);
	}

	AppInfo Bootstrap::CreateInfo(jlb::LinearAllocator& tempAllocator)
	{
		AppInfo info{};
		info.validationLayers.Allocate(tempAllocator, 1, "VK_LAYER_KHRONOS_validation");
		return info;
	}

	App Bootstrap::CreateApp(jlb::LinearAllocator& tempAllocator, AppInfo& info)
	{
		App app{};
		CheckValidationSupport(tempAllocator, info);
		CreateInstance(tempAllocator, info, app);
		CreateDebugger(app);
		return app;
	}

	void Bootstrap::DestroyApp(const App& app)
	{
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(app.instance, app.debugger, nullptr);
#endif
		vkDestroyInstance(app.instance, nullptr);
	}

	void Bootstrap::CheckValidationSupport(jlb::LinearAllocator& tempAllocator, AppInfo& info)
	{
#ifdef NDEBUG
		return;
#endif

		assert(info.validationLayers);

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		jlb::Array<VkLayerProperties> availableLayers{};
		availableLayers.Allocate(tempAllocator, layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());

		// Iterate over all the layers to see if they are available.
		for (const auto& layer : info.validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
				if (strcmp(layer, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}

			// If the layer is not available.
			assert(layerFound);
		}

		availableLayers.Free(tempAllocator);
	}

	void Bootstrap::CreateInstance(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app)
	{
		const auto appInfo = CreateApplicationInfo(info);
		auto extensions = GetExtensions(tempAllocator, info);

		// Create the vulkan instance with the selected extensions enabled.
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.GetLength());
		createInfo.ppEnabledExtensionNames = reinterpret_cast<const char**>(extensions.GetData());

		auto validationCreateInfo = CreateDebugInfo();
		EnableValidationLayers(info, validationCreateInfo, createInfo);

		const auto result = vkCreateInstance(&createInfo, nullptr, &app.instance);
		assert(!result);

		extensions.Free(tempAllocator);
	}

	void Bootstrap::EnableValidationLayers(AppInfo& info, 
		VkDebugUtilsMessengerCreateInfoEXT& debugInfo,
	    VkInstanceCreateInfo& instanceInfo)
	{
#ifdef NDEBUG
		instanceInfo.enabledLayerCount = 0;
		return;
#endif

		auto& validationLayers = info.validationLayers;
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.GetLength());
		instanceInfo.ppEnabledLayerNames = reinterpret_cast<const char**>(validationLayers.GetData());
		instanceInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugInfo);
	}

	VkApplicationInfo Bootstrap::CreateApplicationInfo(AppInfo& info)
	{
		const auto& name = info.name.GetData();
		const auto version = VK_MAKE_VERSION(1, 0, 0);

		// Create the info from which the vulkan instance is created.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = name;
		appInfo.applicationVersion = version;
		appInfo.pEngineName = name;
		appInfo.engineVersion = version;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		return appInfo;
	}

	jlb::Array<jlb::StringView> Bootstrap::GetExtensions(jlb::LinearAllocator& tempAllocator, AppInfo& info)
	{
		// Disable debug extensions for release mode.
		uint32_t debugExtensions = 0;
#ifdef _DEBUG
		debugExtensions = 1;
#endif

		// Merge all extensions into one array.
		const size_t size = info.deviceExtensions.GetLength() + debugExtensions;
		jlb::Array<jlb::StringView> extensions{};
		extensions.Allocate(tempAllocator, size);
		extensions.Copy(0, info.deviceExtensions.GetLength(), info.deviceExtensions.GetData());

#ifdef _DEBUG
		extensions[extensions.GetLength() - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#endif

		return extensions;
	}

	void Bootstrap::CreateDebugger(App& app)
	{
#ifdef NDEBUG
		return;
#endif

		auto createInfo = CreateDebugInfo();
		const auto result = CreateDebugUtilsMessengerEXT(app.instance, &createInfo, nullptr, &app.debugger);
		assert(!result);
	}

	VkResult Bootstrap::CreateDebugUtilsMessengerEXT(const VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		if (func)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void Bootstrap::DestroyDebugUtilsMessengerEXT(const VkInstance instance,
		const VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		if (func)
			func(instance, debugMessenger, pAllocator);
	}

	VkDebugUtilsMessengerCreateInfoEXT Bootstrap::CreateDebugInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT info{};
		info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		info.pfnUserCallback = DebugCallback;
		return info;
	}

	inline VkBool32 Bootstrap::DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
}
