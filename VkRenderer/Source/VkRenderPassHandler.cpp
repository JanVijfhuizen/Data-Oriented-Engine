#include "pch.h"
#include "VkRenderPassHandler.h"

namespace vk
{
	VkAttachmentDescription RenderPassHandler::CreateAttachmentDescriptionDefaultInfo()
	{
		VkAttachmentDescription attachmentInfo{};
		attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return attachmentInfo;
	}

	VkAttachmentReference RenderPassHandler::CreateAttachmentReferenceDefaultInfo()
	{
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		return colorAttachmentRef;
	}

	VkSubpassDescription RenderPassHandler::CreateSubpassDescriptionDefaultInfo()
	{
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		return subpass;
	}

	VkSubpassDependency RenderPassHandler::CreateSubpassDependencyDefaultInfo()
	{
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		return dependency;
	}

	VkRenderPassCreateInfo RenderPassHandler::CreateDefaultInfo(
		const jlb::ArrayView<VkAttachmentDescription> attachmentDescriptions,
		const jlb::ArrayView<VkSubpassDescription> subpassDescriptions,
		const jlb::ArrayView<VkSubpassDependency> subpassDependencies)
	{
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachmentDescriptions.length;
		renderPassInfo.pAttachments = attachmentDescriptions.data;
		renderPassInfo.subpassCount = subpassDescriptions.length;
		renderPassInfo.pSubpasses = subpassDescriptions.data;
		renderPassInfo.dependencyCount = subpassDependencies.length;
		renderPassInfo.pDependencies = subpassDependencies.data;
		return renderPassInfo;
	}

	VkRenderPassBeginInfo RenderPassHandler::CreateBeginDefaultInfo()
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderArea.offset = { 0, 0 };
		return renderPassInfo;
	}
}
