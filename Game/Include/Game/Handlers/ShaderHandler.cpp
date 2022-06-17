﻿#include "pch.h"
#include "ShaderHandler.h"
#include "StringView.h"
#include "VkRenderer/VkApp.h"
#include "FileLoader.h"
#include "Graphics/Shader.h"

namespace game 
{
	Shader ShaderHandler::Create(const EngineData& EngineData,
		const jlb::StringView vertPath, const jlb::StringView fragPath)
	{
		auto& logicalDevice = EngineData.app->logicalDevice;
		auto& tempAllocator = *EngineData.tempAllocator;

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

	void ShaderHandler::Destroy(const EngineData& EngineData, const Shader& shader)
	{
		vkDestroyShaderModule(EngineData.app->logicalDevice, shader.vert, nullptr);
		vkDestroyShaderModule(EngineData.app->logicalDevice, shader.frag, nullptr);
	}
}
