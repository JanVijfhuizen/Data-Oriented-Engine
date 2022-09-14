#pragma once

namespace jlb
{
	struct Node final
	{
		void* data = nullptr;
		Node* next = nullptr;
	};
}