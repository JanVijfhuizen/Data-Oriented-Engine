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

	namespace tupleImpl
	{
		template<typename Ret, typename In, typename ...Args, size_t ...I>
		auto ApplyImpl(Ret(*func)(In&, Args&...), In& in, IndexSequence<I...>, Tuple<Args...>& tuple)
		{
			return func(in, Get<I>(tuple)...);
		}
	}

	/// <summary>
	/// Calls the given function and forwards the tuple values to that function.
	/// </summary>
	/// <param name="func">Function to pass.</param>
	/// <param name="in">Additional parameter value.</param>
	/// <returns>Func output.</returns>
	template<typename Ret, typename In, typename ...Args>
	auto Apply(Ret(*func)(In&, Args&...), Tuple<Args...>& tuple, In& in)
	{
		return tupleImpl::ApplyImpl(func, in, typename MakeIndexes<Args...>::Type(), tuple);
	}
}
