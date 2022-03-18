#pragma once
#include "Array.h"
#include "StringView.h"
#include "VkApp.h"

namespace jlb 
{
	class LinearAllocator;
}

namespace vk
{
	struct AppInfo final
	{
		jlb::StringView name;
		jlb::Array<jlb::StringView> validationLayers{};
		jlb::Array<jlb::StringView> deviceExtensions{};
	};

	class Bootstrap final
	{
	public:
		[[nodiscard]] static App CreateApp(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void DestroyApp(const App& app);

	private:
		static void CheckValidationSupport(jlb::LinearAllocator& tempAllocator, AppInfo& info);
		static void CreateInstance(jlb::LinearAllocator& tempAllocator, AppInfo& info);

		[[nodiscard]] static VkApplicationInfo CreateApplicationInfo(AppInfo& info);
		[[nodiscard]] static jlb::Array<jlb::StringView> GetExtensions(jlb::LinearAllocator& tempAllocator, AppInfo& info);
	};
}