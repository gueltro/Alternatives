//
// Created by guelt on 6/11/2021.
//

#ifndef COMPILETIMEALTERNATIVES_BELONGSTO_H
#define COMPILETIMEALTERNATIVES_BELONGSTO_H
namespace boxes {
    // Check whether T is equal to any of the types in Range.
    template<typename T, typename... Range>
    struct BelongsTo;

    template<typename T>
    struct BelongsTo<T> {
        constexpr static bool in_range = false;
    };

    template<typename T, typename TInRange, typename... Range>
    struct BelongsTo<T, TInRange, Range...> {
        constexpr static bool in_range =
                std::is_same<T, TInRange>::value ||
                BelongsTo<T, Range...>::in_range;
    };
}
#endif //COMPILETIMEALTERNATIVES_BELONGSTO_H
