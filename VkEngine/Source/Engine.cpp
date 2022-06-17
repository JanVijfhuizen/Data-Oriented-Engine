#include "precomp.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "VkRenderer/VkApp.h"
#include "VkRenderer/VkSwapChain.h"
#include "Game/EngineData.h"
#include <chrono>
#include "VkRenderer/VkStackAllocator.h"
#include "SystemManager.h"

namespace vke
{
	int Engine::Run()
	{
		// Create them here so that other objects can reference them.
		game::EngineData outData{};
		jlb::SystemManager<game::EngineData> systemManager;

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

		// Set up the systme manager.
		
		auto systems = systemManager.CreateProxy(allocator, tempAllocator, outData);
		game::DefineSystems(systems);
		systemManager.Allocate(allocator, tempAllocator);

		// Prepare data to be forwarded to the game.
		
		outData.allocator = &allocator;
		outData.tempAllocator = &tempAllocator;
		outData.vkAllocator = &vkAllocator;
		outData.app = &app;
		outData.resolution = swapChain.GetResolution();
		outData.swapChainRenderPass = swapChain.GetRenderPass();
		outData.swapChainImageCount = swapChain.GetLength();
		outData.systems = &systems;

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
			outData.swapChainCommandBuffer = cmdBuffer;
			outData.swapChainImageIndex = swapChain.GetCurrentImageIndex();

			// Update (delta)time.
			auto newTime = std::chrono::high_resolution_clock::now();
			outData.deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(newTime - oldTime).count() * 0.001f;
			outData.time += outData.deltaTime;
			oldTime = newTime;

			{
				double x;
				double y;
				glfwGetCursorPos(windowHandler.GetGLFWWindow(), &x, &y);
				x /= outData.resolution.x;
				y /= outData.resolution.y;
				outData.mousePos.x = x;
				outData.mousePos.y = y;
			}

			// Update the game.
			systemManager.Update(outData);

			// Submit for presentation to the screen.
			const auto presentResult = swapChain.EndFrame(allocator, app);
			// Recreate the swapchain if the swapchain is inadequate/outdated, or if we're in a testrun.
			if (presentResult)
			{
				swapChain.Recreate(allocator, app, windowHandler);
				outData.resolution = swapChain.GetResolution();
				outData.swapChainRenderPass = swapChain.GetRenderPass();
				// Let the game know we have recreated the swapchain.
				systemManager.OnRecreateSwapChainAssets(outData);
			}
		}

		// Make sure all the graphics related operations are finished before stopping the application.
		const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
		assert(!idleResult);

		// Let the game know we're quitting.
		// Free all the systems.
		systemManager.Exit(outData);
		systemManager.Free(allocator, outData);

		// Make sure all the Vulkan assets have been deallocated.
		assert(vkAllocator.IsEmpty());

		// Delete the Vulkan mmeory pools.
		vkAllocator.Free(app);

		// Clean up the Vulkan swapchain.
		swapChain.Free(allocator, app);
		// Destroy the Vulkan app.
		vk::boots::DestroyApp(app);
		// Destroy the window.
		windowHandler.Free();

		// Make sure everyting is properly deallocated.
		assert(tempAllocator.IsEmpty());
		assert(allocator.IsEmpty());

		// Free memory allocations.
		tempAllocator.Free();
		allocator.Free();

		return 0;
	}
}
