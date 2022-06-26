#include "VkEngine/pch.h"
#include "VkEngine/Graphics/ShaderLoader.h"
#include "VkEngine/Graphics/Shader.h"
#include "FileLoader.h"
#include "VkRenderer/VkApp.h"

namespace vke::shader
{
	Shader Load(const EngineData& info, const jlb::StringView vertPath, const jlb::StringView fragPath)
	{
		auto& logicalDevice = info.app->logicalDevice;
		auto& tempAllocator = *info.tempAllocator;

		auto vert = jlb::file::Read(tempAllocator, vertPath);
		auto frag = jlb::file::Read(tempAllocator, fragPath);

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vert.GetLength();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vert.GetData());

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = frag.GetLength();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.GetData());

		Shader shader{};

		auto result = vkCreateShaderModule(logicalDevice, &vertCreateInfo, nullptr, &shader.vert);
		assert(!result);
		result = vkCreateShaderModule(logicalDevice, &fragCreateInfo, nullptr, &shader.frag);
		assert(!result);

		frag.Free(tempAllocator);
		vert.Free(tempAllocator);

		return shader;
	}
	void Unload(const EngineData& info, const Shader& shader)
	{
		vkDestroyShaderModule(info.app->logicalDevice, shader.vert, nullptr);
		vkDestroyShaderModule(info.app->logicalDevice, shader.frag, nullptr);
	}
}
