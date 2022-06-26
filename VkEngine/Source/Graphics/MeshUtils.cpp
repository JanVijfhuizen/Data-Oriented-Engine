#include "VkEngine/pch.h"
#include "VkEngine/Graphics/MeshUtils.h"

namespace vke::mesh
{
	void Destroy(const EngineData& info, const Mesh& mesh)
	{
		auto& app = *info.app;
		auto& vkAllocator = *info.vkAllocator;

		vkDestroyBuffer(app.logicalDevice, mesh.indexBuffer.buffer, nullptr);
		vkDestroyBuffer(app.logicalDevice, mesh.vertexBuffer.buffer, nullptr);

		vkAllocator.FreeBlock(mesh.indexBuffer.memBlock);
		vkAllocator.FreeBlock(mesh.vertexBuffer.memBlock);
	}
}
