#pragma once

namespace game
{
	class LayoutHandler final
	{
	public:
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

		[[nodiscard]] static VkDescriptorSetLayout Create(const EngineOutData& engineOutData, Info& info);
	};
}