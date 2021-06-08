//
// Created by guelt on 6/11/2021.
//

#ifndef COMPILETIMEALTERNATIVES_BOXVALUEINSET_H
#define COMPILETIMEALTERNATIVES_BOXVALUEINSET_H

#include "ValueInSet.h"
#include "Box.h"

#include <variant>

namespace boxes {
    namespace internal {
        template <typename T, T... set>
        struct BoxedUnionValueCreator {
            template <T value>
            using UnionValueMaker = ValueInSet<T, value, set...>;

            using BoxedUnionValueType = Box<UnionValueMaker<set>...>;
        };
    }

    // A value of type T which belongs to a set.
    // Wraps a ValueInSet<T, value, start, set> where `value` have been type-erased.
    template <typename T, T... set>
    using BoxValueInSet = typename internal::BoxedUnionValueCreator<T, set...>::BoxedUnionValueType;
}
#endif //COMPILETIMEALTERNATIVES_BOXVALUEINSET_H
