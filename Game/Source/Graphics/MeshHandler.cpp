#include "pch.h"
#include "Graphics/MeshHandler.h"

namespace game
{
	void MeshHandler::Destroy(const EngineOutData& engineOutData, Mesh& mesh)
	{
		auto& app = *engineOutData.app;
		auto& vkAllocator = *engineOutData.vkAllocator;

		vkDestroyBuffer(app.logicalDevice, mesh.indexBuffer, nullptr);
		vkDestroyBuffer(app.logicalDevice, mesh.vertexBuffer, nullptr);

		vkAllocator.FreeBlock(mesh.indexMemBlock);
		vkAllocator.FreeBlock(mesh.vertexMemBlock);
	}
}
