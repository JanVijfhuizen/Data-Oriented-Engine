#pragma once

namespace jlb
{
    template<int...> struct IndexSequence
    {
	    
    };

    namespace indexImpl
    {
        template<int I, typename IndexTuple, typename...Types>
        struct MakeIndexesImpl;

        template<int I, int... Indexes, typename T, typename ...Types>
        struct MakeIndexesImpl<I, IndexSequence<Indexes...>, T, Types...>
        {
            typedef typename MakeIndexesImpl<I + 1, IndexSequence<Indexes..., I>, Types...>::Type Type;
        };

        template<int I, int...Indexes>
        struct MakeIndexesImpl<I, IndexSequence<Indexes...>>
        {
            typedef IndexSequence<Indexes...> Type;
        };
    }

    /// <summary>
    /// Creates a linear sequence of numbers, based on the amount of arguments passed.<br>
    /// If N arguments are passed, it will generate a IndexSequence from 0 to N - 1.
    /// </summary>
    template<typename ...Types>
    struct MakeIndexes : indexImpl::MakeIndexesImpl<0, IndexSequence<>, Types...>
    {
	    
    };
}