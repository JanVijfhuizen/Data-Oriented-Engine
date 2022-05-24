#include "pch.h"
#include "Graphics/MeshHandler.h"

namespace game
{
	void MeshHandler::Destroy(const EngineOutData& outData, Mesh& mesh)
	{
		auto& app = *outData.app;
		auto& vkAllocator = *outData.vkAllocator;

		vkDestroyBuffer(app.logicalDevice, mesh.indexBuffer.buffer, nullptr);
		vkDestroyBuffer(app.logicalDevice, mesh.vertexBuffer.buffer, nullptr);

		vkAllocator.FreeBlock(mesh.indexBuffer.memBlock);
		vkAllocator.FreeBlock(mesh.vertexBuffer.memBlock);
	}
}
