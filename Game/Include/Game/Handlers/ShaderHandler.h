#pragma once

namespace jlb 
{
	class StringView;
}

namespace game
{
	struct Shader;

	class ShaderHandler final
	{
	public:
		[[nodiscard]] static Shader Create(const EngineData& EngineData, jlb::StringView vertPath, jlb::StringView fragPath);
		static void Destroy(const EngineData& EngineData, const Shader& shader);
	};
}
