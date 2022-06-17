#include "pch.h"
#include "Graphics/MeshUtils.h"

namespace game::mesh
{
	void Destroy(const EngineData& EngineData, Mesh& mesh)
	{
		auto& app = *EngineData.app;
		auto& vkAllocator = *EngineData.vkAllocator;

		vkDestroyBuffer(app.logicalDevice, mesh.indexBuffer.buffer, nullptr);
		vkDestroyBuffer(app.logicalDevice, mesh.vertexBuffer.buffer, nullptr);

		vkAllocator.FreeBlock(mesh.indexBuffer.memBlock);
		vkAllocator.FreeBlock(mesh.vertexBuffer.memBlock);
	}
}
