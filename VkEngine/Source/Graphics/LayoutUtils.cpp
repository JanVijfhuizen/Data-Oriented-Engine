#include "VkEngine/pch.h"
#include "VkEngine/Graphics/LayoutUtils.h"
#include "Array.h"
#include "VkRenderer/VkApp.h"
#include "VkEngine/EngineData.h"

namespace vke::layout
{
	VkDescriptorSetLayout Create(const EngineData& engineInfo, Info& info)
	{
		jlb::Array<VkDescriptorSetLayoutBinding> bindings{};
		bindings.Allocate(*engineInfo.tempAllocator, info.bindings.length);

		for (size_t i = 0; i < bindings.GetLength(); ++i)
		{
			auto& binding = bindings[i];
			const auto& bindingInfo = info.bindings[i];

			binding.binding = i;
			binding.descriptorType = bindingInfo.type;
			binding.descriptorCount = bindingInfo.count;
			binding.stageFlags = bindingInfo.flag;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.pNext = nullptr;
		layoutInfo.flags = 0;
		layoutInfo.bindingCount = bindings.GetLength();
		layoutInfo.pBindings = bindings.GetData();

		VkDescriptorSetLayout layout;
		const auto result = vkCreateDescriptorSetLayout(engineInfo.app->logicalDevice, &layoutInfo, nullptr, &layout);
		assert(!result);

		bindings.Free(*engineInfo.tempAllocator);
		return layout;
	}
}