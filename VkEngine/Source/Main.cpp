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
		vk::AppInfo defaultAppInfo = vk::Bootstrap::CreateInfo(tempAllocator);
		vk::AppInfo appInfo = defaultAppInfo;

		auto windowExtensions = windowHandler.GetRequiredExtensions(tempAllocator);
		appInfo.deviceExtensions = {};

		// Add the window extensions to the extensions provided by default.
		// Welcome to stack allocation hell.
		const auto defaultDeviceExtensions = defaultAppInfo.deviceExtensions;
		appInfo.deviceExtensions.Allocate(tempAllocator, windowExtensions.GetLength() + defaultDeviceExtensions.GetLength());
		appInfo.deviceExtensions.Copy(0, defaultDeviceExtensions.GetLength(), defaultDeviceExtensions.GetData());
		appInfo.deviceExtensions.Copy(defaultDeviceExtensions.GetLength(), defaultDeviceExtensions.GetLength() + windowExtensions.GetLength(), windowExtensions.GetData());
		appInfo.windowHandler = &windowHandler;

		app = vk::Bootstrap::CreateApp(tempAllocator, appInfo);

		appInfo.deviceExtensions.Free(tempAllocator);
		windowExtensions.Free(tempAllocator);
		defaultAppInfo.Free(tempAllocator);
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
