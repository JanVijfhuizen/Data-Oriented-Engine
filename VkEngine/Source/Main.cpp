#include "precomp.h"
#include "WindowHandler.h"
#include "VkBootstrap.h"
#include "VkApp.h"
#include "VkSwapChain.h"
#include "ImguiImpl.h"
#include "VkLinearAllocator.h"
#include "VkBufferHandler.h"

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

	vk::LinearAllocator vkAllocator{};
	vkAllocator.Allocate(allocator, app);

	// Testing.
	VkBufferCreateInfo info = vk::BufferHandler::CreateBufferDefaultInfo(sizeof(size_t) * 35, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	VkBuffer buffer;
	vkCreateBuffer(app.logicalDevice, &info, nullptr, &buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(app.logicalDevice, buffer, &memRequirements);
	auto poolId = vk::LinearAllocator::GetPoolId(app, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkAllocator.DefineAlignment(memRequirements.alignment, poolId);
	vkAllocator.Reserve(sizeof(size_t) * 35, poolId);
	vkAllocator.Reserve(sizeof(size_t) * 35, poolId);
	vkAllocator.Compile(app);

	auto block = vkAllocator.CreateBlock(sizeof(size_t) * 35, poolId);
	auto block2 = vkAllocator.CreateBlock(sizeof(size_t) * 21, poolId);

	vkBindBufferMemory(app.logicalDevice, buffer, block2.memory, block2.offset);

	vkAllocator.FreeBlock(block2);
	vkAllocator.FreeBlock(block);

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

	vkDestroyBuffer(app.logicalDevice, buffer, nullptr);

	const auto idleResult = vkDeviceWaitIdle(app.logicalDevice);
	assert(!idleResult);

	vkAllocator.Free(allocator, app);
	imguiImpl.Cleanup(app);

	swapChain.Free(allocator, app);
	vk::Bootstrap::DestroyApp(app);

	assert(allocator.IsEmpty());
	assert(tempAllocator.IsEmpty());
}
