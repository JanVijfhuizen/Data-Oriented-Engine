#include "precomp.h"
#include "Engine.h"
#include "WindowHandler.h"
#include <iostream>

#include "VkLinearAllocator.h"
#include "VkApp.h"
#include "VkSwapChain.h"

#ifdef _DEBUG
#include "ImguiImpl.h"
#endif

namespace vke
{
	void Engine::Run()
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
		swapChain.Allocate(allocator, app, windowHandler);

#ifdef _DEBUG
		ImguiImpl imguiImpl{};
		imguiImpl.Setup(app, swapChain, windowHandler);
#endif

		vk::LinearAllocator vkAllocator{};
		vkAllocator.Allocate(allocator, app);

		game::Start();
		bool quit = false;
		while (!quit)
		{
			windowHandler.BeginFrame(quit);
			const auto cmdBuffer = swapChain.BeginFrame(app);

#ifdef _DEBUG
			ImguiImpl::Beginframe();
#endif

			game::Update();

#ifdef _DEBUG
			ImguiImpl::EndFrame(cmdBuffer);
#endif

			const auto presentResult = swapChain.EndFrame(allocator, app);
			if (presentResult)
				swapChain.Recreate(allocator, app, windowHandler);
		}

		const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
		assert(!idleResult);

		vkAllocator.Free(allocator, app);

#ifdef _DEBUG
		imguiImpl.Cleanup(app);
#endif

		swapChain.Free(allocator, app);
		vk::Bootstrap::DestroyApp(app);

		assert(allocator.IsEmpty());
		assert(tempAllocator.IsEmpty());

		std::cout << "Constant memory used: " << allocator.GetTotalRequestedSpace() << std::endl;
		std::cout << "Temporary memory used: " << tempAllocator.GetTotalRequestedSpace() << std::endl;
	}
}
