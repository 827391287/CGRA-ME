/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __CONSTRAINT_SET_H__
#define __CONSTRAINT_SET_H__

#include <CGRA/ILPMapper.h>
#include <CGRA/TupleHash.h>

#include <algorithm>
#include <stdexcept>
#include <functional>

#ifdef USE_GUROBI
#include <gurobi_c++.h>

/**
 * A better GRBCallback class to inherit from - now others can access the callback API
 */
class GRBCallbackPublic : public GRBCallback {
public:
    GRBCallbackPublic() = default;
    GRBCallbackPublic(const GRBCallbackPublic&) = default;
    GRBCallbackPublic& operator=(const GRBCallbackPublic&) = default;
    GRBCallbackPublic(GRBCallbackPublic&&) = default;
    GRBCallbackPublic& operator=(GRBCallbackPublic&&) = default;

    using GRBCallback::where;

    using GRBCallback::callback;
    using GRBCallback::getDoubleInfo;
    using GRBCallback::getIntInfo;
    using GRBCallback::getStringInfo;
    using GRBCallback::getSolution;
    using GRBCallback::getNodeRel;
    using GRBCallback::setSolution;
    //using GRBCallback::useSolution;
    using GRBCallback::addCut;
    using GRBCallback::addLazy;
    using GRBCallback::abort;
};

/**
 * Abstracts the task of running code every time a solution is found
 */
template<typename FUNC>
class OnMIPSolutionFunctorGRBCallback : public GRBCallbackPublic {
public:
    OnMIPSolutionFunctorGRBCallback(FUNC on_MIP_solution_functor)
        : on_MIP_solution_functor(std::move(on_MIP_solution_functor))
    { }

    void callback() override {
        if (where == GRB_CB_MIPSOL) {
            onMIPSolution();
        }
    }

    void onMIPSolution() {
        on_MIP_solution_functor(*this);
    }

private:
    FUNC on_MIP_solution_functor;
};

template<typename FUNC>
auto makeOnMIPSolutionFunctorGRBCallback(FUNC&& on_MIP_solution_functor) {
    return OnMIPSolutionFunctorGRBCallback<FUNC>(std::forward<FUNC>(on_MIP_solution_functor));
}

/**
 * Similar to std::optional... but we aren't on c++17
 * Throws if you try to deference it and it doesn't have anything.
 * Used to (optionally) pass the callback to constraint generators
 * so they can access the current values of ILP variables.
 */
class MaybeGRBCallback {
public:
    MaybeGRBCallback(GRBCallbackPublic* impl)
        : impl(impl)
    { }

    bool hasImpl() const { return impl != nullptr; }
    operator bool() const { return hasImpl(); }

    GRBCallbackPublic& get() const {
        if (not hasImpl()) { throw std::logic_error("Cannot get implementation - implementation not provided/implementation pointer is null"); }
        else { return *impl; }
    }

    GRBCallbackPublic& operator*() const { return get(); }
    GRBCallbackPublic* operator->() const { return &(**this); }

private:
    GRBCallbackPublic* impl;
};


/**
 * Base class for all the constraints identifiers.
 * Constraint identifiers are used to avoid build up sets of
 * constraints that have been suggested, so that they can be automatically
 * re-added (recall, Gurobi may "forget" lazily constraints added via callback)
 */
struct ConstraintID {
    virtual bool operator==(const ConstraintID&) const = 0;
    virtual std::size_t hash() const = 0;
    virtual ~ConstraintID() = default;
};

namespace std {
    template<>
    struct hash<ConstraintID> {
        std::size_t operator()(const ConstraintID& cid) const {
            return cid.hash();
        }
    };
}

/**
 * A constraint identifier that is backed by a single value.
 * Useful for eg. when each constraint can be identified by a single
 * integer (eg. a counter) or even a string. Supports anything that
 * specializes std::hash.
 */
template<typename REPR>
struct StandardConstraintID : ConstraintID {

    REPR data;

    template<typename T>
    StandardConstraintID(T&& t)
        : data(t)
    { }

    StandardConstraintID(const StandardConstraintID&) = default;
    StandardConstraintID& operator=(const StandardConstraintID&) = default;
    StandardConstraintID(StandardConstraintID&&) = default;
    StandardConstraintID& operator=(StandardConstraintID&&) = default;

    bool operator==(const ConstraintID& rhs) const override {
        auto* rhs_ptr = dynamic_cast<const StandardConstraintID*>(&rhs);
        return rhs_ptr && this->data == rhs_ptr->data;
    }

    std::size_t hash() const override {
        return std::hash<REPR>{}(data);
    }
};

/**
 * A constraint identifier that is backed by a tuple of values.
 * Useful for eg. when constraints are generated in a nested loop - just
 * have one tuple element for each loop index. Supports anything that
 * specializes std::hash.
 */
template<typename... TUP_ARGS>
struct TupleConstraintID : ConstraintID {

    std::tuple<TUP_ARGS...> data;

    template<typename... T>
    TupleConstraintID(T&&... t)
        : data(std::forward<T>(t)...)
    { }

    TupleConstraintID(const TupleConstraintID&) = default;
    TupleConstraintID& operator=(const TupleConstraintID&) = default;
    TupleConstraintID(TupleConstraintID&&) = default;
    TupleConstraintID& operator=(TupleConstraintID&&) = default;

    bool operator==(const ConstraintID& rhs) const override {
        auto* rhs_ptr = dynamic_cast<const TupleConstraintID*>(&rhs);
        return rhs_ptr && this->data == rhs_ptr->data;
    }

    std::size_t hash() const override {
        return tuple_hash::hashValue(data);
    }
};

/**
 * Helper for creating ConstraintIDs - guesses template parameters
 * Also useful because certain old versions of GCC don't supply an implicit constructor
 * for unique_ptr<Base> from a Derived pointer...
 */
template<typename REPR>
std::unique_ptr<StandardConstraintID<REPR>> makeNewStandardConstraintID(REPR data) {
    return std::make_unique<StandardConstraintID<REPR>>(std::forward<REPR>(data));
}

template<typename... TUP_ARGS>
std::unique_ptr<TupleConstraintID<TUP_ARGS...>> makeNewTupleConstraintID(TUP_ARGS&&... targs) {
    return std::make_unique<TupleConstraintID<TUP_ARGS...>>(std::forward<TUP_ARGS>(targs)...);
}

using TypeErasedConstraintEnum = int;

enum class ConstraintAddMode {
       LazyViaCallback,        //< Only add if adding by callback
       LazyToModel1,           //< Only add if adding to the model, and set Lazy=1 on the constraint
       LazyToModel2,           //< Only add if adding to the model, and set Lazy=2 on the constraint
       LazyToModel3,           //< Only add if adding to the model, and set Lazy=3 on the constraint
    NotLazyToModel,            //< Only add if adding to the model, does not set Lazy to any value
};

/**
 * A Configuration object for calls to ConstraintSet's addConstraints* functions.
 * Supports a separate addition mode per ConstraintGroup.
 */
struct ConstraintAddConf {
    /**
     * Records all entries in amodes_and_cgroups. Expects a container of pair like elements,
     * where the first element is an ConstraintAddMode, and the other is a iterable set of ConstraintGroup.
     * Will check for duplicate entries. Defaulted template parameters allow you to do something like this:
     * ConstraintAddConf cac ({
     *     { ConstraintAddMode::LazyToModel1, {ConstraintGroup::LinePathBalance, ConstraintGroup::CyclePathBalance} }
     * });
     */
    template<typename CONTAINER>
    ConstraintAddConf(const CONTAINER& amodes_and_cgroups) {
        for (auto& amode_and_cgroup : amodes_and_cgroups) {
            addAs(amode_and_cgroup.first, amode_and_cgroup.second);
        }
    }

    template<typename ConstraintEnum, typename CGROUPS = std::set<ConstraintEnum>, typename CONTAINER = std::vector<std::pair<ConstraintAddMode, CGROUPS>>>
    static ConstraintAddConf make(const CONTAINER& amodes_and_cgroups = {}) {
        return {amodes_and_cgroups};
    }

    /**
     * Configure all ConstraintGroup in cgroups to be added according to amode.
     * Throws if check_overwrite == true, and an overwrite of an existing entry is attempted.
     * Defaulted template parameters allow you to do something like this:
     * cac.addAs( ConstraintAddMode::LazyToModel1, {LinePathBalance, CyclePathBalance} );
     */
    template<typename CGROUPS>
    // template<typename ConstraintEnum, typename CGROUPS = std::set<ConstraintEnum>>
    void addAs(ConstraintAddMode amode, const CGROUPS& cgroups, bool check_overwrite = true) {
        for (auto& cg : cgroups) {
            auto emplace_result = add_modes.emplace((TypeErasedConstraintEnum)cg, amode);
            if (not emplace_result.second) {
                if (check_overwrite) {
                    throw std::logic_error("constraint already added");
                } else {
                    emplace_result.first->second = amode;
                }
            }
        }
    }

    bool willRequireLazyModelAttribute() {
        return std::find_if(begin(add_modes), end(add_modes), [](auto&& e) {
            return e.second == ConstraintAddMode::LazyViaCallback;
        }) != end(add_modes);
    }

    template<typename, typename>
    friend class ConstraintSet;
    std::map<TypeErasedConstraintEnum, ConstraintAddMode> add_modes = {};
};

using IDAndConstraint = std::pair<std::unique_ptr<ConstraintID>, GRBTempConstr>;

/**
 * Stores constraints that have been added so far, so they may be automatically added again.
 */
struct ConstraintCache {
    std::map<TypeErasedConstraintEnum, std::vector<IDAndConstraint>> added_constraints;
};

struct GenFuncReturn {
    std::vector<IDAndConstraint> constraints = {};
};

struct ConstraintGeneratorParams {
    MaybeGRBCallback callback;

    bool inIncrementalMode() { return callback; }
    auto makeReturnValue() { return GenFuncReturn(); }
};

using CGenFunc = std::function<GenFuncReturn(ConstraintGeneratorParams)>;

struct CGen {
    CGenFunc gen_func;
};

using ConstraintGenerators = std::vector<std::pair<TypeErasedConstraintEnum, CGen>>;

enum class ConstraintDest { ToModel, ViaCallback, };

[[nodiscard]]
ConstraintCache addConstraintsImpl(
    ConstraintCache&& cache,
    const ConstraintGenerators& constraint_generators, int verbosity,
    ConstraintDest destination, const ConstraintAddConf& add_conf, GRBCallbackPublic* cb, GRBModel* model
);

/**
 * A class for holding constraint generators, and controlling the addition
 * of constraints to a model, both initially and by callback.
 */
template<typename ConstraintEnum, typename SetOfConstraintEnums = std::set<ConstraintEnum>>
class ConstraintSet {
public:
    /**
     * Register a generator under the group cgroup. More than one generator can be registered
     * for the some ConstraintGroup, but the granularity this class in only at the ConstraintGroup
     * level; lazy settings, callback settings, and statistic printings are done per ConstraintGroup.
     */
    void registerGenerator(ConstraintEnum cgroup, CGenFunc constraint_generator) {
        constraint_generators.emplace_back((TypeErasedConstraintEnum)cgroup, CGen {
            std::move(constraint_generator),
        });
    }

    SetOfConstraintEnums getAllConstraintGroupsRegistered() const {
        SetOfConstraintEnums res;
        auto& cgens = constraint_generators;
        std::transform(begin(cgens), end(cgens), std::inserter(res, end(res)), [](auto&& e) { return (ConstraintEnum)e.first; });
        return res;
    }

    /**
     * Add constraints via the provided callback according to add_conf. Be sure to not discard
     * the return value, as it must be passed in again the next time an addConstraints* function
     * is called, so that constraints that were added in the past can be automatically re-added.
     * Intended to be used with std::move; similar to the following, with the rationale that
     * ConstraintSet can be made const in more situations.
     *
     * const ConstraintSet& cs = getConstraintSet();
     * ConstraintCache cc = cs.addConstraintsToModel(ConstraintSet::ConstraintCache{}, add_conf, model);
     * <...snip...>
     * cc = cs.addConstraintsViaCallback(std::move(cc), add_conf, cb);
     * <...snip...>
     * cc = cs.addConstraintsViaCallback(std::move(cc), add_conf, cb);
     */
    [[nodiscard]]
    ConstraintCache addConstraintsViaCallback(
        ConstraintCache&& cache,
        const ConstraintAddConf& add_conf, GRBCallbackPublic& cb
    ) const {
        return addConstraintsImpl(std::move(cache), constraint_generators, verbosity, ConstraintDest::ViaCallback, add_conf, &cb, nullptr);
    }

    /**
     * Add constraints to the provided model according to add_conf.
     * Be sure not to discard the return value; see addConstraintsViaCallback for details
     */
    [[nodiscard]]
    ConstraintCache addConstraintsToModel(
        ConstraintCache&& cache,
        const ConstraintAddConf& add_conf, GRBModel& model
    ) const {
        return addConstraintsImpl(std::move(cache), constraint_generators, verbosity, ConstraintDest::ToModel, add_conf, nullptr, &model);
    }

    /**
     * Print more detail. 0 for silence
     */
    void setVerbosity(int i) { verbosity = i; }

private:
    ConstraintGenerators constraint_generators = {};
    int verbosity = 0;
};

/**
 * Get the value that Gurobi's Lazy constraint attribute should have, given an AddMode.
 */
inline int gurobiLazyIntValueOf(ConstraintAddMode add_mode) {
    switch (add_mode) {
        case ConstraintAddMode::LazyViaCallback:     return 0;
        case ConstraintAddMode::LazyToModel1:        return 1;
        case ConstraintAddMode::LazyToModel2:        return 2;
        case ConstraintAddMode::LazyToModel3:        return 3;
        case ConstraintAddMode::NotLazyToModel:      return 0;
        default: throw std::out_of_range("unexpected ConstraintAddMode");
    }
}

/**
 * Bring GRBException into the std::exception world. Will either throw the given
 * std::exception_ptr, or throw an exception inheriting from std::nested_exception
 * and WRAPPER_EXCEPTION.
 *
 * Ex.
 * try {
 *     codeThatMightThrowAGRBException();
 * } catch (...) {
 *     throwExceptionButWrapIfGurobiException(std::current_exception());
 * }
 */
template<typename WRAPPER_EXCEPTION = cgrame_error>
[[noreturn]] void throwExceptionButWrapIfGurobiException(std::exception_ptr eptr) {
    try {
        std::rethrow_exception(eptr);
    } catch (const GRBException& grbex) {
        std::throw_with_nested(WRAPPER_EXCEPTION(
            "wrapped GRBException with code=" + std::to_string(grbex.getErrorCode())
                + " and message='" + grbex.getMessage() + '\''
        ));
    }
}

#endif /*   USE_GUROBI         */

const int GUROBI_MAX_STRING_LENGTH = 255;

/**
 * Take `s` and make sure its length is at most GUROBI_MAX_STRING_LENGTH.
 * Result can be assumed to be unique, and will be mostly a
 * character sub-sequence of `s`
 * Colons are not allowed in the .lp file spec, so they are replaced too.
 */
std::string makeNameSolverSafe(std::string s);

#endif /* __CONSTRAINT_SET_H__ */
