#pragma once

namespace game::layout
{
	struct Info final
	{
		struct Binding final
		{
			VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			size_t size = sizeof(int32_t);
			uint32_t count = 1;
			VkShaderStageFlagBits flag;
		};

		jlb::ArrayView<Binding> bindings{};
	};

	[[nodiscard]] VkDescriptorSetLayout Create(const EngineOutData& outData, Info& info);
}