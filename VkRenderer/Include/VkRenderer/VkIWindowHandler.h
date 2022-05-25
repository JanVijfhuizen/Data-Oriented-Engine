#pragma once
#include "Array.h"
#include "StringView.h"

namespace jlb 
{
	class StackAllocator;
}

namespace vk
{
	/// <summary>
	/// Interface for communicating with the Vulkan Bootstrap class.
	/// </summary>
	class IWindowHandler
	{
	public:
		virtual ~IWindowHandler() = default;
		/// <summary>
		/// Create a vulkan surface to render to.
		/// </summary>
		/// <param name="instance">Vulkan application instance.</param>
		[[nodiscard]] virtual VkSurfaceKHR CreateSurface(VkInstance instance) = 0;
		/// <summary>
		/// Gets the required vulkan extensions needed to render to this window.
		/// </summary>
		/// <param name="allocator">Allocator used to create the array.</param>
		/// <returns>Allocated array with the required extensions.</returns>
		[[nodiscard]] virtual jlb::Array<jlb::StringView> GetRequiredExtensions(jlb::StackAllocator& allocator) = 0;
		/// <summary>
		/// Returns the required extension count.
		/// </summary>
		[[nodiscard]] virtual size_t GetRequiredExtensionsCount() = 0;
		/// <summary>
		/// Returns the window resolution.
		/// </summary>
		[[nodiscard]] virtual glm::ivec2 GetResolution() = 0;
	};
}
