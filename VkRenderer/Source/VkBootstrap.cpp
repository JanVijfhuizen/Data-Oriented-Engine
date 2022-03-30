#include "pch.h"
#include "VkBootstrap.h"
#include "Array.h"
#include <iostream>
#include "Heap.h"

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

		assert(info.windowHandler);
		app.surface = info.windowHandler->CreateSurface(app.instance);

		SelectPhysicalDevice(tempAllocator, info, app);

		return app;
	}

	void Bootstrap::DestroyApp(const App& app)
	{
		vkDestroySurfaceKHR(app.instance, app.surface, nullptr);
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(app.instance, app.debugger, nullptr);
#endif
		vkDestroyInstance(app.instance, nullptr);
	}

	Bootstrap::QueueFamilies::operator bool() const
	{
		for (const auto& family : values)
			if (family == SIZE_MAX)
				return false;
		return true;
	}

	Bootstrap::SupportDetails::operator bool() const
	{
		return formats && presentModes;
	}

	uint32_t Bootstrap::SupportDetails::GetRecommendedImageCount() const
	{
		// Always try to go for one larger than the minimum capability.
		// More swapchain images mean less time waiting for a previous frame to render.
		uint32_t imageCount = capabilities.minImageCount + 1;

		const auto& maxImageCount = capabilities.maxImageCount;
		if (maxImageCount > 0 && imageCount > maxImageCount)
			imageCount = maxImageCount;

		return imageCount;
	}

	void Bootstrap::SupportDetails::Free(jlb::LinearAllocator& tempAllocator)
	{
		formats.Free(tempAllocator);
		presentModes.Free(tempAllocator);
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

	void Bootstrap::SelectPhysicalDevice(jlb::LinearAllocator& tempAllocator, AppInfo& info, App& app)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(app.instance, &deviceCount, nullptr);
		assert(deviceCount);

		jlb::Array<VkPhysicalDevice> devices{};
		devices.Allocate(tempAllocator, deviceCount);
		vkEnumeratePhysicalDevices(app.instance, &deviceCount, devices.GetData());

		jlb::Heap<VkPhysicalDevice> candidates{};
		candidates.Allocate(tempAllocator, deviceCount);

		for (auto& device : devices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;

			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			// If families are not all present, continue.
			const auto families = GetQueueFamilies(tempAllocator, app.surface, device);
			if (!families)
				continue;

			// If extensions are not supported, continue.
			if (!CheckDeviceExtensionSupport(tempAllocator, device, info.deviceExtensions))
				continue;

			// Check swap chain support.
			auto swapChainSupportStr = QuerySwapChainSupport(tempAllocator, app, device);
			const bool swapChainSupport = static_cast<bool>(swapChainSupportStr);
			swapChainSupportStr.Free(tempAllocator);
			if (!swapChainSupport)
				continue;

			AppInfo::PhysicalDeviceInfo physicalDeviceInfo
			{
				device,
				deviceProperties,
				deviceFeatures
			};

			assert(info.isPhysicalDeviceValid);
			assert(info.getPhysicalDeviceRating);

			if (!info.isPhysicalDeviceValid(physicalDeviceInfo))
				continue;

			candidates.Insert(device, info.getPhysicalDeviceRating(physicalDeviceInfo));
		}

		assert(candidates.GetCount() > 0);
		app.physicalDevice = candidates.Peek();

		candidates.Free(tempAllocator);
		devices.Free(tempAllocator);
	}

	Bootstrap::QueueFamilies Bootstrap::GetQueueFamilies(jlb::LinearAllocator& tempAllocator, 
		const VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
	{
		QueueFamilies families{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		jlb::Array<VkQueueFamilyProperties> queueFamilies{};
		queueFamilies.Allocate(tempAllocator, queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.GetData());

		// Check for hardware capabilities.
		uint32_t i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			// If the graphics family is present.
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				families.graphics = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			// Since this is a renderer made for games, we need to be able to render to the screen.
			if (presentSupport)
				families.present = i;

			// If all families have been found.
			if (families)
				break;
			i++;
		}

		queueFamilies.Free(tempAllocator);
		return families;
	}

	bool Bootstrap::CheckDeviceExtensionSupport(jlb::LinearAllocator& tempAllocator, 
		const VkPhysicalDevice device, jlb::Array<jlb::StringView>& extensions)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		jlb::Array<VkExtensionProperties> availableExtensions{};
		availableExtensions.Allocate(tempAllocator, extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.GetData());

		bool found = false;
		for (auto& availableExtension : availableExtensions)
			if (strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, availableExtension.extensionName) == 0)
			{
				found = true;
				break;
			}

		if(found)
			for (const auto& extension : extensions)
			{
				found = false;
				for (auto& availableExtension : availableExtensions)
					if(strcmp(extension.GetData(), availableExtension.extensionName) == 0)
					{
						found = true;
						break;
					}

				if (!found)
					break;
			}

		availableExtensions.Free(tempAllocator);
		return found;
	}

	Bootstrap::SupportDetails Bootstrap::QuerySwapChainSupport(
		jlb::LinearAllocator& allocator, App& app, const VkPhysicalDevice device)
	{
		SupportDetails details{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, app.surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, app.surface, &formatCount, nullptr);

		// Get all supported swap chain formats, if any.
		if (formatCount != 0)
		{
			auto& formats = details.formats;
			formats.Allocate(allocator, formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, app.surface, &formatCount, formats.GetData());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, app.surface, &presentModeCount, nullptr);

		// Get all supported swap chain present modes, if any.
		if (presentModeCount != 0)
		{
			auto& presentModes = details.presentModes;
			presentModes.Allocate(allocator, presentModeCount);

			vkGetPhysicalDeviceSurfacePresentModesKHR(device, app.surface,
				&presentModeCount, presentModes.GetData());
		}

		return details;
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

	jlb::Array<jlb::StringView> Bootstrap::GetExtensions(jlb::LinearAllocator& allocator, AppInfo& info)
	{
		// Disable debug extensions for release mode.
		uint32_t debugExtensions = 0;
#ifdef _DEBUG
		debugExtensions = 1;
#endif

		const size_t winExtensionsCount = info.windowHandler->GetRequiredExtensionsCount();
		const size_t deviceExtensionsCount = info.deviceExtensions.GetLength();

		// Merge all extensions into one array.
		const size_t size = deviceExtensionsCount + winExtensionsCount + debugExtensions;
		jlb::Array<jlb::StringView> extensions{};
		extensions.Allocate(allocator, size);
		extensions.Copy(0, deviceExtensionsCount, info.deviceExtensions.GetData());

		auto winExtensions = info.windowHandler->GetRequiredExtensions(allocator);
		extensions.Copy(deviceExtensionsCount, deviceExtensionsCount + winExtensionsCount, winExtensions.GetData());
		winExtensions.Free(allocator);

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
