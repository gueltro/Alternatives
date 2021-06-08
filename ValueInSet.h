//
// Created by guelt on 6/11/2021.
//

#ifndef COMPILETIMEALTERNATIVES_VALUEINSET_H
#define COMPILETIMEALTERNATIVES_VALUEINSET_H

namespace boxes {
    namespace internal{
        template<typename T, T value, T... set>
        struct IsValueInSet;

        template<typename T, T value>
        struct IsValueInSet<T, value> {
            constexpr static bool belongs_to = false;
        };

        template<typename T, T value, T member, T... set>
        struct IsValueInSet<T, value, member, set...> {
            constexpr static bool belongs_to =
                    (value == member) ||
                    IsValueInSet<T, value, set...>::belongs_to;
        };

        template <typename T, T value, T... set>
        constexpr static bool is_value_in_set = IsValueInSet<T, value, set...>::belongs_to;
    }

    // A value of type T which belongs to a set.
    template<typename T, T value, T... set>
    struct ValueInSet {
        template <typename std::enable_if_t<
                internal::is_value_in_set<T, value, set...>
        >* = nullptr>
        constexpr static ValueInSet<T, value, set...> make() {
            return ValueInSet();
        }

    private:
        constexpr ValueInSet() {}
    };
}

#endif //COMPILETIMEALTERNATIVES_VALUEINSET_H
