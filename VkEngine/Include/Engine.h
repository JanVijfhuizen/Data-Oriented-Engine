#pragma once
#include <vector>

namespace jlb
{
	class LinearAllocator;
}

namespace vke
{
	constexpr VkDeviceSize MEM_DEFAULT_SIZE = 65536;

	class Engine final
	{
	public:
		static int Run();

	private:
		struct VersionData final
		{
			struct PoolInfo final
			{
				VkDeviceSize size = MEM_DEFAULT_SIZE;
				VkDeviceSize alignment = 256;
			};

			size_t buildVersion = SIZE_MAX;
			size_t allocSpace = MEM_DEFAULT_SIZE;
			size_t tempAllocSpace = MEM_DEFAULT_SIZE;
			std::vector<PoolInfo> poolInfos{};
		};

		static int RunGame(VersionData& versionData, bool allocRun);

		static bool LoadVersionData(VersionData& outVersionData);
		static void SaveVersionData(VersionData& versionData);
	};
}