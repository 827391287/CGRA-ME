/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/ConstraintSet.h>

ConstraintCache addConstraintsImpl(
    ConstraintCache&& cache,
    const ConstraintGenerators& constraint_generators, int verbosity,
    ConstraintDest destination, const ConstraintAddConf& add_conf, GRBCallbackPublic* cb, GRBModel* model
) {
    const bool in_callback_mode = destination == ConstraintDest::ViaCallback;

    MaybeGRBCallback maybe_callback(in_callback_mode ? cb : nullptr);

    if (in_callback_mode) {
        maybe_callback.get(); // will throw if we don't have the callback
    } else {
        if (not model) {
            throw std::logic_error("need a model for non-lazy mode");
        }
    }

    struct Counts {
        int re_adds = 0;
        int novel = 0;
        int re_add_avoids = 0;

        bool isInteresting() const { return re_adds + novel + re_add_avoids != 0; }
        int totalAdded() const { return re_adds + novel; }
    };

    std::map<TypeErasedConstraintEnum, Counts> constraint_numbers;

    for (auto& group_and_gen : constraint_generators) {
        const auto& group = group_and_gen.first;

        // skip if not in configuration
        if (add_conf.add_modes.find(group) == end(add_conf.add_modes)) {
            continue;
        }

        const auto& add_mode = add_conf.add_modes.at(group);
        const bool is_callback_mode_group = add_mode == ConstraintAddMode::LazyViaCallback;
        const bool add_via_lazy_callback = in_callback_mode && is_callback_mode_group;
        const bool add_to_model_non_lazy = not in_callback_mode && ( add_mode == ConstraintAddMode::NotLazyToModel );
        const bool add_to_model_lazy = not in_callback_mode && ( add_mode == ConstraintAddMode::LazyToModel1 || add_mode == ConstraintAddMode::LazyToModel2 || add_mode == ConstraintAddMode::LazyToModel3 );
        const bool adding_lazy = add_via_lazy_callback || add_to_model_lazy;

        // skip if not adding
        if (
               not add_via_lazy_callback
            && not add_to_model_non_lazy
            && not add_to_model_lazy
        ) {
            continue;
        }

        auto& added_constraints = cache.added_constraints[group];
        auto& constr_nums = constraint_numbers[group];
        for (auto& id_and_constraint : added_constraints) {
            if (add_via_lazy_callback) {
                cb->addLazy(id_and_constraint.second);
                ++constr_nums.re_adds;
            }
        }

        auto gen_result = group_and_gen.second.gen_func(ConstraintGeneratorParams{ maybe_callback });
        for (auto& id_and_constraint : gen_result.constraints) {
            auto& c = id_and_constraint.second;
            if (add_via_lazy_callback) {
                if (std::find_if(begin(added_constraints), end(added_constraints), [&](auto&& test) { return *test.first == *id_and_constraint.first; }) == end(added_constraints)) {
                    cb->addLazy(c);
                    added_constraints.emplace_back(std::move(id_and_constraint));
                    ++constr_nums.novel;
                } else {
                    ++constr_nums.re_add_avoids;
                }
            }
            if (add_to_model_non_lazy || add_to_model_lazy) {
                std::stringstream name_ss;
                name_ss << group << '_' << constr_nums.novel;
                const auto& the_constraint = model->addConstr(c, name_ss.str());

                // maybe set the constraint as lazy, accornding to the enum
                if (add_to_model_lazy) {
                    const auto the_value = gurobiLazyIntValueOf(add_mode);
                    model->set(GRB_IntAttr_Lazy, &the_constraint, &the_value, 1); // the 1 is the "length" of the "arrays"
                }

                ++constr_nums.novel;
            }
        }

        const char* destination_string = in_callback_mode ? "via callback" : "to model";
        const char* lazy_string = adding_lazy ? " lazy" : "";
        const auto lazy_int_string = add_to_model_lazy ? (" L=" + std::to_string(gurobiLazyIntValueOf(add_mode))) : std::string();

        if (constr_nums.isInteresting() && verbosity > 0) {
            std::cout
                << "added this many" << lazy_string << lazy_int_string << " constraints " << destination_string << " for " << group
                << ": (re-add, novel, re-add-avoid) : " << constr_nums.re_adds << ", " << constr_nums.novel << ", " << constr_nums.re_add_avoids << '\n';
        }
    }

    if (verbosity > 0) {
        std::cout << "tried adding constraints from " << constraint_numbers.size() << " groups, with no constraints from";
        std::vector<TypeErasedConstraintEnum> groups_with_no_constraints_added;
        for (const auto& group_and_nums : constraint_numbers) {
            if (group_and_nums.second.totalAdded() != 0) { continue; }
            groups_with_no_constraints_added.push_back(group_and_nums.first);
        }
        print_container(std::cout, groups_with_no_constraints_added);
        std::cout << std::endl;
    }

    return std::move(cache);
}

std::string makeNameSolverSafe(std::string s) {
    if ((std::ptrdiff_t)s.size() <= GUROBI_MAX_STRING_LENGTH) { return s; }

    // if too long, take the first MAX-HASH_CHARS chars and append the hash
    using S = std::remove_cv_t<std::remove_reference_t<decltype(s)>>;
    const auto hash = std::hash<S>{}(s);
    using H = std::remove_cv_t<std::remove_reference_t<decltype(hash)>>;
    using HNL = std::numeric_limits<H>;
    const int print_radix = 16;
    const int HASH_CHARS = HNL::digits / print_radix * HNL::radix;
    s.resize(GUROBI_MAX_STRING_LENGTH);
    const char hex_digits[] = "0123456789abcdef";
    for (auto i = 0; i < HASH_CHARS; ++i) {
        s[GUROBI_MAX_STRING_LENGTH - HASH_CHARS + i] = hex_digits[(hash >> i) & 0x0f];
    }

    // LP file format doesn't like colons
    for (auto& c : s) if (c == ':') c = '-';

    return s;
}
