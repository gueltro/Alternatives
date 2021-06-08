//
// Created by guelt on 6/19/2021.
//

#ifndef COMPILETIMEALTERNATIVES_VALUEINRANGE_H
#define COMPILETIMEALTERNATIVES_VALUEINRANGE_H

namespace boxes {
    // A value of type T which satisfies start <= value < end.
    template <typename T, T v, T start, T end>
    struct ValueInRange {
        template <typename std::enable_if_t<
                        v >= start &&
                        v < end
                >* = nullptr>
        constexpr static ValueInRange<T, v, start, end> make() {
            return ValueInRange();
        }

    private:
        constexpr ValueInRange() {}
    };
}

#endif //COMPILETIMEALTERNATIVES_VALUEINRANGE_H
