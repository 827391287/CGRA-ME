/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/Mapper.h>

#include <regex>

namespace {
    // avoid static initialization order problems
    const auto& all_mapper_name() { static std::string v = "AllMappers"; return v; }
    const auto& all_mapper_prefix() { static std::string v = all_mapper_name() + "."; return v; }
}

Mapper::Mapper(std::shared_ptr<CGRA> cgra, int timelimit)
    : cgra(cgra)
    , timelimit(timelimit)
{ }

Mapper::~Mapper()
{
}

MapperRegistry::MapperRegistry(std::vector<ImplicitTuple<std::string, ImplicitlyToString, std::string>> common_defaults)
    : m_mapper_makers()
{
    registerMapper(all_mapper_name(), nullptr, false, "Default arg storage", common_defaults, {});
}

std::unique_ptr<Mapper> MapperRegistry::createMapper(const std::string& mapper_id, std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args) const {
    const auto lookup = m_mapper_makers.find(mapper_id);
    if (lookup == m_mapper_makers.end()) {
        throw std::logic_error("No mapper registered with ID `" + mapper_id + "'");
    } else if (not lookup->second.maker) {
        throw std::logic_error("Mapper `" + mapper_id + "' has an invalid maker function");
    }
    const auto& maker_desc = m_mapper_makers.at(mapper_id);
    const auto& key_prefix = makeKeyPrefix(mapper_id);
    const ConfigStore final_inis = getMapperArgs(mapper_id, args);
    const auto key_is_unset = [&](const auto& kv) { return not final_inis.hasKey(key_prefix + kv.first); };
    const auto first_unset_default = std::find_if(maker_desc.registered_defaults.begin(), maker_desc.registered_defaults.end(), key_is_unset);
    if (first_unset_default != maker_desc.registered_defaults.end()) {
        throw std::logic_error("Required key `" + first_unset_default->first + "' missing when creating mapper `" + mapper_id + "'");
    }

    for (const auto& kv : final_inis) {
        if (maker_desc.is_composite) {
            if (kv.first.substr(0, key_prefix.size()) != key_prefix) { continue; } // skip checking keys not meant for this mapper
            if (kv.first.substr(key_prefix.size()).find('.') != kv.first.npos) { continue; } // skip checking nested keys
        }
        const auto key_no_prefix = std::string(std::next(kv.first.begin(), key_prefix.size()), kv.first.end());
        if (not maker_desc.hasMatchingArg(key_no_prefix)) {
            throw std::logic_error("Given key `" + key_no_prefix + "' does not match any of the allowed arguments for `" + mapper_id + "'");
        }
    }

    return maker_desc.maker(cgra, timelimit, final_inis);
}

void MapperRegistry::registerMapper(
        std::string mapper_id, MapperMaker maker, bool is_composite, std::string mapper_description,
        std::vector<ImplicitTuple<std::string, ImplicitlyToString, std::string>> required_arg_names_defaults_and_descriptions,
        std::vector<std::pair<std::string, std::string>> optional_arg_regexes_and_descriptions
) {
    const auto lookup = m_mapper_makers.find(mapper_id);
    if (lookup != m_mapper_makers.end()) {
        throw std::logic_error("There is already a mapper registered with ID `" + mapper_id + "' that has description:\n" + lookup->second.description);
    }

    MakerAndDescription desc{
        std::move(maker), is_composite, std::move(mapper_description), {} ,{}
    };

    for (auto& name_default_desc : required_arg_names_defaults_and_descriptions) {
        const auto& arg_name = std::get<0>(name_default_desc);
        desc.registered_defaults.addString(arg_name, std::move(std::get<1>(name_default_desc).impl));
        desc.allowed_arg_regexes_and_documentation.insert({arg_name, {std::regex(arg_name), std::move(std::get<2>(name_default_desc))}});
    }

    for (auto& regex_and_desc : optional_arg_regexes_and_descriptions) {
        auto& arg_regex = std::get<0>(regex_and_desc);
        desc.allowed_arg_regexes_and_documentation.insert({arg_regex, {std::regex(arg_regex), std::move(std::get<1>(regex_and_desc))}});
    }

    m_mapper_makers.emplace(std::move(mapper_id), std::move(desc));
}

ConfigStore MapperRegistry::getAllRegisteredArgDefaults() const {
    ConfigStore result;
    for (const auto& id_and_mapper : m_mapper_makers) {
        set_all(result, addPrefix(id_and_mapper.second.registered_defaults, id_and_mapper.first + '.'));
    }
    return result;
}

ConfigStore MapperRegistry::getMapperArgs(const std::string& mapper_id, const ConfigStore& args) const {
    const auto& maker_desc = m_mapper_makers.at(mapper_id);
    const ConfigStore requested_all_mapper_defaults = addPrefix(
        filterKeys(
            getEntriesForPrefix(args, all_mapper_prefix()),
            [&](const auto& k) { return maker_desc.hasMatchingArg(k); }
        ),
        makeKeyPrefix(mapper_id)
    );
    const ConfigStore mapper_specific_keys = getEntriesForPrefix(args, mapper_id + '.'); // keys for just this mapper

    if (maker_desc.is_composite) {
        // find nested keys -- keys that should apply to mappers called by this composite mapper
        ConfigStore nested_keys;
        for (const auto& kv : mapper_specific_keys) {
            if (kv.first.find('.') != kv.first.npos) { // has dot <=> is nested
                nested_keys.setString(kv.first, kv.second);
            }
        }
 
        // add the computed args to the given args -- this is how the mapper specific keys get passed down for composite mappers
        return with_set(requested_all_mapper_defaults, args, nested_keys);
    } else {
        // just pass the defaults overridden by the mapper specific keys -- non-composite mappers expect no prefixes
        return with_set(requested_all_mapper_defaults, mapper_specific_keys);
    }
}

void MapperRegistry::printMapperList(std::ostream& os) const {
    for (const auto& id_and_mapper : m_mapper_makers) {
        if (id_and_mapper.first == all_mapper_name()) { continue; }
        os << id_and_mapper.first << "\n\t";
        for (const auto a_char : id_and_mapper.second.description) {
            os << a_char;
            if (a_char == '\n') {
                os << '\t';
            }
        }
        os << '\n';
    }
}

void MapperRegistry::printDefaultsAsINI(std::ostream& os) const {
    const auto& all_mapper_maker = m_mapper_makers.at(all_mapper_name());
    const auto maybe_print_arg = [&,this](const auto& maker, const auto& kv, bool always_print) {
        const auto& arg_docs = maker.allowed_arg_regexes_and_documentation.at(kv.first).second;
        if (not arg_docs.empty()) {
            os << "# " << arg_docs.substr(0,arg_docs.find_first_of('\n')) << '\n';
        } else if (all_mapper_maker.registered_defaults.hasKey(kv.first) && all_mapper_maker.registered_defaults.getString(kv.first) == kv.second) {
            // skip if no docs and same value as a AllMappers default
            if (not always_print) {
                return;
            }
        }
        os << kv.first << " =";
        if (not kv.second.empty()) {
            os << " " << kv.second;
        }
        os << '\n';
    };

    os << "#\n";
    os << "# Gobal settings & defaults\n";
    os << "#\n";
    os << "\n";
    os << '[' << all_mapper_name() << "]\n";

    for (const auto& kv : all_mapper_maker.registered_defaults) {
        maybe_print_arg(all_mapper_maker, kv, true);
    }

    os << "\n";
    os << "#\n";
    os << "# The rest are mapper-specific & overrides\n";
    os << "#\n";

    for (const auto& name_and_maker : m_mapper_makers) {
        if (name_and_maker.first == all_mapper_name()) { continue; } // handled above
        const auto& maker = name_and_maker.second;
        os << "\n";
        os << '[' << name_and_maker.first << "]\n";
        for (const auto& kv : maker.registered_defaults) {
            maybe_print_arg(maker, kv, false);
        }
    }
}

void MapperRegistry::printHelpForMapper(const std::string& mapper_id, const ConfigStore& args, std::ostream& os) const {
    const char* indent = "  ";
    const auto configured_args = getEntriesForPrefix(getMapperArgs(mapper_id, args), makeKeyPrefix(mapper_id));
    const auto maker = m_mapper_makers.at(mapper_id);

    os << maker.description << "\n\nThe following argument and argument regular expressions are allowed:\n";
    for (const auto& name_and_regex_and_desc : maker.allowed_arg_regexes_and_documentation) {
        os << "\nArg `" << name_and_regex_and_desc.first << "': ";
        bool last_was_newline = false;
        for (const auto& c : name_and_regex_and_desc.second.second) {
            if (last_was_newline) os << indent;
            os << c;
            last_was_newline = c == '\n';
        }
        os << '\n';
        if (maker.registered_defaults.hasKey(name_and_regex_and_desc.first)) { // is it a required argument?
            os << indent << "configured value: `" << configured_args.getStringOr(name_and_regex_and_desc.first, "!UNSET!") << "'";
        } else {
            bool found_match = false;
            for (const auto& kv : configured_args) {
                if (regex_match(kv.first, name_and_regex_and_desc.second.first)) {
                    if (not std::exchange(found_match, true)) {
                        os << indent << "matching options passed:";
                    }
                    os << " " << kv.first << "=" << kv.second;
                }
            }
            if (not found_match) {
                os << indent << "(no matching options passed)";
            }
        }
        os << "\n";
    }
}

MapperRegistry& AutoRegisterMapper::getDefaultMutableRegisty() {
    static MapperRegistry default_registry({
        {"arch_id", "", "Possible hint to heuristics"},
        {"seed", 0, "Seed for all preudo-random operations in a mapper"},
        {"verbosity", 0, "Controls logging. A value of 0 requests silence."},
        {"max_threads", 1, "Maximum number of simultaneous threads that a mapper should try to use"},
        {"model_dump_filename", "", "Set to non-empty to enable dumping of ILP models"},
        {"model_IIS_dump_filename", "", "Set to non-empty to enable computation and dumping of ILP irreducable inconsistent subsystems"},
    });
    return default_registry;
}

AutoRegisterMapper::AutoRegisterMapper(
    std::string mapper_id, MapperMaker maker, bool is_composite, std::string mapper_description,
    std::vector<ImplicitTuple<std::string, ImplicitlyToString, std::string>> required_arg_names_defaults_and_descriptions,
    std::vector<std::pair<std::string, std::string>> optional_arg_regexes_and_descriptions
) {
    try {
        getDefaultMutableRegisty().registerMapper(
            mapper_id, std::move(maker), is_composite, std::move(mapper_description),
            required_arg_names_defaults_and_descriptions, optional_arg_regexes_and_descriptions
        );
    } catch (...) {
        // Print & continue without re-throwing
        // This way, just registering a bad mapper doesn't bring down the entire executable/dynamic library during static init.
        // If it's really needed, then trying to create the mapper will fail anyway.
        printExceptionToOutAndErr(
            std::current_exception(), "==>",
            ("Exception thrown while registering mapper `" + mapper_id + "'. Will continue, while ignoring this mapper").c_str()
        );
    }
}
