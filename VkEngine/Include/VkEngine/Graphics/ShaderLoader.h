#pragma once
#include <StringView.h>

namespace vke
{
	struct Shader;

	namespace shader 
	{
		[[nodiscard]] Shader Load(const EngineData& info, jlb::StringView vertPath, jlb::StringView fragPath);
		void Unload(const EngineData& info, const Shader& shader);
	}
}