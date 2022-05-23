#include "pch.h"
#include "Graphics/MeshHandler.h"

namespace game
{
	void MeshHandler::Destroy(const EngineOutData& outData, Mesh& mesh)
	{
		auto& app = *outData.app;
		auto& vkAllocator = *outData.vkAllocator;

		vkDestroyBuffer(app.logicalDevice, mesh.indexBuffer, nullptr);
		vkDestroyBuffer(app.logicalDevice, mesh.vertexBuffer, nullptr);

		vkAllocator.FreeBlock(mesh.indexMemBlock);
		vkAllocator.FreeBlock(mesh.vertexMemBlock);
	}
}
