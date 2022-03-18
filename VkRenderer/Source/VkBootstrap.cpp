#include "pch.h"
#include "VkBootstrap.h"
#include "Array.h"

vk::App vk::Bootstrap::CreateApp(jlb::LinearAllocator& tempAllocator, AppInfo& info)
{
	CheckValidationSupport(tempAllocator, info);
	return {};
}

void vk::Bootstrap::DestroyApp(const App& app)
{
}

void vk::Bootstrap::CheckValidationSupport(jlb::LinearAllocator& tempAllocator, AppInfo& info)
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
}

void vk::Bootstrap::CreateInstance(jlb::LinearAllocator& tempAllocator, AppInfo& info)
{
	const auto appInfo = CreateApplicationInfo(info);
}

VkApplicationInfo vk::Bootstrap::CreateApplicationInfo(AppInfo& info)
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

jlb::Array<jlb::StringView> vk::Bootstrap::GetExtensions(jlb::LinearAllocator& tempAllocator, AppInfo& info)
{
	assert(info.deviceExtensions);

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
