//
// Created by guelt on 6/8/2021.
//

#ifndef COMPILETIMEALTERNATIVES_UNION_H
#define COMPILETIMEALTERNATIVES_UNION_H

#include "internal/BelongsTo.h"
#include "internal/OverloadStateful.h"

namespace boxes {
    namespace internal {
        template<bool is_defined, typename T, typename... Alts>
        struct UnionStructHelper;

        template<typename T, typename A, typename B>
        struct UnionStructHelper<true, T, A, B> {
            using type = T;
        };

        template <typename T, typename... Alts>
        using UnionHelper = UnionStructHelper<BelongsTo<T, Alts...>::in_range, T, Alts...>;
    }

    // A pathetic attempt to create a zero-runtime-overhead union-like type.
    // The actual type needs to be specified at compile time, and it can be used to restraint
    // the types.
    // It is not really clear to me yet whether this is useful at all, while it is clearly painful to explicitly
    // specify the type T on each declaration of Union.
    // This may become useful if I ever manage to create something that allows a Box to be matched against
    // a functor whose operator accepts a union as an argument.
    template<typename T, typename... Alts>
    using Union = typename internal::UnionHelper<T, Alts...>::type;

    // Used to match a Union whose explicit type is T.
    template<typename T, typename... Fs>
    constexpr static auto Match(T x, Fs&&... fs) {
        return internal::overloaded_stateful<Fs...>
                (std::forward<Fs>(fs)...)
                (std::move(x));
    }
}

#endif //COMPILETIMEALTERNATIVES_UNION_H
