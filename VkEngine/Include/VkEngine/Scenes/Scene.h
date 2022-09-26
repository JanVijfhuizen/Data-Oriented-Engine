#pragma once

namespace vke
{
	class Scene
	{
	public:
		void Allocate();
		void Free();

	private:
		jlb::StackAllocator _allocator{};
	};
}
