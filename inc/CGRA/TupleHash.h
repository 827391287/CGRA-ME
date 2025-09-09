/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __TUPLE_HASH_H__
#define __TUPLE_HASH_H__

/**
 * Inspired by https://gist.github.com/nsuke/5990643
 * Initially used in CGRA-ME to provide generic hashing of data
 * for ConstraintSet.h's ConstraintID.
 */

#include <tuple>
namespace tuple_hash {
    namespace {
        template<class T>
        [[nodiscard]]
        std::size_t hash_combine(std::size_t seed, const T& v) {
            return seed ^ (std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2));
        }

        template<std::size_t INDEX>
        struct hashvalueimpl {
            template<typename TUPLE>
            [[nodiscard]]
            static size_t apply(std::size_t seed, const TUPLE& tuple) {
                return hash_combine(
                    hashvalueimpl<INDEX-1>::apply(seed, tuple),
                    std::get<INDEX>(tuple)
                );
            }
        };

        template<>
        struct hashvalueimpl<0> {
            template<typename TUPLE>
            [[nodiscard]]
            static size_t apply(std::size_t seed, const TUPLE& tuple) {
                return hash_combine(seed, std::get<0>(tuple));
            }
        };
    }

    template<typename...>
    struct hash;

    template<typename... TT>
    struct hash<std::tuple<TT...>>  {
        template<typename TUPLE>
        std::size_t operator()(const TUPLE& tuple) const {
            std::size_t seed = 0;
            return hashvalueimpl<std::tuple_size<TUPLE>::value - 1>::apply(seed, tuple);
        }
    };

    template<typename T>
    std::size_t hashValue(const T& t) {
        return hash<T>{}(t);
    }

    /**
     * Help for making your simple struct hashable. Provides
     * a constructor that takes one argument for each member
     * exposed through asTuple().
     *
     * ex:
     *
     * struct MyStruct : tuple_hash::TupleHashOps<MyStruct> {  // be sure update the template parameter to the base class
     *    int i,j;
     *
     *    MyStruct(int i, int j) : i(i), j(j) { }               // required until c++17 (inheriting precludes aggregate property)
     *    auto asTuple() const { return std::make_tuple(i,j); } // all you have to do is provide this function. Retuning std::tie is fine.
     * };
     *
     * std::unordered_map<MyStruct, int, MyStruct::Hash> myMap;
     * mySet.emplace(MyStruct{1,1}, 1);
     */
    template<typename Derived>
    struct TupleHashOps {
        const Derived& asDerived() const { return *static_cast<const Derived*>(this); }
              Derived& asDerived()       { return *static_cast<      Derived*>(this); }

        bool operator==(const Derived& rhs) const { return asDerived().asTuple() == rhs.asTuple(); }
        bool operator<(const Derived& rhs) const { return asDerived().asTuple() < rhs.asTuple(); }

        struct Hash { std::size_t operator()(const Derived& d) const {
            return hashValue(d.asTuple());
        }};
    };
}

/**
 * C++14's tuple doesn't support any sort of implicit construction.
 * Should not be needed if we upgrade to C++17
 * Also, try not to use for too many things, as you can't easily convert from std::vector<ImplicitTuple<...>> to std::vector<std::tuple<...>> for example.
 */
template<typename... CArgs>
struct ImplicitTuple : std::tuple<CArgs...> {
    using Base = std::tuple<CArgs...>;

    template<typename Arg>
    explicit ImplicitTuple(Arg&& arg) : Base(std::forward<Arg>(arg)) { }

    ImplicitTuple(Base arg) : Base(std::move(arg)) { }

    template<typename Arg1, typename... Args>
    ImplicitTuple(Arg1&& arg1, Args&&... args) : Base(std::forward<Arg1>(arg1), std::forward<Args>(args)...) { }

    using Base::tuple;
    using Base::operator=;
};

#endif /* __TUPLE_HASH_H__ */
