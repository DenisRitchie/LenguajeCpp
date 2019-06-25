#pragma once
#ifndef _INDEX_SEQUENCE_HPP_
#define _INDEX_SEQUENCE_HPP_

#include <type_traits>

namespace cpp_sequences
{
    template<size_t...>
    struct index_sequence
    {
    };

    template<typename Sequence1, typename Sequence2>
    struct concatenate;

    template<size_t ...Index1, size_t ...Index2>
    struct concatenate<index_sequence<Index1...>, index_sequence<Index2...>>
    {
        using type = index_sequence<Index1..., Index2...>;
    };

    template<size_t First, size_t Length>
    struct make_range_impl;

    template<size_t First, size_t Length>
    using make_range = typename make_range_impl<First, Length>::type;

    template<size_t First, size_t Length>
    struct make_range_impl
    {
        using type = typename concatenate<
            make_range<First, Length / 2>,
            make_range<First + Length / 2, Length - Length / 2>
        >::type;
    };

    template<size_t First>
    struct make_range_impl<First, 0>
    {
        using type = index_sequence<>;
    };

    template<size_t First>
    struct make_range_impl<First, 1>
    {
        using type = index_sequence<First>;
    };

    template<size_t Length>
    using make_index_sequence = make_range<0, Length>;

    template<typename Ctor, size_t ...Index>
    std::array<std::invoke_result_t<Ctor, size_t>, sizeof...(Index)>
    make_array_impl(Ctor&& ctor, index_sequence<Index...>)
    {
        return std::array<std::invoke_result_t<Ctor, size_t>, sizeof...(Index)>{ ctor(Index)... };
    }

    template<size_t N, typename Ctor>
    std::array<std::invoke_result_t<Ctor, size_t>, N> make_array(Ctor&& ctor)
    {
        return make_array_impl(std::forward<Ctor>(ctor), make_index_sequence<N>());
    }

    namespace test
    {
        class dummy_type
        {
        public:
            dummy_type(int value) : m_value{ value }
            {
            }

            friend std::ostream& operator<<(std::ostream& os, const dummy_type& v)
            {
                return os << v.m_value;
            }

        private:
            int m_value;
        };

        template<class T, size_t Size, size_t ...Ix>
        void print_natives_arrays_impl(T(&ar)[Size], index_sequence<Ix...>)
        {
            ((std::cout << ar[Ix] << ", "), ...);
            std::cout.put('\n');
        }

        template<class ...T, size_t ...Size>
        void print_natives_arrays(T(&...ar)[Size])
        {
            (print_natives_arrays_impl(ar, make_index_sequence<Size>()), ...);
        }

        void run()
        {
            auto arr = make_array<12>([](size_t index) { return dummy_type{ static_cast<int>(index) * 2 }; });
            auto arr2 = make_array<100>([](size_t index) { return index; });
            print_natives_arrays("Print", "Elems", arr._Elems, arr2._Elems);
        }
    }
}

#endif //!_INDEX_SEQUENCE_HPP_