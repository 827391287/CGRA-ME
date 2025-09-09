/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __CONFIG_STORE_H___
#define __CONFIG_STORE_H___

#include <CGRA/utility/Collections.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>

template<typename S1, typename S2>
bool equalsIgnoreCase(const S1& s1, const S2& s2) {
    auto i1 = begin(s1);
    auto i2 = begin(s2);
    for (; i1 != end(s1) && i2 != end(s2); ++i1, ++i2) {
        if (std::tolower(*i1) != std::tolower(*i2)) {
            return false;
        }
    }

    return i1 == end(s1) && i2 == end(s2);
}

template<typename S>
bool isTruthString(const S& s) {
    for (const char* test : { "on", "true", "yes"}) {
        if (equalsIgnoreCase(s, std::string(test))) {
            return true;
        }
    }
    return false;
}

template<typename S>
bool isFarceString(const S& s) {
    for (const char* test : { "off", "false", "no"}) {
        if (equalsIgnoreCase(s, std::string(test))) {
            return true;
        }
    }
    return false;
}

inline const char* makeBoolString(bool v) { return v ? "true" : "false"; }

struct ImplicitlyToString  {
    std::string impl;

    template<typename T, typename std::enable_if_t<std::is_integral<T>::value && not std::is_same<bool,T>::value, bool> = true>
    ImplicitlyToString(const T& number) : impl(std::to_string(number)) { }
    template<typename T, typename std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    ImplicitlyToString(const T& number) : impl(std::to_string(number)) { }

    ImplicitlyToString(std::string s) : impl(std::move(s)) { }
    ImplicitlyToString(const char* s) : ImplicitlyToString(std::string(s)) { }

    template<typename T, typename std::enable_if_t<std::is_same<bool,T>::value, bool> = true>
    ImplicitlyToString(const T& b) : impl(makeBoolString(b)) { }
};

/**
 * A class that helps with storing things like INI settings or
 * other simple key-value data. Provides convenience conversions
 * from the stored string data and supports in-code defaults.
 */
class ConfigStore {
public:
    using BackingStore = std::map<std::string, std::string>;

    ConfigStore() = default;

    ConfigStore(BackingStore src)
        : backing_store(std::move(src))
    { }


    /**
     * @brief Flexible constructor that converts everything value it is given to a string.
     *
     * Keys must be string due to ambiguity (eg. what is a canonical conversion for bools?), and not having a clear use-case.
     *
     * Ex.:
     * @code{.cpp}
     * ConfigStore c {
     *     {"0", false},
     *     {"1", "1"},
     *     {"two", 2},
     *     {"3", std::string("3")},
     *     {"4", returnsBool(arg4)},
     *     {"five", returnsInt(arg5)},
     * };
     * @endcode
     */
    ConfigStore(std::initializer_list<std::pair<std::string, ImplicitlyToString>> ilist)
        : backing_store()
    {
        for (auto& kv : ilist) {
            addString(kv.first, kv.second.impl); // cannot move. initializer_list forces copy
        }
    }

    ConfigStore(const ConfigStore&) = default;
    ConfigStore& operator=(const ConfigStore&) = default;
    ConfigStore(ConfigStore&&) = default;
    ConfigStore& operator=(ConfigStore&&) = default;

    /**
     * Inserts `value` under the key `key`. Does not overwrite.
     */
    bool addString(std::string key, std::string value) { return backing_store.emplace(std::move(key), std::move(value)).second; }
    bool    addInt(std::string key, long long value)   { return addString(std::move(key), std::to_string(value)); }
    bool   addReal(std::string key, double value)      { return addString(std::move(key), std::to_string(value)); }
    bool   addBool(std::string key, bool value)        { return addString(std::move(key), makeBoolString(value)); }

    /**
     * Sets `key` to have the value `value`. Will overwrite.
     */
    void setString(std::string key, std::string value) { insert_or_assign(backing_store, std::move(key), std::move(value)); }
    void    setInt(std::string key, long long value)   { setString(std::move(key), std::to_string(value)); }
    void   setReal(std::string key, double value)      { setString(std::move(key), std::to_string(value)); }
    void   setBool(std::string key, bool value)        { setString(std::move(key), makeBoolString(value)); }

    /**
     * The simple getters. Throw exceptions if the key is not found.
     */
    const std::string& getString(const std::string& key) const {
        try {
            return backing_store.at(key);
        } catch (const std::out_of_range& oor) {
            throw std::out_of_range(
                "ConfigStore accessed with non-existent key `" + key + "'. Original exception's .what() = `" + oor.what() + "'"
            );
        }
    }

    long long getInt(const std::string& key) const {
        const auto& value = getString(key);
        std::size_t final_pos = 0;
        auto result = std::stoll(value, &final_pos);
        if (final_pos != value.size()) {
            throw makeIncompleteConversionException(key, value);
        }
        return result;
    }

    double getReal(const std::string& key) const {
        const auto& value = getString(key);
        std::size_t final_pos = 0;
        auto result = std::stod(value, &final_pos);
        if (final_pos != value.size()) {
            throw makeIncompleteConversionException(key, value);
        }
        return result;
    }

    bool getBool(const std::string& key) const {
        const auto& value = getString(key);
        if (isTruthString(value)) {
            return true;
        }
        if (isFarceString(value)) {
            return false;
        }

        return getInt(key) != 0;
    }

    /**
     * The defaulted getters. Return the second argument if the key has no value.
     */
    const std::string& getStringOr(const std::string& key, const std::string& otherwise) const {
        if (hasKey(key)) { return getString(key); }
        else { return otherwise; }
    }

    std::string getStringOr(const std::string& key, std::string&& otherwise) const {
        if (hasKey(key)) { return getString(key); }
        else { return std::move(otherwise); }
    }

    long long getIntOr(const std::string& key, long long otherwise) const {
        if (hasKey(key)) { return getInt(key); }
        else { return otherwise; }
    }

    double getRealOr(const std::string& key, double otherwise) const {
        if (hasKey(key)) { return getInt(key); }
        else { return otherwise; }
    }

    bool getBoolOr(const std::string& key, bool otherwise) const {
        if (hasKey(key)) { return getBool(key); }
        else { return otherwise; }
    }

    /**
     * Returns true iff key has a value
     */
    bool hasKey(const std::string& key) const {
        return backing_store.find(key) != backing_store.end();
    }

    auto begin() const { return backing_store.begin(); }
    auto end()   const { return backing_store.end(); }
    bool empty() const { return backing_store.empty(); }

    bool operator==(const ConfigStore& rhs) const {
        return this->backing_store == rhs.backing_store;
    }
    bool operator!=(const ConfigStore& rhs) const { return not (*this == rhs); }

    /**
     * Print out the contents in a neat way.
     */
    friend std::ostream& operator<<(std::ostream& os, const ConfigStore& cs) {
        std::ptrdiff_t max_key_length = 0;
        for (const auto& key_and_value : cs.backing_store) {
            max_key_length = std::max(max_key_length, (std::ptrdiff_t)key_and_value.first.size());
        }

        os << "{\n";
        for (const auto& key_and_value : cs.backing_store) {
            os << std::setw(max_key_length + 2) << key_and_value.first << " = " << key_and_value.second << '\n';
        }
        os << '}';
        return os;
    }

private:
    std::invalid_argument makeIncompleteConversionException(const std::string& key, const std::string& value) const {
        return std::invalid_argument("value `" + value + "' at key `" + key + "' could not be completely converted");
    }

    BackingStore backing_store = {};
};


/**
 * @brief Add (or set) all keys from @p from in @p into
 */
inline ConfigStore& add_all(ConfigStore& into, const ConfigStore& from) { for (auto& kv : from) { into.addString(kv.first, kv.second); } return into; }
inline ConfigStore& set_all(ConfigStore& into, const ConfigStore& from) { for (auto& kv : from) { into.setString(kv.first, kv.second); } return into; }

/**
 * Like `set_all` but throws if a key doesn't exist
 */
inline ConfigStore& override_all(ConfigStore& into, const ConfigStore& from) {
    for (auto& kv : from) {
        if (not into.hasKey(kv.first)) { throw std::out_of_range(
            "attempted to override non-existent key " + kv.first + " with value " + kv.second
        );}
        into.setString(kv.first, kv.second);
    }
    return into;
}

// variadic lvalue overloads
template<typename... CSes> ConfigStore& add_all     (ConfigStore& into, const ConfigStore& from1, const CSes&... froms) { add_all(into, from1);      (void)(int[]){(add_all(into, froms),0)...};      return into; }
template<typename... CSes> ConfigStore& set_all     (ConfigStore& into, const ConfigStore& from1, const CSes&... froms) { set_all(into, from1);      (void)(int[]){(set_all(into, froms),0)...};      return into; }
template<typename... CSes> ConfigStore& override_all(ConfigStore& into, const ConfigStore& from1, const CSes&... froms) { override_all(into, from1); (void)(int[]){(override_all(into, froms),0)...}; return into; }

// variadic rvalue overloads
template<typename... CSes> ConfigStore add_all     (ConfigStore&& into, const ConfigStore& from1, const CSes&... froms) { return add_all(into, from1, froms...); }
template<typename... CSes> ConfigStore set_all     (ConfigStore&& into, const ConfigStore& from1, const CSes&... froms) { return set_all(into, from1, froms...); }
template<typename... CSes> ConfigStore override_all(ConfigStore&& into, const ConfigStore& from1, const CSes&... froms) { return override_all(into, from1, froms...); }

// These always operate on (and return) a copy
template<typename... CSes> ConfigStore with_added     (ConfigStore into, const ConfigStore& from1, const CSes&... froms) { return add_all(std::move(into), from1, froms...); }
template<typename... CSes> ConfigStore with_set       (ConfigStore into, const ConfigStore& from1, const CSes&... froms) { return set_all(std::move(into), from1, froms...); }
template<typename... CSes> ConfigStore with_overridden(ConfigStore into, const ConfigStore& from1, const CSes&... froms) { return override_all(std::move(into), from1, froms...); }

/**
 * Return a (new) ConfigStore, that is @p cs filtered to only contain entries that start with @p prefix, with that prefix removed.
 */
inline ConfigStore getEntriesForPrefix(const ConfigStore& cs, const std::string& prefix) {
    ConfigStore result;
    for (const auto& kv : cs) {
        if (kv.first.substr(0, prefix.size()) == prefix) {
            result.setString(kv.first.substr(prefix.size()), kv.second);
        }
    }
    return result;
}

/**
 * Return a (new) ConfigStore with all the keys prefixed with @p prefix
 */
inline ConfigStore addPrefix(const ConfigStore& cs, const std::string& prefix) {
    ConfigStore result;
    for (const auto& kv : cs) {
        result.setString(prefix + kv.first, kv.second);
    }
    return result;
}


/**
 * Return a (new) ConfigStore with only the allowed keys; keys that the filter returns true
 */
template<typename Filter>
auto filterKeys(const ConfigStore& cs, Filter&& f) -> decltype(f(std::string()), ConfigStore()) {
    ConfigStore result;
    for (const auto& kv : cs) {
        if (std::forward<Filter>(f)(kv.first)) {
            result.addString(kv.first, kv.second);
        }
    }
    return result;
}

/**
 * 2-arg (key, value) overload
 */
template<typename Filter>
auto filterKeys(const ConfigStore& cs, Filter&& f) -> decltype(f(std::string(),std::string()), ConfigStore()) {
    ConfigStore result;
    for (const auto& kv : cs) {
        if (std::forward<Filter>(f)(kv.first, kv.second)) {
            result.addString(kv.first, kv.second);
        }
    }
    return result;
}

#endif /* __CONFIG_STORE_H___ */
