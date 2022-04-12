#include "precomp.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "VkLinearAllocator.h"
#include "VkApp.h"
#include "VkSwapChain.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#include "ImguiImpl.h"
#endif

namespace vke
{
	const char* VERSION_PATH = 
#ifdef _DEBUG
	"version-debug.txt";
#else
	"version.txt";
#endif
	constexpr size_t ENGINE_VERSION = 1;

	int Engine::Run()
	{
		VersionData versionData;
		const auto versionResult = LoadVersionData(versionData);
		if(!versionResult)
		{
			const int gameResult = RunGame(versionData, true);
			if (gameResult)
				return gameResult;

			SaveVersionData(versionData);
			LoadVersionData(versionData);
		}

		const int gameResult = RunGame(versionData, false);

		if(!gameResult)
			SaveVersionData(versionData);
		return gameResult;
	}

	int Engine::RunGame(VersionData& versionData, bool allocRun)
	{
		jlb::LinearAllocator allocator{ versionData.allocSpace };
		jlb::LinearAllocator tempAllocator{ versionData.tempAllocSpace };

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

		versionData.allocSpace = allocator.GetTotalRequestedSpace();
		versionData.tempAllocSpace = tempAllocator.GetTotalRequestedSpace();

		assert(allocator.IsEmpty());
		assert(tempAllocator.IsEmpty());

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
		memFile.open(VERSION_PATH, std::ios::in);
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

	void Engine::SaveVersionData(VersionData& stats)
	{
		std::ofstream memFIle{};
		memFIle.open(VERSION_PATH, std::ios::out);
		assert(memFIle.is_open());

		memFIle << ENGINE_VERSION << std::endl;;
		memFIle << stats.allocSpace << std::endl;
		memFIle << stats.tempAllocSpace << std::endl;
		
		memFIle.close();
	}
}
