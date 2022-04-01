#include "pch.h"
#include "WindowHandler.h"
#include "VkBootstrap.h"
#include "VkApp.h"
#include "VkSwapChain.h"
#include "imgui/imgui.h"

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
		vk::AppInfo appInfo = vk::Bootstrap::CreateDefaultInfo(tempAllocator);
		appInfo.windowHandler = &windowHandler;

		app = vk::Bootstrap::CreateApp(tempAllocator, appInfo);

		appInfo.Free(tempAllocator);
	}

	vk::SwapChain swapChain{};
	swapChain.Allocate(allocator, app);
	swapChain.Recreate(allocator, app, windowHandler);

	bool quit = false;
	while(!quit)
	{
		windowHandler.BeginFrame(quit);
		const auto cmdBuffer = swapChain.BeginFrame(app);
		const auto presentResult = swapChain.EndFrame(allocator, app);
		if (presentResult)
			swapChain.Recreate(allocator, app, windowHandler);
	}

	swapChain.Free(allocator, app);
	vk::Bootstrap::DestroyApp(app);

	assert(allocator.IsEmpty());
	assert(tempAllocator.IsEmpty());
}
