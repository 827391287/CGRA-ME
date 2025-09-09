/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __UTILITY_COLLECTIONS_H__
#define __UTILITY_COLLECTIONS_H__

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

/**
 * Make two iterators behave like a collection
 */
template<typename BIter, typename EIter = BIter>
struct iterator_range {
    BIter b;
    EIter e;

    template<typename BIter_C, typename EIter_C = BIter_C>
    iterator_range(BIter_C&& b_, EIter_C&& e_)
        : b(std::forward<BIter_C>(b_))
        , e(std::forward<EIter_C>(e_))
    { }

    iterator_range(const iterator_range&) = default;
    iterator_range& operator=(const iterator_range&) = default;
    iterator_range(iterator_range&&) = default;
    iterator_range& operator=(iterator_range&&) = default;

    const BIter& begin() const { return b; }
          BIter& begin()       { return b; }

    const EIter& end() const { return e; }
          EIter& end()       { return e; }

    bool empty() const { return begin() != end(); }
};

template<typename BIter, typename EIter = BIter>
auto make_iterator_range(BIter&& b, EIter&& e) {
    using irange_type = iterator_range< std::decay_t<BIter>, std::decay_t<EIter> >;
    return irange_type(std::forward<BIter>(b), std::forward<EIter>(e));
}

template<typename BIter, typename EIter = BIter>
auto make_iterator_range(const std::pair<BIter, EIter>& p) {
    using irange_type = iterator_range< std::decay_t<BIter>, std::decay_t<EIter> >;
    return irange_type(p.first, p.second);
}

template<typename Range, typename = decltype(begin(std::declval<Range>()))>
auto make_iterator_range(const Range& r) {
    using std::begin;
    using std::end;
    return make_iterator_range(begin(r), end(r));
}

/**
 * Make a range of tuples to appear as a range of just one element of the tuples
 * A particular use case is for easy iteration of key and value lists of associative
 *   types like std::*map
 */
template<int get_index, typename Range>
struct tuple_get_range {
    Range r;
    using Biter = std::remove_cv_t<std::remove_reference_t<decltype(r.begin())>>;
    using Eiter = std::remove_cv_t<std::remove_reference_t<decltype(r.end())>>;

    template<typename Iter>
    struct iterator {
        Iter impl;
        iterator& operator++() { ++impl; return *this; }
        auto& operator*()        { using std::get; return get<get_index>(*impl); }
        auto& operator*()  const { using std::get; return get<get_index>(*impl); }
        auto* operator->()       { return &this->operator*(); }
        auto* operator->() const { return &this->operator*(); }

        template<typename RHSIter>
        bool operator==(const iterator<RHSIter>& rhs) {return this->impl == rhs.impl; }
        template<typename RHSIter>
        bool operator!=(const iterator<RHSIter>& rhs) {return this->impl != rhs.impl; }
        template<typename RHSIter>
        bool operator< (const iterator<RHSIter>& rhs) {return this->impl <  rhs.impl; }
    };

    iterator<Biter> begin()       { return {r.begin()}; }
    iterator<Eiter>   end()       { return {r.end()}; }
    iterator<Biter> begin() const { return {((const Range)r).begin()}; }
    iterator<Eiter>   end() const { return {((const Range)r).end()}; }
};

template<int get_index, typename Range>
tuple_get_range<get_index, Range> make_tuple_get_range(Range&& r) {
    return {r};
}

template<typename Range>
tuple_get_range<1, Range> make_second_of_pair_range(Range&& r) {
    return {r};
}

template<typename Range>
tuple_get_range<0, Range> make_first_of_pair_range(Range&& r) {
    return {r};
}

template<typename Range>
auto reversed(Range& r) {
    return make_iterator_range(r.rbegin(), r.rend());
}

/**
 * For when you want something that behaves similar
 * to a `std::.*set`, but with just one element.
 */
template<typename VertexID>
struct SingleItemImmutableSet {
    const VertexID v;

    auto begin() const { return &v; }
    auto end() const { return &v + 1; }

    template<typename Q> auto find(const Q& q) const { return (v == q) ? begin() : end(); }
    template<typename Q> std::ptrdiff_t count(const Q& q) { return (v == q) ? 1 : 0; }
};

template<typename VertexID> auto cbegin(const SingleItemImmutableSet<VertexID>& siis) { return siis.begin(); }
template<typename VertexID> auto   cend(const SingleItemImmutableSet<VertexID>& siis) { return siis.end();   }
template<typename VertexID> auto  begin(const SingleItemImmutableSet<VertexID>& siis) { return siis.begin(); }
template<typename VertexID> auto    end(const SingleItemImmutableSet<VertexID>& siis) { return siis.end();   }
template<typename VertexID> auto  begin(      SingleItemImmutableSet<VertexID>& siis) { return siis.begin(); }
template<typename VertexID> auto    end(      SingleItemImmutableSet<VertexID>& siis) { return siis.end();   }

template<typename VertexID>
SingleItemImmutableSet<VertexID> singleItemSet(VertexID v) {
    return { std::move(v) };
}

template<template <typename...> class Map, typename... InitialParams>
struct StandardMapMaker {
    template<typename... RestParams>
    auto operator()() const {
        return Map<InitialParams..., RestParams...>();
    }
};

template<template <typename...> class Set, typename... InitialParams>
struct StandardSetMaker {
    template<typename... RestParams>
    auto operator()() const {
        return Set<InitialParams..., RestParams...>();
    }
};


/**
 * @brief Provides some standard map operations, with a fixed size
 */
template<typename IndexMap, typename Key, typename MappedType>
struct VectorMap {
    using value_type = std::pair<Key, MappedType>;
    using DataStore = std::vector<value_type>;

    struct const_iterator {
        typename DataStore::const_iterator impl;
        auto& operator*() const { return *impl; }
        auto* operator->() const { return &*impl; }
        bool operator==(const const_iterator& rhs) const { return impl == rhs.impl; }
        bool operator!=(const const_iterator& rhs) const { return impl != rhs.impl; }
    };
    struct iterator {
        typename DataStore::iterator impl;
        auto& operator*() const { return *impl; }
        auto* operator->() const { return &*impl; }
        bool operator==(const iterator& rhs) const { return impl == rhs.impl; }
        bool operator!=(const iterator& rhs) const { return impl != rhs.impl; }
        operator const_iterator() const { return {impl}; }
    };

    VectorMap(std::size_t size) : VectorMap(size, IndexMap()) {}
    VectorMap(std::size_t size, IndexMap imap)
        : imap(std::move(imap))
        , data(size, {imap.invalid_key(), MappedType()}) {}

    const MappedType& at(const Key& k) const {
        return iterAt(k)->second;
    }
    MappedType& at(const Key& k) {
        return iterAt(k)->second;
    }
    MappedType& operator[](const Key& k) {
        auto i = iterAt(k);
        i->first = k;
        return i->second;
    }

    std::pair<iterator, bool> insert(value_type vt) {
        auto i = iterAt(vt.first);
        bool had_no_data = not has_data(i);
        if (had_no_data) *i = vt;
        return {i, had_no_data};
    }

    iterator find(const Key& k) {
        auto i = iterAt(k);
        return has_data(i) ? i : end();
    }
    const_iterator find(const Key& k) const {
        auto i = iterAt(k);
        return has_data(i) ? i : end();
    }

    std::size_t count(const Key& k) const { return find(k) == end() ? 0 : 1; }

    iterator end() { return {data.end()}; }
    const_iterator end() const { return {data.end()}; }

  private:
    iterator iterAt(const Key& k) { return {data.begin() + imap(k)}; }
    const_iterator iterAt(const Key& k) const { return {data.begin() + imap(k)}; }
    bool has_data(const_iterator i) const { return i->first != imap.invalid_key(); }

    DataStore data;
    IndexMap imap;
};

/**
 * @brief Provides some standard set operations, with a fixed size
 */
template<typename IndexMap, typename Value>
struct VectorSet {
    using value_type = Value;
    using DataStore = std::vector<value_type>;

    struct const_iterator {
        typename DataStore::const_iterator impl;
        auto& operator*() const { return *impl; }
        auto* operator->() const { return &*impl; }
        bool operator==(const const_iterator& rhs) const { return impl == rhs.impl; }
        bool operator!=(const const_iterator& rhs) const { return impl != rhs.impl; }
    };
    struct iterator {
        typename DataStore::iterator impl;
        auto& operator*() const { return *impl; }
        auto* operator->() const { return &*impl; }
        bool operator==(const iterator& rhs) const { return impl == rhs.impl; }
        bool operator!=(const iterator& rhs) const { return impl != rhs.impl; }
        operator const_iterator() const { return {impl}; }
    };

    VectorSet(std::size_t size) : VectorSet(size, IndexMap()) {}
    VectorSet(std::size_t size, IndexMap imap)
        : imap(std::move(imap))
        , data(size, imap.invalid_key()) {}

    std::pair<iterator, bool> insert(value_type vt) {
        auto i = iterAt(vt);
        bool had_no_data = not has_data(i);
        if (had_no_data) *i = vt;
        return {i, had_no_data};
    }

    iterator find(const value_type& k) {
        auto i = iterAt(k);
        return has_data(i) ? i : end();
    }
    const_iterator find(const value_type& k) const {
        auto i = iterAt(k);
        return has_data(i) ? i : end();
    }

    std::size_t count(const value_type& k) const { return find(k) == end() ? 0 : 1; }

    iterator end() { return {data.end()}; }
    const_iterator end() const { return {data.end()}; }

  private:
    iterator iterAt(const value_type& k) { return {data.begin() + imap(k)}; }
    const_iterator iterAt(const value_type& k) const { return {data.begin() + imap(k)}; }
    bool has_data(iterator i) const { return *i != imap.invalid_key(); }

    IndexMap imap;
    DataStore data;
};

template<typename... InitialParams>
struct VectorMapMaker {
    std::size_t size;
    template<typename... RestParams>
    auto operator()() const {
        return VectorMap<InitialParams..., RestParams...>(size);
    }
};

template<typename... InitialParams>
struct VectorSetMaker {
    std::size_t size;
    template<typename...>
    auto operator()() const {
        return VectorSet<InitialParams...>(size);
    }
};

/**
 * Copies elements from `base_container` to a default-constructed instance of `COLLECTION`.
 * Only copies element where filter returns a truth value.
 */
template<typename COLLECTION, typename UNARY_PREDICATE>
auto filter_collection(const COLLECTION& base_container, UNARY_PREDICATE&& filter) {
    COLLECTION result;
    std::copy_if(begin(base_container), end(base_container), std::inserter(result, end(result)), filter);
    return result;
}

/**
 * Do the ranges `r1` and `r2` have the same elements, and are they the same size?
 * Uses `operator==`.
 */
template<typename R1, typename R2>
bool are_ranges_same(const R1& r1, const R2& r2) {
    return std::mismatch(begin(r1), end(r1), begin(r2), end(r2)) == std::make_pair(end(r1), end(r2));
}

/**
 * Performs the common pattern of looking up an item in a map, and giving some default
 * value instead of throwing or giving an "end" iterator.
 * Note: only returns by reference if `default_value` is a lvalue.
 * uses `ASSOCIATIVE_COLLECTION::find(const KEY&)`.
 */
template<typename ASSOCIATIVE_COLLECTION, typename KEY, typename DEFAULT_VALUE>
auto value_for_key_or(ASSOCIATIVE_COLLECTION& assoc_collection, const KEY& key, DEFAULT_VALUE& default_value)
    -> std::conditional_t<
        std::is_lvalue_reference<DEFAULT_VALUE>::value,
        decltype(false ? default_value : assoc_collection.find(key)->second)&,
        decltype(false ? default_value : assoc_collection.find(key)->second)
    >
{
    const auto search_result = assoc_collection.find(key);
    if (search_result == end(assoc_collection)) {
        return default_value;
    } else {
        return search_result->second;
    }
}

/**
 * C++17 member function.
 */
template<typename ASSOCIATIVE_COLLECTION, typename KEY, typename VALUE>
auto insert_or_assign(ASSOCIATIVE_COLLECTION& assoc_collection, KEY&& key, VALUE&& value) {
    const auto search_result = assoc_collection.find(key);
    if (search_result == end(assoc_collection)) {
        return assoc_collection.emplace(std::forward<KEY>(key), std::forward<VALUE>(value));
    } else {
        search_result->second = std::forward<KEY>(value);
        return std::make_pair(search_result, true);
    }
}

/**
 * Same as std::find_first_of, but returns a pair of iterators, one from each range.
 * Modified from https://en.cppreference.com/w/cpp/algorithm/find_first_of
 */
template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
std::pair<InputIt,ForwardIt> find_first_of_and_matching(
    InputIt first, InputIt last,
    ForwardIt s_first, ForwardIt s_last,
    BinaryPredicate p
) {
    for (; first != last; ++first) {
        for (ForwardIt it = s_first; it != s_last; ++it) {
            if (p(*first, *it)) {
                return {first,it};
            }
        }
    }
    return {last, s_last};
}

// version of abvove without a predicate, uses operator==
template<typename InputIt, typename ForwardIt>
auto find_first_of_and_matching(
    InputIt first, InputIt last,
    ForwardIt s_first, ForwardIt s_last
) {
    return find_first_of_and_matching(
        first, last, s_first, s_last,
        [](auto&& lhs, auto&& rhs) { return lhs == rhs; }
    );
}

// range version of above
template<typename Range1, typename Range2, typename BinaryPredicate>
auto find_first_of_and_matching(Range1&& r1, Range2& r2, BinaryPredicate p) {
    using std::begin; using std::end;
    return find_first_of_and_matching(begin(r1), end(r1), begin(r2), end(r2), std::move(p));
}

// range version of above, uses operator==
template<typename Range1, typename Range2>
auto find_first_of_and_matching(Range1&& r1, Range2& r2) {
    using std::begin; using std::end;
    return find_first_of_and_matching(begin(r1), end(r1), begin(r2), end(r2));
}

namespace detail {
    struct shift_in_printer {
    template<typename STREAM, typename T>
        void operator()(STREAM& os, const T& t) const {
            os << t;
        }
    };
}

/**
 * Prints a container to `os`, using `operator<<`. Defaults to printing elements using `operator<<`,
 * but this can be overridden with the 'printer' argument.
 * Printing is of the form:
 * `container_prefix` `element_printer(elem1)` `element_separator` `element_printer(os,elem2)` `element_separator` ... `container_suffix`
 */
template<
    typename CONTAINER,
    typename OSTREAM,
    typename PRINTER = detail::shift_in_printer,
    typename T1, typename T2, typename T3
>
void print_container(
    OSTREAM&& os,
    const CONTAINER& c,
    const T1& element_separator,
    const T2& container_prefix,
    const T3& container_suffix,
    PRINTER&& element_printer = PRINTER{}
) {
    using std::begin; using std::end;
    auto beg = begin(c);
    auto en = end(c);

    os << container_prefix;
    if (beg != en) {
        element_printer(os,*beg);
        std::for_each(std::next(beg), en, [&](const auto& v){
            os << element_separator;
            element_printer(os,v);
        });
    }
    os << container_suffix;
}

/**
 * Prints the given container `c` to `os` in the format:
 * "{ " elem1 ", " elem2 ", " ... " }"
 *
 * ex.:
 * std::vector<int> nums;
 * print_container(std::cout, nums);
 */
template<typename CONTAINER, typename OSTREAM, typename PRINTER = detail::shift_in_printer>
void print_container(
    OSTREAM&& os,
    const CONTAINER& c,
    PRINTER&& element_printer = PRINTER{}
) {
    print_container(os, c, ", ", "{ ", " }", element_printer);
}

/**
 * Prints an associative container to `os`, using `operator<<`. Defaults to printing elements using `operator<<`,
 * but this can be overridden with the 'printer' arguments. Value type must model `std::pair`.
 * Printing is of the form:
 * `container_prefix` `element_prefix` `key_printer(os,key1)` `key_value_separator` `value_printer(os,val1)` `element_suffix` `element_separator` ... `container_suffix`
 */
template<
    typename ASSOC_CONTAINER,
    typename OSTREAM,
    typename KEY_PRINTER = detail::shift_in_printer,
    typename VALUE_PRINTER = detail::shift_in_printer,
    typename T1, typename T2, typename T3, typename T4, typename T5, typename T6
>
void print_assoc_container(
    OSTREAM&& os,
    const ASSOC_CONTAINER& c,
    const T1& element_separator,
    const T2& key_value_separator,
    const T3& element_prefix,
    const T4& element_suffix,
    const T5& container_prefix,
    const T6& container_suffix,
    KEY_PRINTER&& value_printer = KEY_PRINTER{},
    VALUE_PRINTER&& key_printer = VALUE_PRINTER{}
) {
    using std::begin; using std::end;
    auto beg = begin(c);
    auto en = end(c);

    os << container_prefix;
    if (beg != en) {
        os << element_prefix;
        key_printer(os,beg->first);
        os << key_value_separator;
        value_printer(os,beg->second);
        os << element_suffix;
        std::for_each(std::next(beg), en, [&](const auto& v){
            os << element_separator;
            os << element_prefix;
            key_printer(os,v.first);
            os << key_value_separator;
            value_printer(os,v.second);
            os << element_suffix;
        });
    }
    os << container_suffix;
}

/**
 * Prints the given associative container `c` to `os` in the format:
 * {{ key1 -> val1 }, { key2 -> val2 }, ...}
 *
 * ex.:
 * std::unordered_map<int, float> nums;
 * print_assoc_container(std::cout, nums);
 */
template<typename ASSOC_CONTAINER, typename OSTREAM, typename KEY_PRINTER = detail::shift_in_printer, typename VALUE_PRINTER = detail::shift_in_printer>
void print_assoc_container(
    OSTREAM&& os,
    const ASSOC_CONTAINER& c,
    KEY_PRINTER&& value_printer = KEY_PRINTER{},
    VALUE_PRINTER&& key_printer = VALUE_PRINTER{}
) {
    print_assoc_container(os, c, ", ", " -> ", "{ ", " }", "{", "}", value_printer, key_printer);
}

template<typename T = std::vector<int>>
T make_zero_to_n_minus_one_list(std::ptrdiff_t n) {
    T result(n);
    std::iota(result.begin(), result.end(), 0);
    return result;
}

#endif /* __UTILITY_COLLECTIONS_H__ */
