//
// Created by guelt on 6/22/2021.
//

#ifndef COMPILETIMEALTERNATIVES_FLATBOX_H
#define COMPILETIMEALTERNATIVES_FLATBOX_H

#include "Box.h"

namespace boxes{
    namespace internal {
        template<typename T>
        struct IsBoxedChecker;

        template<typename T>
        struct IsBoxedChecker {
            constexpr static bool value = false;
        };

        template<typename... Alternatives>
        struct IsBoxedChecker<Box < Alternatives...>> {
            constexpr static bool value = true;
        };

        // Checks whether T is an instance of Box<Alternatives...>.
        template<typename T>
        constexpr static bool is_boxed = IsBoxedChecker<T>::value;

        template<typename... Ts>
        struct IsFlat;

        template<>
        struct IsFlat<> {
            constexpr static bool is_flat = true;
        };

        template<typename T, typename... Ts>
        struct IsFlat<T, Ts...> {
        constexpr static bool is_flat =
                !is_boxed<T> && IsFlat<Ts...>::is_flat;
        };

        // Checks whether Ts... is exclusively composed of non-boxed types.
        template<typename... Ts>
        constexpr static bool is_flat = IsFlat<Ts...>::is_flat;

        // Represents a pack of alternatives which may or may not contain instances of Box.
        template <typename... Alternatives>
        struct MaybeFlatBoxedUnion{
            constexpr static bool is_flat_boxed_union = is_flat<Alternatives...>;
        };

        // Partially constructed Boxed union.
        template <typename... Alternatives>
        struct PartialFlatBoxedUnion {
            template<typename Alternative>
            struct Extender;

            template<typename Alternative>
            struct Extender {
                using extend = PartialFlatBoxedUnion<Alternatives..., Alternative>;
            };

            template<typename... MultipleAlternatives>
            struct Extender<Box<MultipleAlternatives...>> {
                using extend = PartialFlatBoxedUnion<
                        Alternatives...,
                        MultipleAlternatives...
                        >;
            };
        };

        // Performs each round of flattening of the Alternatives.
        template <typename PartialFlatBoxUnion, typename... Alternatives>
        struct Flattener;

        template <typename... ProcessedAlternatives>
        struct Flattener<PartialFlatBoxedUnion<ProcessedAlternatives...>> {
            using flatten = MaybeFlatBoxedUnion<ProcessedAlternatives...>;
        };

        template <typename... ProcessedAlternatives, typename Alternative, typename... Alternatives>
        struct Flattener<
                PartialFlatBoxedUnion<ProcessedAlternatives...>,
                Alternative,
                Alternatives...> {
            using flatten =  typename Flattener<
                    typename PartialFlatBoxedUnion<ProcessedAlternatives...>
                            ::template Extender<Alternative>::extend,
                    Alternatives...
                    >::flatten;
        };

        template <bool is_flat, typename... MaybeFlatBoxedUnion>
        struct RecursiveFlattener;

        template <typename... Alternatives>
        struct RecursiveFlattener <true, MaybeFlatBoxedUnion<Alternatives...>> {
            using boxed_union = Box<Alternatives...>;
        };

        template <typename... Alternatives>
        struct RecursiveFlattener <false, MaybeFlatBoxedUnion<Alternatives...>> {
            using maybe_flat_boxed_union = typename Flattener<
                        PartialFlatBoxedUnion<>, Alternatives...>::flatten;

            using boxed_union = typename RecursiveFlattener<
                    maybe_flat_boxed_union::is_flat_boxed_union,
                    maybe_flat_boxed_union>
                    ::boxed_union;
        };

        template <typename... Alternatives>
        using flatten_recursively = typename RecursiveFlattener<
                MaybeFlatBoxedUnion<Alternatives...>::is_flat_boxed_union,
                MaybeFlatBoxedUnion<Alternatives...>
                >::boxed_union;
    }

    // Version of Box where inner BoxedUnions have been recursively unwrapped.
    template <typename... Alternatives>
    using FlatBox = internal::flatten_recursively<Alternatives...>;
}

#endif //COMPILETIMEALTERNATIVES_FLATBOX_H
