#pragma once

namespace jlb
{
	template <size_t I, typename T>
	struct TupleLeaf
	{
		T value{};
	};

	template <size_t I, typename ...Args>
	struct TupleImpl;

	template <size_t I, typename Head, typename ...Tail>
	struct TupleImpl<I, Head, Tail...> : TupleLeaf<0, Head>, TupleImpl<I + 1, Tail...>
	{
		
	};

	template<typename ...Args>
	using Tuple = TupleImpl<0, Args...>;

	template <size_t I, typename Head, typename ...Tail>
	[[nodiscard]] auto& Get(const TupleImpl<I, Head, Tail...>& tuple)
	{
		return tuple.template TupleLeaf<I, Head>::value;
	}

	template <typename Head, typename ...Tail>
	[[nodiscard]] constexpr size_t Size(const Tuple<Head, Tail...>& tuple)
	{
		return sizeof...(Tail) + 1;
	}
}