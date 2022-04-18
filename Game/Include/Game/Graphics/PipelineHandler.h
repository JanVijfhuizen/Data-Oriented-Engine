#pragma once
#include "ArrayView.h"

namespace game
{
	class PipelineHandler final
	{
	public:
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
			bool shaderSamplingEnabled = true;
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
		};

		static void CreatePipeline(const EngineOutData& engineOutData, const Info& info, VkPipelineLayout& outLayout, VkPipeline& outPipeline);
	};
}
