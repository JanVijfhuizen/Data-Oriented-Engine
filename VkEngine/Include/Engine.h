#pragma once

namespace jlb
{
	class LinearAllocator;
}

namespace vke
{
	class Engine final
	{
	public:
		static int Run();

	private:
		struct RuntimeData final
		{
			jlb::LinearAllocator* allocator;
			jlb::LinearAllocator* tempAllocator;
		};

		struct VersionData final
		{
			size_t buildVersion = SIZE_MAX;
			size_t allocSpace = 65536;
			size_t tempAllocSpace = 65536;
		};

		static int RunGame(RuntimeData& data, bool allocRun);

		static bool LoadVersionData(VersionData& outVersionData);
		static void SaveVersionData(RuntimeData& runtimeData);
	};
}