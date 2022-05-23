#include "pch.h"
#include "Graphics/LayoutHandler.h"
#include "Array.h"
#include "VkRenderer/VkApp.h"

namespace game
{
	VkDescriptorSetLayout LayoutHandler::Create(const EngineOutData& outData, Info& info)
	{
		jlb::Array<VkDescriptorSetLayoutBinding> bindings{};
		bindings.Allocate(*outData.tempAllocator, info.bindings.length);

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
		const auto result = vkCreateDescriptorSetLayout(outData.app->logicalDevice, &layoutInfo, nullptr, &layout);
		assert(!result);
		
		bindings.Free(*outData.tempAllocator);
		return layout;
	}
}
