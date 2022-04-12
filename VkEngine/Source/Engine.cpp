#include "precomp.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "VkLinearAllocator.h"
#include "VkApp.h"
#include "VkSwapChain.h"

#ifdef _DEBUG
#include "ImguiImpl.h"
#include <iostream>
#include <fstream>
#include <string>
#endif

namespace vke
{
	const char* MEM_USAGE_PATH = "memUsage.txt";

	void Engine::Run()
	{
		size_t allocatorSize = 65536;
		size_t tempAllocatorSize = 65536;

		{
			std::ifstream memFile{};
			memFile.open(MEM_USAGE_PATH, std::ios::in);
			assert(memFile.is_open());

			memFile.seekg(0, std::ios::end);
			if(memFile.tellg())
			{
				memFile.seekg(0, 0);

				std::string s;
				std::getline(memFile, s);
				allocatorSize = std::stoi(s);
				std::getline(memFile, s);
				tempAllocatorSize = std::stoi(s);
			}

			memFile.close();
		}

		jlb::LinearAllocator allocator{ 65536 };
		jlb::LinearAllocator tempAllocator{ 65536 };

		WindowHandler windowHandler{};
		{
			const WindowHandler::Info windowCreateInfo{};
			windowHandler.Construct(windowCreateInfo);
		}

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

		{
			std::ofstream memFIle{};
			memFIle.open(MEM_USAGE_PATH, std::ios::out);
			assert(memFIle.is_open());
			memFIle << allocator.GetTotalRequestedSpace() << std::endl;
			memFIle << tempAllocator.GetTotalRequestedSpace() << std::endl;
			memFIle.close();
		}
	}
}
