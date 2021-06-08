//
// Created by guelt on 6/21/2021.
//

#ifndef COMPILETIMEALTERNATIVES_BOXVALUEINRANGEMAKER_H
#define COMPILETIMEALTERNATIVES_BOXVALUEINRANGEMAKER_H

#include "BoxValueInRange.h"
#include "Box.h"

namespace boxes {
    // Error returned when a user attempts to create a runtime value which doesn't belong to the set.
    struct ValueNotInRange {};

    // Value returned when attempting to create a BoxValueInRange in range at runtime.
    template<typename T, T start, T end>
    using UnionValueInRangeOrError = Box <BoxValueInRange<T, start, end>, ValueNotInRange>;

    namespace internal {
        template <typename T, T start, T end>
        using Storage = BoxValueInRange<T, start, end> [end - start];

        template <typename T, T start, T end>
        struct BoxedUnionValueInRangeCreator {
            template <T value>
            constexpr static auto boxed_value =
                    BoxValueInRange<T, start, end>(
                            ValueInRange<T, value, start, end>::make());
        };

        template <typename T, T start, T end, T... offsets>
        struct UnionValueInRangeMakerImpl {
            // Returns a Box which is either:
            // - a BoxValueInRange if the value is in range.
            // - a ValueNotInRange if the value is not in range.
            constexpr static UnionValueInRangeOrError<T, start, end> make(T value) {
                if (value >= start && value <= end) {
                    return storage_[value - start];
                }
                return ValueNotInRange{};
            }

            constexpr static Storage<T, start, end> storage_ =
                    {
                            BoxedUnionValueInRangeCreator<T, start, end>
                            ::template boxed_value<start + offsets>...
                    };
        };

        template <typename T, T start, T end, typename IntegerSequence>
        struct UnionValueInRangeMakerAdapter;

        template <typename T, T start, T end, T... range>
        struct UnionValueInRangeMakerAdapter<T, start, end, std::integer_sequence<T, range...>> {
            using MakerImpl = UnionValueInRangeMakerImpl<T, start, end, range...>;
        };
    }

    // Used to created a BoxValueInRange from a runtime value in O(1);
    template <typename T, T start, T end>
    // If value is in set, returns a BoxedValeInRange<T, set...>.
    // If value is not in set, returns a ValueNotInRange.
    using BoxValueInRangeMaker = typename internal::UnionValueInRangeMakerAdapter
                <T, start, end, std::make_integer_sequence<T, end - start>>
                ::MakerImpl;
}

#endif //COMPILETIMEALTERNATIVES_BOXVALUEINRANGEMAKER_H

