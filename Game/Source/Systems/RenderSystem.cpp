#include "pch.h"
#include "Systems/RenderSystem.h"
#include "FileLoader.h"
#include "StringView.h"
#include "VkRenderer/VkApp.h"

namespace game
{
	void RenderSystem::Allocate(const EngineOutData& engineOutData, const size_t size)
	{
		EntitySystem::Allocate(*engineOutData.allocator, size);

		LoadShader(engineOutData);
	}

	void RenderSystem::Free(jlb::LinearAllocator& allocator)
	{
		EntitySystem::Free(allocator);
	}

	void RenderSystem::Update(vk::App& app)
	{
		for (auto& task : *this)
		{
			// Do a render thing.
		}

		SetCount(0);
	}

	RenderTask RenderSystem::CreateDefaultTask(Renderer& renderer, Transform& transform)
	{
		RenderTask task{};
		return task;
	}

	void RenderSystem::LoadShader(const EngineOutData& engineOutData)
	{
		auto vert = jlb::FileLoader::Read(*engineOutData.tempAllocator, "Shaders/vert.spv");
		auto frag = jlb::FileLoader::Read(*engineOutData.tempAllocator, "Shaders/vert.spv");

		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vert.GetLength();
		vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vert.GetData());

		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = frag.GetLength();
		fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(frag.GetData());

		VkShaderModule vertModule;
		VkShaderModule fragModule;

		auto result = vkCreateShaderModule(engineOutData.app->logicalDevice, &vertCreateInfo, nullptr, &vertModule);
		assert(!result);
		result = vkCreateShaderModule(engineOutData.app->logicalDevice, &fragCreateInfo, nullptr, &fragModule);
		assert(!result);

		vkDestroyShaderModule(engineOutData.app->logicalDevice, vertModule, nullptr);
		vkDestroyShaderModule(engineOutData.app->logicalDevice, fragModule, nullptr);

		frag.Free(*engineOutData.tempAllocator);
		vert.Free(*engineOutData.tempAllocator);
	}
}
