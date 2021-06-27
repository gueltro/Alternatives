//
// Created by guelt on 6/8/2021.
//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../Union.h"
#include "../ValueInSet.h"
#include "../BoxValueInSet.h"
#include "../Box.h"
#include "../ValueInRange.h"
#include "../BoxValueInRange.h"
#include "../BoxValueInRangeMaker.h"
#include "../BoxValueInSetMaker.h"
#include "../FlatBox.h"

#include <iostream>


namespace boxes {

    template<typename T>
    void Print(Union<T, int, std::string> either) {
        Match(
                either,
                [](int a) { std::cout << "Printing int: " << a << std::endl; },
                [](std::string b) { std::cout << "Printing string: " << b << std::endl; });
    }

    TEST_CASE("DoesCompile") {
        Print<int>(1);
        std::string s = "TEST_STRING";
        Print<std::string>(s);
    }

    struct A {};
    struct B {};

    TEST_CASE("Unbox") {
        int x = Match(
                        A{},
                        [](A a){
                            return 0;
                        },
                        [](B b){
                            return 1;
                        }
                        );

        int y = Match(
                B{},
                [](A a){
                    return 0;
                },
                [](B b){
                    return 1;
                }
        );

        CHECK(x == 0);
        CHECK(y == 1);
    }

    TEST_CASE("MatchCaptureLambda") {

        int k = 1;
        std::unique_ptr<int> k_ptr = std::make_unique<int>(k);
        int x = Match(
                A{},
                [k_p = std::move(k_ptr)](A a) mutable {
                    return *k_p;
                },
                [](B b) mutable {
                    return 0;
                }
        );

        CHECK(x == k);
    }

    TEST_CASE("BoxedUnionCompiles") {
        std::vector<Box<int, std::string>> v;
        v.emplace_back(1);
        std::string s = "Really";
        v.emplace_back(s);

        while (!v.empty()) {
            Box<int, std::string> either(std::move(v.back()));
            v.pop_back();
            std::move(either).Unbox(
                    [](int i) { Print<int>(i); },
                    [](std::string s) { Print<std::string>(s); }
            );
        }
    }

    TEST_CASE("BoxedUnionCompiles") {
        std::vector<Box<int, std::string>> v;
        v.emplace_back(1);
        std::string s = "Really";
        v.emplace_back(s);

        while (!v.empty()) {
            Box<int, std::string> either(std::move(v.back()));
            v.pop_back();
            std::move(either).Unbox(
                    [](int i) { Print<int>(i); },
                    [](std::string s) { Print<std::string>(s); }
            );
        }
    }

    struct MyFunctor {
        int operator()(A a){
            return 0;
        }

        int operator()(B b){
            return 1;
        }
    };

    TEST_CASE("BoxedFunctor") {
        Box<A,B> either_a = A{};
        int x = std::move(either_a)
                .Unbox(MyFunctor{});

        Box<A,B> either_b = B{};
        int y = std::move(either_b)
                .Unbox(MyFunctor{});

        CHECK(x == 0);
        CHECK(y == 1);
    }

    struct MyFunctorPeek {
        int operator()(const A& a){
            return 0;
        }

        int operator()(const B& b) {
            return 1;
        }
    };

    TEST_CASE("BoxedFunctorPeek") {
        Box<A,B> either_a = A{};
        int x = either_a
                .Peek(MyFunctorPeek{});

        Box<A,B> either_b = B{};
        int y = either_b
                .Peek(MyFunctorPeek{});

        CHECK(x == 0);
        CHECK(y == 1);
    }

    struct MyStatefulFunctor {
        MyStatefulFunctor(int x) : x_(x) {}

        int operator()(A a){
            return x_ + 0;
        }

        int operator()(B b){
            return x_ + 1;
        }

    private:
        int x_;
    };

    TEST_CASE("BoxedStatefulFunctor") {
        Box<A,B> either_a = A{};
        int x = std::move(either_a)
                .Unbox(MyStatefulFunctor(1));

        Box<A,B> either_b = B{};
        int y = std::move(either_b)
                .Unbox(MyStatefulFunctor(1));

        CHECK(x == 1);
        CHECK(y == 2);
    }

    template <uint8_t v>
    using MyRange = ValueInSet<uint8_t , v, 1, 2, 3, 4, 5>;
    using BoxedMyRange = BoxValueInSet<uint8_t, 1, 2, 3, 4, 5>;
    using BoxedMyRangeMaker = BoxedUnionValueMaker<uint8_t, 1, 2, 3, 4, 5>;

    struct ValueFunc {
        template<uint8_t v>
        int operator()(MyRange<v> value) {
            return v;
        }
    };

    TEST_CASE("EitherValue") {
        auto v = MyRange<1>::make();
        BoxedMyRange bv(v);

        auto x = std::move(bv).Unbox(ValueFunc{});

        CHECK(x == 1);
    }

    struct ValueFuncPeek {
        template<uint8_t v>
        int operator()(const MyRange<v>& value) {
            return v;
        }
    };

    TEST_CASE("EitherValuePeek") {
        auto v = MyRange<1>::make();
        BoxedMyRange bv(v);

        auto x = bv.Peek(ValueFuncPeek{});

        CHECK(x == 1);
    }

    TEST_CASE("EitherValueAtRuntime") {
        auto box_or_error = BoxedMyRangeMaker::make(1);
        std::move(box_or_error).Unbox(
                [](BoxedMyRange b) {
                    int x = std::move(b).Unbox(ValueFunc{});
                    CHECK(x == 1);
                },
                [](ValueNotInSet) {
                    CHECK(false);
                }
        );
    }

    template <int v>
    using MyConsecutiveRange = ValueInRange<int, v, 1, 42>;
    using MyConsecutiveRangeBoxed = BoxValueInRange<int, 1, 42>;
    using MyConsecutiveRangeMaker = BoxValueInRangeMaker<int, 1, 42>;

    struct ValueInRangeFunc {
        template<int v>
        int operator()(MyConsecutiveRange<v> value) {
            return v;
        }
    };

    TEST_CASE("EitherValueInRange") {
        auto x= MyConsecutiveRange<1>::make();
        MyConsecutiveRangeBoxed b(x);

        int v  = std::move(b)
                .Unbox(ValueInRangeFunc{});
        CHECK(v == 1);
    }

    struct ValueInRangeFuncPeek {
        template<int v>
        int operator()(const MyConsecutiveRange<v>& value) {
            return v;
        }
    };

    TEST_CASE("EitherValueInRangePeek") {
        auto x= MyConsecutiveRange<1>::make();
        MyConsecutiveRangeBoxed b(x);

        int v  = b.Peek(ValueInRangeFunc{});
        CHECK(v == 1);
    }

    TEST_CASE("EitherValueInRange at runtime") {
        auto b = MyConsecutiveRangeMaker
                ::make(1);

        std::move(b).Unbox(
                [](MyConsecutiveRangeBoxed n) {
                    int v = std::move(n)
                            .Unbox(ValueInRangeFunc{});
                    CHECK(v == 1);
                },
                [](ValueNotInRange) {
                    CHECK(false);
                }
        );
    }

    struct C{};
    struct D{};

    TEST_CASE("FlatBoxedUnionCompiles") {
        using flat_abcd = Box<A,B,C,D>;

        using flat_ab_cd = FlatBox<Box<A,B>, Box<C,D>>;
        using flat_a_bcd = FlatBox<A, Box<B, C, D>>;
        using flat_abc_d = FlatBox<Box<A, B, C>, D>;
        using flat_a_bc_d = FlatBox<A, Box<B,C>, D>;

        static_assert(std::is_same_v<flat_abcd, flat_ab_cd>);
        static_assert(std::is_same_v<flat_abcd, flat_a_bcd>);
        static_assert(std::is_same_v<flat_abcd, flat_abc_d>);
        static_assert(std::is_same_v<flat_abcd, flat_a_bc_d>);

        using flat_nested = FlatBox<A, Box<B, Box<C,D>>>;
        static_assert(std::is_same_v<flat_abcd, flat_a_bc_d>);
    }
}
