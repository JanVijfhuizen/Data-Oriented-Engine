#pragma once
#include "ArrayView.h"

namespace vke::pipeline
{
	struct Info final
	{
		struct Module final
		{
			VkShaderModule module;
			VkShaderStageFlagBits flags;
		};

		jlb::ArrayView<VkVertexInputAttributeDescription> vertInputAttribDescriptions;
		jlb::ArrayView<VkVertexInputBindingDescription> vertInputBindingDescriptions;
		glm::ivec2 resolution;
		bool shaderSamplingEnabled = false;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		bool usePushConstant = false;
		size_t pushConstantSize = 0;
		VkCompareOp depthBufferCompareOp = VK_COMPARE_OP_LESS;
		jlb::ArrayView<VkDescriptorSetLayout> layouts{};
		jlb::ArrayView<Module> modules;
		bool depthBufferEnabled = false;
		VkRenderPass renderPass;
		VkPipeline basePipeline = VK_NULL_HANDLE;
		int32_t basePipelineIndex = -1;
		VkPrimitiveTopology topologyType = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	};

	void Create(const EngineData& engineInfo, const Info& info, VkPipelineLayout& outLayout, VkPipeline& outPipeline);
}
