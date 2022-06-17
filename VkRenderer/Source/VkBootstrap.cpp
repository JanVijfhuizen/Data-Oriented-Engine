#include "pch.h"
#include "VkBootstrap.h"
#include "VkApp.h"
#include "Array.h"
#include <iostream>
#include "Heap.h"
#include "HashMap.h"
#include "StackVector.h"
#include "ArrayUtils.h"

namespace vk
{
	void AppInfo::Free(jlb::StackAllocator& tempAllocator)
	{
		validationLayers.Free(tempAllocator);
		deviceExtensions.Free(tempAllocator);
	}

	namespace boots
	{
		VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance,
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

		void DestroyDebugUtilsMessengerEXT(const VkInstance instance,
			const VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
		{
			const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
				vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
			if (func)
				func(instance, debugMessenger, pAllocator);
		}

		VkBool32 DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData)
		{
			std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
			return VK_FALSE;
		}

		VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo()
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

		void CreateDebugger(App& app)
		{
#ifdef NDEBUG
			return;
#endif

			auto createInfo = CreateDebugInfo();
			const auto result = CreateDebugUtilsMessengerEXT(app.instance, &createInfo, nullptr, &app.debugger);
			assert(!result);
		}

		AppInfo CreateDefaultInfo(jlb::StackAllocator& tempAllocator)
		{
			AppInfo info{};
			info.validationLayers.Allocate(tempAllocator, 1, "VK_LAYER_KHRONOS_validation");

			info.isPhysicalDeviceValid = [](AppInfo::PhysicalDeviceInfo& info)
			{
				if (!info.features.samplerAnisotropy)
					return false;
				if (!info.features.geometryShader)
					return false;
				return true;
			};
			info.getPhysicalDeviceRating = [](AppInfo::PhysicalDeviceInfo& info)
			{
				size_t score = 0;
				auto& properties = info.properties;

				// Arbitrary increase in score, not sure what to look for to be honest.
				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					score += 1000;
				// Increase the score based on the maximum resolution supported.
				score += properties.limits.maxImageDimension2D;

				return score;
			};
			info.getPhysicalDeviceFeatures = []()
			{
				VkPhysicalDeviceFeatures deviceFeatures{};
				deviceFeatures.samplerAnisotropy = VK_TRUE;
				deviceFeatures.sampleRateShading = VK_TRUE;
				deviceFeatures.geometryShader = VK_TRUE;
				return deviceFeatures;
			};

			return info;
		}

		void CheckValidationSupport(jlb::StackAllocator& tempAllocator, AppInfo& info)
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

		VkApplicationInfo CreateApplicationInfo(AppInfo& info)
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

		jlb::Array<jlb::StringView> GetExtensions(jlb::StackAllocator& allocator, AppInfo& info)
		{
			// Disable debug extensions for release mode.
			uint32_t debugExtensions = 0;
#ifdef _DEBUG
			debugExtensions = 1;
#endif

			const size_t winExtensionsCount = info.windowHandler->GetRequiredExtensionsCount();

			// Merge all extensions into one array.
			const size_t size = winExtensionsCount + debugExtensions;
			jlb::Array<jlb::StringView> extensions{};
			extensions.Allocate(allocator, size);

			auto winExtensions = info.windowHandler->GetRequiredExtensions(allocator);
			jlb::Copy(extensions.GetView(), 0, winExtensionsCount, winExtensions.GetData());
			winExtensions.Free(allocator);

#ifdef _DEBUG
			extensions[extensions.GetLength() - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#endif

			return extensions;
		}

		void EnableValidationLayers(AppInfo& info,
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

		void CreateInstance(jlb::StackAllocator& tempAllocator, AppInfo& info, App& app)
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

		bool CheckDeviceExtensionSupport(jlb::StackAllocator& tempAllocator,
			const VkPhysicalDevice device, jlb::Array<jlb::StringView>& extensions)
		{
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

			jlb::Array<VkExtensionProperties> availableExtensions{};
			availableExtensions.Allocate(tempAllocator, extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.GetData());

			bool found = true;
			for (const auto& extension : extensions)
			{
				found = false;
				for (auto& availableExtension : availableExtensions)
					if (strcmp(extension.GetData(), availableExtension.extensionName) == 0)
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

		QueueFamilies GetQueueFamilies(jlb::StackAllocator& tempAllocator,
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

				if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
					families.transfer = i;

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

		QueueFamilies GetQueueFamilies(jlb::StackAllocator& tempAllocator, App& app)
		{
			return GetQueueFamilies(tempAllocator, app.surface, app.physicalDevice);
		}

		SwapChainSupportDetails QuerySwapChainSupport(
			jlb::StackAllocator& allocator, App& app, const VkPhysicalDevice device)
		{
			SwapChainSupportDetails details{};
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

		SwapChainSupportDetails QuerySwapChainSupport(jlb::StackAllocator& allocator, App& app)
		{
			return QuerySwapChainSupport(allocator, app, app.physicalDevice);
		}

		void SelectPhysicalDevice(jlb::StackAllocator& tempAllocator, AppInfo& info, App& app)
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

		void CreateLogicalDevice(jlb::StackAllocator& tempAllocator, AppInfo& info, App& app)
		{
			auto queueFamilies = GetQueueFamilies(tempAllocator, app.surface, app.physicalDevice);

			const size_t queueFamiliesCount = sizeof queueFamilies.values / sizeof(size_t);
			jlb::StackVector<VkDeviceQueueCreateInfo, queueFamiliesCount> queueCreateInfos{};

			jlb::HashMap<size_t> familyIndexes{};
			familyIndexes.hasher = [](size_t& hashable) {return hashable; };
			familyIndexes.Allocate(tempAllocator, queueFamiliesCount);
			const float queuePriority = 1;

			// Create a queue for each individual queue family.
			// So if a single family handles both graphics and presentation, only create it once.
			// Hence the hashmap.
			for (auto family : queueFamilies.values)
			{
				if (familyIndexes.Contains(family))
					continue;
				familyIndexes.Insert(family);

				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = family;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.Add(queueCreateInfo);
			}

			assert(info.getPhysicalDeviceFeatures);
			auto features = info.getPhysicalDeviceFeatures();

			// Create interface to the selected GPU hardware.
			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.GetCount());
			createInfo.pQueueCreateInfos = queueCreateInfos.GetData();
			createInfo.pEnabledFeatures = &features;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(info.deviceExtensions.GetLength());
			createInfo.ppEnabledExtensionNames = reinterpret_cast<const char**>(info.deviceExtensions.GetData());

			// Only enable debug layers when in debug mode.
			createInfo.enabledLayerCount = 0;
#ifdef _DEBUG
			const auto& validationLayers = info.validationLayers;
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.GetLength());
			createInfo.ppEnabledLayerNames = reinterpret_cast<const char**>(validationLayers.GetData());
#endif

			const auto result = vkCreateDevice(app.physicalDevice, &createInfo, nullptr, &app.logicalDevice);
			assert(!result);

			// Initialize the render queues.
			uint32_t i = 0;
			for (const auto& family : queueFamilies.values)
			{
				vkGetDeviceQueue(app.logicalDevice, family, 0, &app.queues.values[i]);
				i++;
			}

			familyIndexes.Free(tempAllocator);
		}

		void CreateCommandPool(jlb::StackAllocator& tempAllocator, App& app)
		{
			const auto families = GetQueueFamilies(tempAllocator, app.surface, app.physicalDevice);

			// Create a generic pool that will work with any render command, using the graphic command family.
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = families.graphics;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			const auto result = vkCreateCommandPool(app.logicalDevice, &poolInfo, nullptr, &app.commandPool);
			assert(!result);
		}

		App CreateApp(jlb::StackAllocator& tempAllocator, AppInfo info)
		{
			App app{};

			jlb::Array<jlb::StringView> deviceExtensions{};
			deviceExtensions.Allocate(tempAllocator, info.deviceExtensions.GetLength() + 1);
			jlb::Copy(deviceExtensions.GetView(), 0, info.deviceExtensions.GetLength(), info.deviceExtensions.GetData());
			deviceExtensions[deviceExtensions.GetLength() - 1] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
			info.deviceExtensions = deviceExtensions;

			CheckValidationSupport(tempAllocator, info);
			CreateInstance(tempAllocator, info, app);
			CreateDebugger(app);

			assert(info.windowHandler);
			app.surface = info.windowHandler->CreateSurface(app.instance);

			SelectPhysicalDevice(tempAllocator, info, app);
			CreateLogicalDevice(tempAllocator, info, app);
			CreateCommandPool(tempAllocator, app);

			deviceExtensions.Free(tempAllocator);
			return app;
		}

		void DestroyApp(App& app)
		{
			vkDestroyCommandPool(app.logicalDevice, app.commandPool, nullptr);
			vkDestroyDevice(app.logicalDevice, nullptr);

			vkDestroySurfaceKHR(app.instance, app.surface, nullptr);
#ifdef _DEBUG
			DestroyDebugUtilsMessengerEXT(app.instance, app.debugger, nullptr);
#endif
			vkDestroyInstance(app.instance, nullptr);
		}

		QueueFamilies::operator bool() const
		{
			for (const auto& family : values)
				if (family == SIZE_MAX)
					return false;
			return true;
		}

		SwapChainSupportDetails::operator bool() const
		{
			return formats && presentModes;
		}

		size_t SwapChainSupportDetails::GetRecommendedImageCount() const
		{
			// Always try to go for one larger than the minimum capability.
			// More swapchain images mean less time waiting for a previous frame to render.
			size_t imageCount = capabilities.minImageCount + 1;

			const auto& maxImageCount = capabilities.maxImageCount;
			if (maxImageCount > 0 && imageCount > maxImageCount)
				imageCount = maxImageCount;

			return imageCount;
		}

		void SwapChainSupportDetails::Free(jlb::StackAllocator& tempAllocator)
		{
			presentModes.Free(tempAllocator);
			formats.Free(tempAllocator);
		}
	}
}
