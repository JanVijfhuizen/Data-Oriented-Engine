#include "precomp.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkLinearAllocator.h"
#include "VkRenderer/VkSwapChain.h"
#include "Game/EngineData.h"
#include <fstream>
#include <string>
#include <chrono>

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
		}

		return RunGame(versionData, false);
	}

	int Engine::RunGame(VersionData& versionData, bool allocRun)
	{
		jlb::LinearAllocator allocator{};
		allocator.Allocate(versionData.allocSpace);
		jlb::LinearAllocator tempAllocator{};
		tempAllocator.Allocate(versionData.tempAllocSpace);
		jlb::LinearAllocator setupAllocator{};
		setupAllocator.Allocate(versionData.setupAllocSpace);

		WindowHandler windowHandler{};
		{
			const WindowHandler::Info windowCreateInfo{};
			windowHandler.Construct(windowCreateInfo);
		}

		vk::App app{};
		{
			vk::AppInfo appInfo = vk::Bootstrap::CreateDefaultInfo(setupAllocator);
			appInfo.windowHandler = &windowHandler;
			app = vk::Bootstrap::CreateApp(setupAllocator, appInfo);
			appInfo.Free(setupAllocator);
		}

		vk::SwapChain swapChain{};
		swapChain.Allocate(allocator, app, windowHandler);

#ifdef _DEBUG
		ImguiImpl imguiImpl{};
		imguiImpl.Setup(app, swapChain, windowHandler);
#endif

		vk::LinearAllocator vkAllocator{};
		vkAllocator.Allocate(allocator, app);

		{
			VersionData::PoolInfo defaultPoolInfo{};
			const bool load = versionData.poolInfos.size() > 0;

			for (size_t i = 0; i < vkAllocator.GetLength(); ++i)
			{
				auto poolInfo = load ? versionData.poolInfos[i] : defaultPoolInfo;
				vkAllocator.AllocatePool(app, poolInfo.size, poolInfo.alignment, i);
			}
		}

		game::EngineOutData outData{};
		outData.allocator = &allocator;
		outData.tempAllocator = &tempAllocator;
		outData.vkAllocator = &vkAllocator;
		outData.app = &app;
		outData.resolution = swapChain.GetResolution();
		outData.swapChainRenderPass = swapChain.GetRenderPass();

		using ms = std::chrono::duration<float, std::milli>;
		auto oldTime = std::chrono::high_resolution_clock::now();

		setupAllocator.Free();
		game::Start(outData);

		bool quit = false;
		while (!quit)
		{
			windowHandler.BeginFrame(quit);
			const auto cmdBuffer = swapChain.BeginFrame(app);

#ifdef _DEBUG
			ImguiImpl::Beginframe();
#endif
			outData.swapChainCommandBuffer = cmdBuffer;

			auto newTime = std::chrono::high_resolution_clock::now();
			outData.deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - oldTime).count() * 0.001f;
			outData.time += outData.deltaTime;
			oldTime = newTime;
			auto inData = game::Update(outData);

#ifdef _DEBUG
			ImguiImpl::EndFrame(cmdBuffer);
#endif

			const auto presentResult = swapChain.EndFrame(allocator, app, inData.swapChainWaitSemaphores);
			if (presentResult || allocRun)
			{
				swapChain.Recreate(allocator, app, windowHandler);
				outData.resolution = swapChain.GetResolution();
				outData.swapChainRenderPass = swapChain.GetRenderPass();
				game::OnRecreateSwapChainAssets(outData);
			}

			if (allocRun)
				break;
		}

		const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
		assert(!idleResult);

		game::Exit(outData);
		assert(vkAllocator.IsEmpty());

		versionData.poolInfos.clear();
		for (size_t i = 0; i < vkAllocator.GetLength(); ++i)
		{
			versionData.poolInfos.push_back({});
			auto& poolInfo = versionData.poolInfos[i];
			vkAllocator.GetPoolInfo(i, poolInfo.size, poolInfo.alignment);
		}
		
		vkAllocator.Free(allocator, app);

#ifdef _DEBUG
		imguiImpl.Cleanup(app);
#endif

		swapChain.Free(allocator, app);
		vk::Bootstrap::DestroyApp(app);

		windowHandler.Cleanup();

		versionData.setupAllocSpace = setupAllocator.GetTotalRequestedSpace();
		versionData.allocSpace = allocator.GetTotalRequestedSpace();
		versionData.tempAllocSpace = tempAllocator.GetTotalRequestedSpace();

		SaveVersionData(versionData);

		assert(tempAllocator.IsEmpty());
		assert(allocator.IsEmpty());

		tempAllocator.Free();
		allocator.Free();

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

				while(std::getline(memFile, s))
				{
					outVersionData.poolInfos.push_back({});
					auto& poolInfo = outVersionData.poolInfos[outVersionData.poolInfos.size() - 1];
					poolInfo.size = std::stoi(s);
					std::getline(memFile, s);
					poolInfo.alignment = std::stoi(s);
				}
				isValid = true;
			}
		}

		memFile.close();
		return isValid;
	}

	void Engine::SaveVersionData(VersionData& versionData)
	{
		std::ofstream memFile{};
		memFile.open(VERSION_PATH, std::ios::out);
		assert(memFile.is_open());

		memFile << ENGINE_VERSION << std::endl;;
		memFile << versionData.setupAllocSpace << std::endl;
		memFile << versionData.allocSpace << std::endl;
		memFile << versionData.tempAllocSpace << std::endl;

		for (auto& poolInfo : versionData.poolInfos)
		{
			memFile << poolInfo.size << std::endl;
			memFile << poolInfo.alignment << std::endl;
		}
		
		memFile.close();
	}
}
