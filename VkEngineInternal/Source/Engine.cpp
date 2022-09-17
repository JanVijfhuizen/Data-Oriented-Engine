#include "pch.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkSwapChain.h"
#include <chrono>

#include "JlbMath.h"
#include "VkRenderer/VkStackAllocator.h"
#include "SystemManager.h"

namespace vke
{
	int Engine::Run()
	{
		// Create them here so that other objects can reference them.
		EngineData outData{};
		jlb::SystemManager<EngineData> systemManager{};

		// Set up the allocators.
		jlb::StackAllocator allocator{};
		allocator.Allocate();
		jlb::StackAllocator tempAllocator{};
		tempAllocator.Allocate();
		jlb::StackAllocator dumpAllocator{};
		dumpAllocator.Allocate();
		jlb::StackAllocator setupAllocator{};
		setupAllocator.Allocate();

		// Set up the window handler.
		WindowHandler windowHandler{};
		{
			WindowHandler::Info windowCreateInfo{};
			windowCreateInfo.systemManager = &systemManager;
			windowCreateInfo.engineData = &outData;
			windowHandler.Allocate(windowCreateInfo);
		}

		// Set up the vulkan application.
		vk::App app{};
		{
			vk::AppInfo appInfo = vk::boots::CreateDefaultInfo(setupAllocator);
			appInfo.windowHandler = &windowHandler;
			app = vk::boots::CreateApp(setupAllocator, appInfo);
			appInfo.Free(setupAllocator);
		}

		// Set up the Vulkan swapchain.
		vk::SwapChain swapChain{};
		swapChain.Allocate(allocator, app, windowHandler);

		// Set up the Vulkan pool allocator.
		vk::StackAllocator vkAllocator{};
		vkAllocator.Allocate(app);

		const jlb::Systems<EngineData> systems = systemManager;

		// Prepare data to be forwarded to the game.
		EngineSwapChainData engineSwapChainData{};
		engineSwapChainData.resolution = swapChain.GetResolution();
		engineSwapChainData.renderPass = swapChain.GetRenderPass();
		engineSwapChainData.imageCount = swapChain.GetLength();
		outData.allocator = &allocator;
		outData.tempAllocator = &tempAllocator;
		outData.dumpAllocator = &dumpAllocator;
		outData.vkAllocator = &vkAllocator;
		outData.app = &app;
		outData.swapChainData = &engineSwapChainData;
		outData.systems = systems;

		// Set up the systems.
		{
			const auto systemsInitializer = systemManager.CreateInitializer(allocator, tempAllocator, outData);
			game::DefineSystems(systemsInitializer);
		}
		systemManager.Allocate(allocator, tempAllocator);

		// Set up a clock.
		using ms = std::chrono::duration<float, std::milli>;
		auto oldTime = std::chrono::high_resolution_clock::now();

		// Delete the allocations made during the setup step, and make sure everything is properly deallocated.
		assert(setupAllocator.IsEmpty());
		setupAllocator.Free();

		// Start the game.
		systemManager.Awake(outData);
		systemManager.Start(outData);

		bool quit = false;
		while (!quit)
		{
			// Start a new frame.
			windowHandler.BeginFrame(quit);
			const auto cmdBuffer = swapChain.BeginFrame(app);

			// Update swap chain information.
			engineSwapChainData.commandBuffer = cmdBuffer;
			engineSwapChainData.imageIndex = swapChain.GetCurrentImageIndex();

			// Update (delta)time.
			auto newTime = std::chrono::high_resolution_clock::now();
			outData.deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - oldTime).count() * 0.001f;
			outData.deltaTime = jlb::math::Min(outData.deltaTime, 1000.f - 1e-5f);
			outData.time += outData.deltaTime;
			oldTime = newTime;

			{
				double x;
				double y;
				glfwGetCursorPos(windowHandler.GetGLFWWindow(), &x, &y);

				x /= engineSwapChainData.resolution.x;
				y /= engineSwapChainData.resolution.y;

				x = x * 2 - 1;
				y = y * 2 - 1;

				auto& mousePos = outData.mousePos;
				mousePos.x = x;
				mousePos.y = y;

				outData.mouseAvailable = mousePos.x > -1 && mousePos.x < 1 && mousePos.y > -1 && mousePos.y < 1;
			}

			// Update the game.
			systemManager.BeginFrame(outData);
			systemManager.PreUpdate(outData);
			systemManager.Update(outData);
			systemManager.PostUpdate(outData);
			systemManager.EndFrame(outData);

			// Submit for presentation to the screen.
			const auto presentResult = swapChain.EndFrame(allocator, app);
			// Recreate the swapchain if the swapchain is inadequate/outdated, or if we're in a testrun.
			if (presentResult)
			{
				swapChain.Recreate(allocator, app, windowHandler);
				engineSwapChainData.resolution = swapChain.GetResolution();
				engineSwapChainData.renderPass = swapChain.GetRenderPass();
				// Let the game know we have recreated the swapchain.
				systemManager.OnRecreateSwapChainAssets(outData);
			}
			
			dumpAllocator.Clear();
		}

		// Make sure all the graphics related operations are finished before stopping the application.
		const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
		assert(!idleResult);

		// Let the game know we're quitting.
		// Free all the systems.
		systemManager.Exit(outData);
		systemManager.Free(allocator, tempAllocator, outData);

		// Make sure all the Vulkan assets have been deallocated.
		assert(vkAllocator.IsEmpty());

		// Delete the Vulkan memory pools.
		vkAllocator.Free(app);

		// Clean up the Vulkan swapchain.
		swapChain.Free(allocator, app);
		// Destroy the Vulkan app.
		vk::boots::DestroyApp(app);
		// Destroy the window.
		windowHandler.Free();

		// Make sure everything is properly deallocated.
		assert(tempAllocator.IsEmpty());
		assert(allocator.IsEmpty());

		// Free memory allocations.
		dumpAllocator.Free();
		tempAllocator.Free();
		allocator.Free();

		return 0;
	}
}
