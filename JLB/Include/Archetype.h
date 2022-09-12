#pragma once
#include "NestedVector.h"
#include "Tuple.h"

namespace jlb
{
	template <typename ...Components>
	class Archetype
	{
	public:
		template <typename T>
		[[nodiscard]] constexpr bool HasComponent() const;

	private:
		NestedVector<Tuple<Components...>> _vector{};

		template <typename T, typename Head, typename Second, typename ...Tail>
		[[nodiscard]] static constexpr bool ContainsType();
		template <typename T, typename Head>
		[[nodiscard]] static constexpr bool ContainsType();
	};

	template <typename ... Components>
	template <typename T>
	constexpr bool Archetype<Components...>::HasComponent() const
	{
		return ContainsType<T, Components...>();
	}

	template <typename ... Components>
	template <typename T, typename Head, typename Second, typename ...Tail>
	constexpr bool Archetype<Components...>::ContainsType()
	{
		return ContainsType<T, Head>() || ContainsType<T, Second, Tail...>();
	}

	template <typename ... Components>
	template <typename T, typename Head>
	constexpr bool Archetype<Components...>::ContainsType()
	{
		return typeid(T) == typeid(Head);
	}
}
