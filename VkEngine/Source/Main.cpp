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
		vk::AppInfo createInfo{};
		createInfo.validationLayers.Allocate(tempAllocator, 1, "VK_LAYER_KHRONOS_validation");
		app = vk::Bootstrap::CreateApp(tempAllocator, createInfo);
		createInfo.validationLayers.Free(tempAllocator);
	}

	bool quit = false;
	while(!quit)
	{
		windowHandler.BeginFrame(quit);
	}

	vk::Bootstrap::DestroyApp(app);
}
