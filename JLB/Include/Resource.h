#pragma once
#include "StringView.h"

namespace game
{
	struct Resource final
	{
		jlb::StringView name = nullptr;
		void* ptr = nullptr;

		template <typename T>
		[[nodiscard]] T* Get();
	};

	template <typename T>
	T* Resource::Get()
	{
		return reinterpret_cast<T*>(ptr);
	}
}
