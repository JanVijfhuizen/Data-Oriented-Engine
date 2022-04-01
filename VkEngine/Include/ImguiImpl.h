#pragma once

namespace vk 
{
	class SwapChain;
	struct App;
}

namespace vke
{
	class WindowHandler;

	class ImguiImpl final
	{
	public:
		void Setup(vk::App& app, vk::SwapChain& swapChain, WindowHandler& windowHandler);
		void Cleanup(vk::App& app) const;

		static void Beginframe();
		static void EndFrame(VkCommandBuffer cmdBuffer);
	private:
		VkDescriptorPool _imguiPool{};
	};
}
