#pragma once
#include "IndexSequence.h"

namespace jlb
{
	namespace tupleImpl
	{
		template <size_t I, typename T>
		struct TupleLeaf
		{
			T value;
		};

		template <size_t I, typename ...Ts>
		struct TupleImpl;

		template <size_t I>
		struct TupleImpl<I>
		{

		};

		template <size_t I, typename Head, typename ...Tail>
		struct TupleImpl<I, Head, Tail...> : TupleLeaf<I, Head>, TupleImpl<I + 1, Tail...>
		{
			
		};
	}

	/// <summary>
	/// Gets the value in the tuple at index I.
	/// </summary>
	template<size_t I, typename Head, typename ...Tail>
	[[nodiscard]] Head& Get(tupleImpl::TupleImpl<I, Head, Tail...>& tuple)
	{
		return tuple.TupleLeaf<I, Head>::value;
	}

	/// <summary>
	/// Struct that can hold multiple different types.
	/// </summary>
	/// <typeparam name="...Ts">Types to be held.</typeparam>
	template <typename ...Ts>
	using Tuple = tupleImpl::TupleImpl<0, Ts...>;
	
	template<typename Ret, typename ...Args, size_t ...I>
	auto ApplyImpl(Ret(*func)(Args&...), IndexSequence<I...>, Tuple<Args...>& tuple)
	{
		return func(Get<I>(tuple)...);
	}

	template<typename Ret, typename ...Args>
	auto Apply(Ret(*func)(Args&...), Tuple<Args...>& tuple)
	{
		return ApplyImpl(func, typename MakeIndexes<Args...>::Type(), tuple);
	}
}
