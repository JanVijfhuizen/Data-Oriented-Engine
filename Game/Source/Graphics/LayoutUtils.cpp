#include "pch.h"
#include "Graphics/LayoutUtils.h"
#include "Array.h"
#include "VkRenderer/VkApp.h"

namespace game::layout
{
	VkDescriptorSetLayout Create(const EngineData& EngineData, Info& info)
	{
		jlb::Array<VkDescriptorSetLayoutBinding> bindings{};
		bindings.Allocate(*EngineData.tempAllocator, info.bindings.length);

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
		const auto result = vkCreateDescriptorSetLayout(EngineData.app->logicalDevice, &layoutInfo, nullptr, &layout);
		assert(!result);
		
		bindings.Free(*EngineData.tempAllocator);
		return layout;
	}
}
