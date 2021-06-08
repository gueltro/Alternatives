//
// Created by guelt on 6/19/2021.
//

#ifndef COMPILETIMEALTERNATIVES_BOXVALUEINRANGE_H
#define COMPILETIMEALTERNATIVES_BOXVALUEINRANGE_H
#include "ValueInRange.h"
#include "Box.h"

namespace boxes {
    namespace internal {
        template<typename T, T length>
        struct IndexProvider {
            using indexes = std::make_integer_sequence<T, length>;
        };

        template<typename T, T start, T end, typename Indexes>
        struct BoxValueInRangeCreator;

        template<typename T, T start, T end, T... indexes>
        struct BoxValueInRangeCreator<T, start, end, std::integer_sequence<T, indexes...>> {
            template <T v>
            using UnionValueInRangeMaker = ValueInRange<T, v, start, end>;
            using union_value = Box<UnionValueInRangeMaker<(start + indexes)>...>;
        };
    }

    // A value of type T which is bigger or equal than start and smaller than end.
    // Wraps a ValueInRange<T, value, start, set> where `value` have been type-erased.
    template <typename T, T start, T end>
    using BoxValueInRange  = typename internal::BoxValueInRangeCreator<
            T,
            start,
            end,
            typename internal::IndexProvider<T, (end - start)>::indexes>::union_value;
}

#endif //COMPILETIMEALTERNATIVES_BOXVALUEINRANGE_H
