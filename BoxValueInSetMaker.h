//
// Created by guelt on 6/12/2021.
//

#ifndef COMPILETIMEALTERNATIVES_BOXVALUEINSETMAKER_H
#define COMPILETIMEALTERNATIVES_BOXVALUEINSETMAKER_H

#include "Box.h"
#include "BoxValueInSet.h"

namespace boxes {
    // Error returned when the runtime provided value is not in the set.
    struct ValueNotInSet {};

    // Value returned when attempting to create a BoxValueInRange in range at runtime.
    template<typename T, T... set>
    using BoxedUnionValueOrError = Box<BoxValueInSet<T, set...>, ValueNotInSet>;

    namespace internal {
        template<typename T, T... set>
        struct BoxValueInSetMaker {

            template<typename V, V... possible_values>
            struct BoxedUnionValueMakerImpl;

            constexpr static BoxedUnionValueOrError<T, set...> make(T value){
                return BoxedUnionValueMakerImpl<T, set...>::make(std::move(value));
            }

            template<typename V>
            struct BoxedUnionValueMakerImpl<V> {
                constexpr static BoxedUnionValueOrError<V, set...> make(V value){
                    return ValueNotInSet {};
                }
            };

            template<typename V, V possible_value, V... possible_values>
            struct BoxedUnionValueMakerImpl<T, possible_value, possible_values...> {
                constexpr static BoxedUnionValueOrError<V, set...> make(V value){
                    if (value == possible_value){
                        return BoxValueInSet<V, set...>(
                                ValueInSet<V, possible_value, set... >::make());
                    }
                    return BoxedUnionValueMakerImpl<V, possible_value>::make(std::move(value));
                }
            };
        };
    }

    // Make a BoxValueInSet from a runtime value of type T. Takes O(|set|).
    // If value is in set, returns a BoxValueInSet<T, set...>.
    // If value is not in set, returns a ValueNotInSet.
    // TODO: Implement in O(1) using a constexpr perfect hash map.
    template<typename T, T... set>
    struct BoxedUnionValueMaker {
        constexpr static BoxedUnionValueOrError<T, set...> make(T value){
            return internal::BoxValueInSetMaker<T, set...>::make(std::move(value));
        }
    };
}

#endif //COMPILETIMEALTERNATIVES_BOXVALUEINSETMAKER_H
