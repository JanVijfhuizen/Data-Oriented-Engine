#include "precomp.h"
#include "WindowHandler.h"
#include "VkBootstrap.h"
#include "VkApp.h"
#include "VkSwapChain.h"
#include "ImguiImpl.h"

int main()
{
	vke::WindowHandler windowHandler{};
	{
		const vke::WindowHandler::Info windowCreateInfo{};
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
	swapChain.Allocate(allocator, app, windowHandler);

	vke::ImguiImpl imguiImpl{};
	imguiImpl.Setup(app, swapChain, windowHandler);

	game::Start();
	bool quit = false;
	while(!quit)
	{
		windowHandler.BeginFrame(quit);
		const auto cmdBuffer = swapChain.BeginFrame(app);
		vke::ImguiImpl::Beginframe();
		ImGui::ShowDemoWindow();
		game::Update();
		vke::ImguiImpl::EndFrame(cmdBuffer);
		const auto presentResult = swapChain.EndFrame(allocator, app);
		if (presentResult)
			swapChain.Recreate(allocator, app, windowHandler);
	}

	const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
	assert(!idleResult);

	imguiImpl.Cleanup(app);

	swapChain.Free(allocator, app);
	vk::Bootstrap::DestroyApp(app);

	assert(allocator.IsEmpty());
	assert(tempAllocator.IsEmpty());
}
