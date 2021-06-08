//
// Created by guelt on 6/13/2021.
//

#ifndef COMPILETIMEALTERNATIVES_OVERLOADSTATEFUL_H
#define COMPILETIMEALTERNATIVES_OVERLOADSTATEFUL_H

namespace boxes {
    namespace internal {
        // Merge several Functors into a single functor which supports all of their operators.
        template<class... Fs>
        struct overloaded_stateful : Fs ... {
            explicit overloaded_stateful(Fs &&... ts) : Fs(std::forward<Fs>(ts))... {}

            using Fs::operator()...;
        };
    }
}
#endif //COMPILETIMEALTERNATIVES_OVERLOADSTATEFUL_H
