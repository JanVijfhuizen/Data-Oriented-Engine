#pragma once
#include "VkIWindowHandler.h"

namespace jlb 
{
	class StackAllocator;
}

namespace vk
{
	struct App;

	/// <summary>
	/// Information from which the Vulkan Bootstrap class can create a vulkan application.
	/// </summary>
	struct AppInfo final
	{
		/// <summary>
		/// Contains GPU hardware information.
		/// </summary>
		struct PhysicalDeviceInfo final
		{
			VkPhysicalDevice device;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
		};

		// Name of the application.
		jlb::StringView name;
		// Layers used to debug vulkan. Only enabled during debugging.
		jlb::Array<jlb::StringView> validationLayers{};
		// Required hardware extensions.
		jlb::Array<jlb::StringView> deviceExtensions{};
		// Class that manages the window.
		IWindowHandler* windowHandler = nullptr;
		// Returns whether or not the physical device can be used by the program.
		bool(*isPhysicalDeviceValid)(PhysicalDeviceInfo& info) = nullptr;
		// Returns how qualified the physical device is.
		size_t(*getPhysicalDeviceRating)(PhysicalDeviceInfo& info) = nullptr;
		// Returns the features needed to be enabled for the program to work.
		VkPhysicalDeviceFeatures(*getPhysicalDeviceFeatures)() = nullptr;

		void Free(jlb::StackAllocator& tempAllocator);
	};

	namespace boots
	{
		/// <summary>
		/// Contains swap chain support details for a Vulkan physical device.
		/// </summary>
		struct SwapChainSupportDetails final
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			jlb::Array<VkSurfaceFormatKHR> formats{};
			jlb::Array<VkPresentModeKHR> presentModes{};

			[[nodiscard]] explicit operator bool() const;
			[[nodiscard]] size_t GetRecommendedImageCount() const;
			void Free(jlb::StackAllocator& tempAllocator);
		};

		/// <summary>
		/// Contains the standard render queue families.
		/// </summary>
		struct QueueFamilies final
		{
			union
			{
				struct
				{
					size_t graphics;
					size_t present;
					size_t transfer;
				};

				size_t values[3]
				{
					SIZE_MAX,
					SIZE_MAX,
					SIZE_MAX
				};
			};

			[[nodiscard]] operator bool() const;
		};

		/// <summary>
		/// Create a default info struct that will be good enough for most Vulkan applications.<br>
		/// For more advanced use, it is adviced to create one yourself.
		/// </summary>
		/// <returns>Info struct from which a Vulkan application can be created.</returns>
		[[nodiscard]] AppInfo CreateDefaultInfo(jlb::StackAllocator& tempAllocator);
		/// <summary>
		/// Creates a new Vulkan application.
		/// </summary>
		/// <param name="info">Info struct from which to create the application.</param>
		/// <returns>The created Vulkan application.</returns>
		[[nodiscard]] App CreateApp(jlb::StackAllocator& tempAllocator, AppInfo info);

		/// <summary>
		/// Destroys a Vulkan application.
		/// </summary>
		/// <param name="app">Application to be destroyed.</param>
		void DestroyApp(App& app);

		/// <summary>
		/// Returns a struct containing information about the GPU hardware's swap chain support.
		/// </summary>
		/// <param name="app">Vulkan Application.</param>
		[[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(jlb::StackAllocator& allocator, App& app);
		/// <summary>
		/// Returns the GPU's render queue families.
		/// </summary>
		[[nodiscard]] QueueFamilies GetQueueFamilies(jlb::StackAllocator& tempAllocator, App& app);
	}
}
