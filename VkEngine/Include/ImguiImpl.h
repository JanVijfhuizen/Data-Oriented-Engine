#pragma once

namespace vk 
{
	class SwapChain;
	struct App;
}

namespace vke
{
	class WindowHandler;

	/// <summary>
	/// Abstraction for setting up IMGUI. Not performant AT ALL, highly advised to only use it while debugging.
	/// </summary>
	class ImguiImpl final
	{
	public:
		void Setup(vk::App& app, vk::SwapChain& swapChain, WindowHandler& windowHandler);
		void Cleanup(vk::App& app) const;

		// Call this after render pass has begun.
		static void Beginframe();
		// Call this before the render pass ends.
		static void EndFrame(VkCommandBuffer cmdBuffer);
	private:
		VkDescriptorPool _imguiPool{};
	};
}
