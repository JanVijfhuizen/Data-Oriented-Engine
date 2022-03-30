#include "pch.h"
#include "WindowHandler.h"
#include "VkBootstrap.h"

int main()
{
	WindowHandler windowHandler{};
	{
		const WindowHandler::Info windowCreateInfo{};
		windowHandler.Construct(windowCreateInfo);
	}

	jlb::LinearAllocator allocator{ 65536 };
	jlb::LinearAllocator tempAllocator{ 65536 };

	vk::App app{};
	{
		vk::AppInfo appInfo = vk::Bootstrap::CreateInfo(tempAllocator);

		auto windowExtensions = windowHandler.GetRequiredExtensions(tempAllocator);
		appInfo.deviceExtensions = {};
		appInfo.windowHandler = &windowHandler;

		appInfo.isPhysicalDeviceValid = [](vk::AppInfo::PhysicalDeviceInfo& info)
		{
			if (!info.features.samplerAnisotropy)
				return false;
			if (!info.features.geometryShader)
				return false;
			return true;
		};
		appInfo.getPhysicalDeviceRating = [](vk::AppInfo::PhysicalDeviceInfo& info)
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

		app = vk::Bootstrap::CreateApp(tempAllocator, appInfo);

		windowExtensions.Free(tempAllocator);
		appInfo.Free(tempAllocator);
	}

	bool quit = false;
	while(!quit)
	{
		windowHandler.BeginFrame(quit);
	}

	vk::Bootstrap::DestroyApp(app);

	assert(allocator.IsEmpty());
	assert(tempAllocator.IsEmpty());
}
