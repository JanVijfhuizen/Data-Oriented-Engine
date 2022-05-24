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
		[[nodiscard]] static Shader Create(const EngineOutData& outData, jlb::StringView vertPath, jlb::StringView fragPath);
		static void Destroy(const EngineOutData& outData, const Shader& shader);
	};
}
