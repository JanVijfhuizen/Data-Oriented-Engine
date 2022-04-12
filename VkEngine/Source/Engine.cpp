#include "precomp.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "VkLinearAllocator.h"
#include "VkApp.h"
#include "VkSwapChain.h"

#ifdef _DEBUG
#include "ImguiImpl.h"
#include <fstream>
#include <string>
#endif

namespace vke
{
	const char* MEM_USAGE_PATH = "memUsage.txt";
	constexpr size_t ENGINE_VERSION = 1;

	int Engine::Run()
	{
		RuntimeData runtimeData{};

		VersionData versionData;
		const auto versionResult = LoadVersionData(versionData);
		if(!versionResult)
		{
			jlb::LinearAllocator allocator{ versionData.allocSpace };
			jlb::LinearAllocator tempAllocator{ versionData.tempAllocSpace };

			runtimeData.allocator = &allocator;
			runtimeData.tempAllocator = &tempAllocator;

			const int gameResult = RunGame(runtimeData, true);
			if (gameResult)
				return gameResult;

			SaveVersionData(runtimeData);
			LoadVersionData(versionData);
		}

		jlb::LinearAllocator allocator{ versionData.allocSpace };
		jlb::LinearAllocator tempAllocator{ versionData.tempAllocSpace };
		runtimeData.allocator = &allocator;
		runtimeData.tempAllocator = &tempAllocator;

		const int gameResult = RunGame(runtimeData, false);

		assert(allocator.IsEmpty());
		assert(tempAllocator.IsEmpty());

		if(!gameResult)
			SaveVersionData(runtimeData);
		return gameResult;
	}

	int Engine::RunGame(RuntimeData& data, const bool allocRun)
	{
		auto& allocator = *data.allocator;
		auto& tempAllocator = *data.tempAllocator;

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

			if (allocRun)
				break;
		}

		const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
		assert(!idleResult);

		vkAllocator.Free(allocator, app);

#ifdef _DEBUG
		imguiImpl.Cleanup(app);
#endif

		swapChain.Free(allocator, app);
		vk::Bootstrap::DestroyApp(app);

		windowHandler.Cleanup();
		return 0;
	}

	bool Engine::LoadVersionData(VersionData& outVersionData)
	{
		outVersionData = {};
		outVersionData.buildVersion = ENGINE_VERSION;

#ifdef _DEBUG
		return false;
#endif

		std::ifstream memFile{};
		memFile.open(MEM_USAGE_PATH, std::ios::in);
		if (!memFile.is_open())
			return false;

		bool isValid = false;

		// If the file is empty.
		memFile.seekg(0, std::ios::end);
		if (memFile.tellg())
		{
			memFile.seekg(0, 0);

			std::string s;
			std::getline(memFile, s);
			const size_t version = std::stoi(s);
			
			if (version == ENGINE_VERSION)
			{
				outVersionData.buildVersion = version;
				std::getline(memFile, s);
				outVersionData.allocSpace = std::stoi(s);
				std::getline(memFile, s);
				outVersionData.tempAllocSpace = std::stoi(s);
				isValid = true;
			}
		}

		memFile.close();
		return isValid;
	}

	void Engine::SaveVersionData(RuntimeData& runtimeData)
	{
		std::ofstream memFIle{};
		memFIle.open(MEM_USAGE_PATH, std::ios::out);
		assert(memFIle.is_open());

		memFIle << ENGINE_VERSION << std::endl;;
		memFIle << runtimeData.allocator->GetTotalRequestedSpace() << std::endl;
		memFIle << runtimeData.tempAllocator->GetTotalRequestedSpace() << std::endl;
		
		memFIle.close();
	}
}
