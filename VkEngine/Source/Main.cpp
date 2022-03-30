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
		appInfo.windowHandler = &windowHandler;

		app = vk::Bootstrap::CreateApp(tempAllocator, appInfo);

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
