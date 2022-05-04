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
	// Path for the version data used for checking/storing memory requirements.
	const char* VERSION_PATH = 
#ifdef _DEBUG
	"version-debug.txt";
#else
	"version.txt";
#endif
	// Used in release mode to check if the version data needs to be refreshed.
	constexpr size_t ENGINE_VERSION = 1;

	int Engine::Run()
	{
		VersionData versionData;
		const auto versionResult = LoadVersionData(versionData);
		// If the version data is out of date.
		if(!versionResult)
		{
			// Go over all the phases except for the main loop.
			for (uint32_t phase = 0; phase != static_cast<uint32_t>(Phase::Main); phase++)
			{
				const int result = RunGame(versionData, static_cast<Phase>(phase));
				// If it returns an error, abort the process and return said error.
				if (result)
					return result;
			}
		}

		// Run the game normally, with the proper memory requirements taken into account.
		return RunGame(versionData, Phase::Main);
	}

	int Engine::RunGame(VersionData& versionData, Phase phase)
	{
		// Set up the allocators.
		jlb::StackAllocator allocator{};
		allocator.Allocate();
		jlb::StackAllocator tempAllocator{};
		tempAllocator.Allocate();
		jlb::StackAllocator setupAllocator{};
		setupAllocator.Allocate();

		// Set up the window handler.
		WindowHandler windowHandler{};
		{
			const WindowHandler::Info windowCreateInfo{};
			windowHandler.Construct(windowCreateInfo);
		}

		// Set up the vulkan application.
		vk::App app{};
		{
			vk::AppInfo appInfo = vk::Bootstrap::CreateDefaultInfo(setupAllocator);
			appInfo.windowHandler = &windowHandler;
			app = vk::Bootstrap::CreateApp(setupAllocator, appInfo);
			appInfo.Free(setupAllocator);
		}

		// Set up the Vulkan swapchain.
		vk::SwapChain swapChain{};
		swapChain.Allocate(allocator, app, windowHandler);

#ifdef _DEBUG
		// Set up IMGUI.
		ImguiImpl imguiImpl{};
		imguiImpl.Setup(app, swapChain, windowHandler);
		size_t FPSMA[50]{};
		size_t FPSIndex = 0;
#endif

		// Set up the Vulkan pool allocator.
		vk::LinearAllocator vkAllocator{};
		vkAllocator.Allocate(allocator, app);

		{
			// Allocate Vulkan pools based on the memory requirements.
			VersionData::VkPoolInfo defaultPoolInfo{};
			const bool load = versionData.poolInfos.size() > 0;

			for (size_t i = 0; i < vkAllocator.GetLength(); ++i)
			{
				auto poolInfo = load ? versionData.poolInfos[i] : defaultPoolInfo;
				vkAllocator.DefinePool(poolInfo.size, poolInfo.alignment, i);
			}
		}

		// Prepare data to be forwarded to the game.
		game::EngineOutData outData{};
		outData.allocator = &allocator;
		outData.tempAllocator = &tempAllocator;
		outData.vkAllocator = &vkAllocator;
		outData.app = &app;
		outData.resolution = swapChain.GetResolution();
		outData.swapChainRenderPass = swapChain.GetRenderPass();
		outData.swapChainImageCount = swapChain.GetLength();

		// Set up a clock.
		using ms = std::chrono::duration<float, std::milli>;
		auto oldTime = std::chrono::high_resolution_clock::now();

		// Delete the allocations made during the setup step, and make sure everything is properly deallocated.
		assert(setupAllocator.IsEmpty());
		setupAllocator.Free();
		// Start the game.
		game::Start(outData);

		bool quit = false;
		while (!quit)
		{
			// Start a new frame.
			windowHandler.BeginFrame(quit);
			const auto cmdBuffer = swapChain.BeginFrame(app);

#ifdef _DEBUG
			ImguiImpl::Beginframe();
#endif
			// Update swap chain information.
			outData.swapChainCommandBuffer = cmdBuffer;
			outData.swapChainImageIndex = swapChain.GetCurrentImageIndex();

			// Update (delta)time.
			auto newTime = std::chrono::high_resolution_clock::now();
			outData.deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - oldTime).count() * 0.001f;
			outData.time += outData.deltaTime;
			oldTime = newTime;

			// Update the game.
			auto inData = game::Update(outData);

#ifdef _DEBUG
			FPSMA[FPSIndex] = static_cast<size_t>(1000.f / outData.deltaTime);
			size_t FPSMALength = sizeof FPSMA / sizeof(size_t);
			FPSIndex = (FPSIndex + 1) % FPSMALength;
			size_t FPSMACombined = 0;

			for (size_t i = 0; i < FPSMALength; ++i)
				FPSMACombined += FPSMA[i];
			FPSMACombined /= FPSMALength;

			ImGui::Begin("Debug Info", 0, ImGuiWindowFlags_AlwaysAutoResize);
			std::string fpsText = "FPS:" + std::to_string(FPSMACombined);
			ImGui::Text("%s", fpsText.c_str());
			std::string depthText = "Depth:" + std::to_string(allocator.GetDepth());
			std::string tempDepthText = "Temp Depth:" + std::to_string(tempAllocator.GetDepth());
			ImGui::Text("%s", depthText.c_str());
			ImGui::Text("%s", tempDepthText.c_str());
			ImGui::End();

			ImguiImpl::EndFrame(cmdBuffer);
#endif

			// Submit for presentation to the screen.
			const auto presentResult = swapChain.EndFrame(allocator, app, inData.swapChainWaitSemaphores);
			// Recreate the swapchain if the swapchain is inadequate/outdated, or if we're in a testrun.
			if (presentResult || phase != Phase::Main)
			{
				swapChain.Recreate(allocator, app, windowHandler);
				outData.resolution = swapChain.GetResolution();
				outData.swapChainRenderPass = swapChain.GetRenderPass();
				// Let the game know we have recreated the swapchain.
				game::OnRecreateSwapChainAssets(outData);
			}

			// Break after a single frame if this is a test run.
			if (phase != Phase::Main)
				break;
		}

		// Make sure all the graphics related operations are finished before stopping the application.
		const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
		assert(!idleResult);

		// Let the game know we're quitting.
		game::Exit(outData);
		// Make sure all the Vulkan assets have been deallocated.
		assert(vkAllocator.IsEmpty());

		// Update the pool information for the version data.
		versionData.poolInfos.clear();
		for (size_t i = 0; i < vkAllocator.GetLength(); ++i)
		{
			versionData.poolInfos.push_back({});
			auto& poolInfo = versionData.poolInfos[i];
			VkDeviceSize tempSize;
			vkAllocator.GetPoolInfo(i, phase == Phase::VkPoolAlignmentCheck ? tempSize : poolInfo.size, poolInfo.alignment);
		}

		// Delete the Vulkan mmeory pools.
		vkAllocator.Free(allocator, app);

#ifdef _DEBUG
		// Clean up IMGUI.
		imguiImpl.Cleanup(app);
#endif

		// Clean up the Vulkan swapchain.
		swapChain.Free(allocator, app);
		// Destroy the Vulkan app.
		vk::Bootstrap::DestroyApp(app);
		// Destroy the window.
		windowHandler.Cleanup();

		// Store the version data.
		SaveVersionData(versionData);

		// Make sure everyting is properly deallocated.
		assert(tempAllocator.IsEmpty());
		assert(allocator.IsEmpty());

		// Free memory allocations.
		tempAllocator.Free();
		allocator.Free();

		return 0;
	}

	bool Engine::LoadVersionData(VersionData& outVersionData)
	{
		outVersionData = {};
		// Store build version for validation comparisons.
		outVersionData.buildVersion = ENGINE_VERSION;

#ifdef _DEBUG
		return false;
#endif

		// Open file.
		std::ifstream memFile{};
		memFile.open(VERSION_PATH, std::ios::in);
		// If no file has been found, return false and use the default version data.
		if (!memFile.is_open())
			return false;

		// Whether or not the version dat is valid and still up to date.
		bool isValid = false;

		// If the file is empty.
		memFile.seekg(0, std::ios::end);
		if (memFile.tellg())
		{
			memFile.seekg(0, 0);

			std::string s;
			std::getline(memFile, s);
			const size_t version = std::stoi(s);

			// If it's a different version, memory requirements are likely different.
			if (version == ENGINE_VERSION)
			{
				// Load memory requirements.
				outVersionData.buildVersion = version;

				// Load Vulkan memory pools requirements.
				while(std::getline(memFile, s))
				{
					if (s.empty())
						break;

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

		// Store memory requirements and engine version.
		memFile << ENGINE_VERSION << std::endl;

		// Store Vulkan memory pool requirements.
		for (auto& poolInfo : versionData.poolInfos)
		{
			memFile << poolInfo.size << std::endl;
			memFile << poolInfo.alignment << std::endl;
		}
		
		memFile.close();
	}
}
