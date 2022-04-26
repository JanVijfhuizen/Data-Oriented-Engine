﻿#pragma once
#include <vector>

namespace jlb
{
	class LinearAllocator;
}

namespace vke
{
	/// <summary>
	/// A data-oriented engine that focuses on optimizing memory performance.
	/// </summary>
	class Engine final
	{
	public:
		/// <summary>
		/// Runs the game via the engine.
		/// </summary>
		/// <returns>0 if successful, otherwise returns a Vulkan error.</returns>
		static int Run();

	private:
		/// <summary>
		/// Engine version data. Used to define the memory requirements for this build.
		/// </summary>
		struct VersionData final
		{
			/// <summary>
			/// Memory requirements for a Vulkan pool.
			/// </summary>
			struct VkPoolInfo final
			{
				VkDeviceSize size = MEM_DEFAULT_SIZE;
				VkDeviceSize alignment = 256;
			};

			size_t buildVersion = SIZE_MAX;
			// Space required during initialization.
			// Only used for temporary data that can be deleted after initialization.
			size_t setupAllocSpace = MEM_DEFAULT_SIZE;
			// Space required during initialization/runtime.
			size_t allocSpace = MEM_DEFAULT_SIZE;
			// Space required during runtime for temporary allocations.
			size_t tempAllocSpace = MEM_DEFAULT_SIZE;
			// Memory requirements per Vulkan pool. Amount of pools and size often varies for different hardware.
			std::vector<VkPoolInfo> poolInfos{};
		};

		/// <summary>
		/// These phases will calculate the specific memory requirements.<br>
		/// Every phase except for the Main phase will stop after a single frame.
		/// </summary>
		enum class Phase
		{
			// Checks the alignment for every Vulkan pool available.
			VkPoolAlignmentCheck,
			// Checks the general memory usage, and the Vulkan pool memory usage (now with proper alignment).
			GeneralMemoryCheck,
			// Main loop.
			Main
		};

		static int RunGame(VersionData& versionData, Phase phase);

		/// <returns>If the version data is found and valid.</returns>
		static bool LoadVersionData(VersionData& outVersionData);
		static void SaveVersionData(VersionData& versionData);
	};
}