//
// Created by guelt on 6/10/2021.
//

#ifndef COMPILETIMEALTERNATIVES_BOX_H
#define COMPILETIMEALTERNATIVES_BOX_H

#include <variant>
#include "internal/OverloadStateful.h"
#include "internal/BelongsTo.h"

namespace boxes {
    // A value whose type is one of `Alternatives`.
    template<typename... Alternatives>
    struct Box {
    public:
        // Create a Box from a runtime value whose type is an element of `Alternatives`.
        template <typename T,
                typename std::enable_if_t<
                        BelongsTo<T,Alternatives...>::in_range
                >* = nullptr>
        constexpr Box(T x) : value_(std::move(x)) {}

        // Unbox a variadic packs of functors against the wrapped value.
        template<typename... FS>
        constexpr auto Unbox(FS&&... fs) && {
            return std::visit(
                    internal::overloaded_stateful<FS...>(std::forward<FS>(fs)...),
                    std::move(value_));
        }

        // Expose a reference to the boxed value to a  variadic packs of functors.
        template<typename... FS>
        constexpr auto Peek(FS&&... fs) const {
            return std::visit(
                    internal::overloaded_stateful<FS...>(std::forward<FS>(fs)...),
                    value_);
        }

    private:
        std::variant<Alternatives...> value_;
    };
}

#endif //COMPILETIMEALTERNATIVES_BOX_H
