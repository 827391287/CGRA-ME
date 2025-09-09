/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef UTILITY_FUNCTIONAL_H__
#define UTILITY_FUNCTIONAL_H__

#include <memory>
#include <mutex>

// forward declarations
template<typename,typename> struct LazyPointer;

/**
 * Construct a 'lazy pointer' -- won't call g until operator* or operator-> is used,
 *     and it will only call it once.
 * Be **very careful** around capture lifetimes.
 * Otherwise, this class behaves like a std::unique_ptr that
 *     propagates const to it's stored object.
 *
 * Ex.
 * struct S { int i; };
 * auto l1 = lazy_pointer([]() -> S { std::cout << "l1 computed! "; return {4}; });
 * std::cout << l1->i << l1->i << '\n'; // prints "l1 computed! 44";
 * auto l3 = lazy_pointer([]{ std::cout << "l3 computed! "; return 5; });
 * std::cout << *l3 << *l3 << '\n'; // prints "l3 comptuted! 55"
 *
 * Note: using a plain function pointer for the generator will result in a compile error
 *     about not being able to declare a mutable function pointer.
 *     To fix this, simply wrap your function pointer in a lambda.
 */
template<
    typename Gen,
    typename Value = std::remove_cv_t<std::remove_reference_t<decltype(std::declval<Gen>()())>>
>
auto lazy_pointer(Gen&& g) {
    return LazyPointer<Gen,Value>(std::forward<Gen>(g));
}

template<
    typename Gen,
    typename Value_ = std::remove_cv_t<std::remove_reference_t<decltype(std::declval<Gen>()())>>
>
struct LazyPointer {
    using Value = Value_;

    template<typename G>
    LazyPointer(G&& g) : gen(std::forward<G>(g)) { }

    LazyPointer(LazyPointer&& src)
        : gen(std::move(src.gen))
        , storage(std::move(src.storage))
    { }

    LazyPointer& operator=(LazyPointer&& rhs) {
        gen = std::move(rhs.gen);
        storage = std::move(rhs.storage);
    }

          Value& operator*()       { resolve(); return *storage; }
    const Value& operator*() const { resolve(); return *storage; }

          Value* operator->()       { resolve(); return storage.get(); }
    const Value* operator->() const { resolve(); return storage.get(); }

    bool isResolved() const { return (bool)storage; }

    void resolve() const {
        if (storage) { return; }
        call_once(is_resolved, [this]{
            storage = std::make_unique<Value>(gen());
        });
    }

private:
    mutable Gen gen;
    mutable std::unique_ptr<Value> storage = nullptr;
    mutable std::once_flag is_resolved = {};
};

struct AlwaysFalse {
    template<typename... T>
    bool operator()(T&&...) { return false; }
};

struct AlwaysTrue {
    template<typename... T>
    bool operator()(T&&...) { return true; }
};

struct DoNothing {
    template<typename... T>
    void operator()(T&&...) { return; }
};

/**
 * Makes a unary function object that will return true if the argument
 * is equal to `t`. (ie. the result of `operator==`)
 * A copy of t is made and stored in the function object.
 */
template<typename T>
auto returnsIfEqualTo(T t) {
    return [t=std::move(t)](auto& q) { return t == q; };
}

#endif /* UTILITY_FUNCTIONAL_H__ */
