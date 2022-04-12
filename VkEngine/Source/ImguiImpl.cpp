#include "precomp.h"
#include "ImguiImpl.h"
#include "VkRenderer/VkApp.h"
#include "WindowHandler.h"
#include "VkRenderer/VkSwapChain.h"
#include "VkRenderer/VkCommandHandler.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace vke
{
	void ImguiImpl::Setup(vk::App& app, vk::SwapChain& swapChain, WindowHandler& windowHandler)
	{
		//Creating descriptor pool for IMGUI
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = sizeof pool_sizes / sizeof(VkDescriptorPoolSize);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(app.logicalDevice, &pool_info, nullptr, &_imguiPool);

		//Initialize IMGUI library
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForVulkan(windowHandler.GetGLFWWIndow(), true);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = app.instance;
		init_info.PhysicalDevice = app.physicalDevice;
		init_info.Device = app.logicalDevice;
		init_info.Queue = app.queues.graphics;
		init_info.DescriptorPool = _imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info, swapChain.GetRenderPass());

		auto cmdBufferInfo = vk::CommandHandler::CreateBufferDefaultInfo(app);
		VkCommandBuffer cmdBuffer;
		auto result = vkAllocateCommandBuffers(app.logicalDevice, &cmdBufferInfo, &cmdBuffer);
		assert(!result);

		auto cmdBeginInfo = vk::CommandHandler::CreateBufferBeginDefaultInfo();
		vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

		ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);
		result = vkEndCommandBuffer(cmdBuffer);
		assert(!result);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(app.queues.graphics, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(app.queues.graphics);

		vkFreeCommandBuffers(app.logicalDevice, app.commandPool, 1, &cmdBuffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void ImguiImpl::Cleanup(vk::App& app) const
	{
		vkDestroyDescriptorPool(app.logicalDevice, _imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImguiImpl::Beginframe()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImguiImpl::EndFrame(const VkCommandBuffer cmdBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
	}
}
