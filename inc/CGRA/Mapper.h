/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef ___MAPPER_H__
#define ___MAPPER_H__

#include <CGRA/CGRA.h>
#include <CGRA/ConfigStore.h>
#include <CGRA/Mapping.h>
#include <CGRA/OpGraph.h>
#include <CGRA/TupleHash.h>

#include <iosfwd>
#include <map>
#include <regex>
#include <vector>

/**
 * @brief Common interface for mappers.
 */
class Mapper
{
    public:
        virtual Mapping mapOpGraph(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string> fix_port) = 0;

        virtual ~Mapper();
    protected:
        Mapper(std::shared_ptr<CGRA> cgra, int timelimit);

        std::shared_ptr<CGRA> cgra; //< Architecture Object
        int timelimit;  //< The mapper timeout in seconds
};

/**
 * @brief Function type for registering mappers.
 */
using MapperMaker = std::function<std::unique_ptr<Mapper>(std::shared_ptr<CGRA>, int, const ConfigStore&)>;

/**
 * @brief Holds instances of mapper makers and creates mappers by ID, while resolving arguments.
 *
 * Can also generate an INI file and print help about the mappers it holds.
 *
 * Two types of mappers can be registered -- composite and non-composite.
 * Composite mappers are in general mappers that call other mappers.
 * They will always receive argument keys that are prefixed by the mapper that they are intended
 *   so it may pass the same arguments to the mappers it invokes,
 * Non-composite mappers will receive argument keys with all prefixes removed -- and only keys meant for that mapper.
 *
 * For example if a non-composite mapper named `nc` is created with arguments `nc.key1=1` and `comp.key2=2`,
 *   it will only receive a ConfigStore with key-value pair `key1=1`.
 * If a composite named `comp` is created with the same arguments, it will receive both unmodified.
 * Composite mappers can also have "nested keys", eg. `comp.nc.key1=3`, which will be extracted to the surrounding scope.
 * So, if that key is added to the previous example, `comp` will be called with `nc.key1=3`, `comp.key2=2'.
 *
 * There is also the common defaults ("AllMapper") mechanism for defining common arguments.
 * All keys that begin with the prefix `AllMapper`. will be passed to a mapper if the part after the prefix matches a required or optional argument.
 */
class MapperRegistry {
public:
    /**
     * @brief Initialize with some common defaults
     */
    MapperRegistry(std::vector<ImplicitTuple<std::string, ImplicitlyToString, std::string>> common_defaults);

    MapperRegistry(const MapperRegistry&) = default;
    MapperRegistry(MapperRegistry&&) = default;
    MapperRegistry& operator=(const MapperRegistry&) = default;
    MapperRegistry& operator=(MapperRegistry&&) = default;

    /**
     * @brief Given a @p mapper_id, create an instance of that mapper with the given arguments
     *
     * Checks all passed arguments against the required and optional regex arguments for @p mapper_id.
     * The actual values of the registered defaults are *not* taken into account -- all values must be passed via @p args.
     */
    std::unique_ptr<Mapper> createMapper(const std::string& mapper_id, std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args) const;

    /**
     * @brief Register a new mapper under the name @p mapper_id
     *
     * @param mapper_id Unique name for this mapper.
     * @param is_composite For mappers that may call other mappers. See class documentation for more information.
     * @param maker function object that makes the mapper.
     * @param mapper_description a short description, optionally followed by a newline and a longer description.
     * @param required_arg_names_defaults_and_descriptions (arg name, default value, short description\\nlong description)
     *      List of arguments that must be passed when creating this mapper. Only use [a-z_] for consistency and to avoid regex active characters.
     *      Default values are only used for generating a the default config. Short & Long descriptions are used for config comments help strings.
     * @param optional_arg_regexes_and_descriptions (arg regex, short description\\nlong description)
     *      Similar to the above, but no default values.
     */
    void registerMapper(
        std::string mapper_id, MapperMaker maker, bool is_composite, std::string mapper_description,
        std::vector<ImplicitTuple<std::string, ImplicitlyToString, std::string>> required_arg_names_defaults_and_descriptions,
        std::vector<std::pair<std::string, std::string>> optional_arg_regexes_and_descriptions
    );

    /**
     * @brief Retrieve all registered default values for required arguments as a ConfigStore
     *
     * Useful for writing tests, and for introspection
     */
    ConfigStore getAllRegisteredArgDefaults() const;

    /**
     * @brief Compute the arguments that would be passed when creating mapper @p mapper_id.
     *
     * No checking for required and optional arguments is performed.
     */
    ConfigStore getMapperArgs(const std::string& mapper_id, const ConfigStore& args) const;

    /**
     * @brief Print a message with every mapper's id and description
     */
    void printMapperList(std::ostream& os) const;

    /**
     * @brief Print an INI file with all registered defaults and some comments based on short descriptions
     */
    void printDefaultsAsINI(std::ostream& os) const;

    /**
     * @brief Print a detailed message about a mapper and the suggested @p args, including all required and optional arguments' long descriptions
     *
     * Does *not* take into account the registered default values.
     * Based on the passed in @p args, configured values an printed for required args, as well as args that match optional arg regexes.
     */
    void printHelpForMapper(const std::string& mapper_id, const ConfigStore& args, std::ostream& os) const;
private:
    const std::string makeKeyPrefix(const std::string& mapper_id) const {
        return m_mapper_makers.at(mapper_id).is_composite ? mapper_id + '.' : ""; // composite mappers expect all keys to be prefixed
    }

    struct MakerAndDescription {
        MapperMaker maker;
        bool is_composite;

        std::string description;
        std::map<std::string, std::pair<std::regex, std::string>> allowed_arg_regexes_and_documentation;
        ConfigStore registered_defaults;

        auto findMatchingArg(const std::string& arg) const {
            const auto matches_key = [&](const auto& name_and_regex_and_doc) { return regex_match(arg, name_and_regex_and_doc.second.first); };
            return std::find_if(allowed_arg_regexes_and_documentation.begin(), allowed_arg_regexes_and_documentation.end(), matches_key);
        }

        bool hasMatchingArg(const std::string& arg) const {
            return
                registered_defaults.hasKey(arg)
                or findMatchingArg(arg) != allowed_arg_regexes_and_documentation.end();
        }
    };

    std::map<std::string, MakerAndDescription> m_mapper_makers;
};

/**
 * @brief Special helper for registering mappers to the default mapper registry.
 *
 * Simply make a static (eg. file scope) variable of this type initialized with your ID, description, maker callback, and arg specs.
 * The mapper will then be available to choose from at the command-line.
 * This still works if dynamic linking of shared objects is used.
 * Be careful about static initialization order! (only use variables defined in the same file, and above the current line)
 * At the time of writing, the default registry has the
 *   `seed`, `max_threads`, `verbosity` `model_dump_filename` and `model_IIS_dump_filename`
 *   common default keys.
 *
 * Ex.:
 * @code{.cpp}
 * AutoRegisterMapper my_arm("MyMapperName",
 *     [](const ConfigStore& args) {
 *         return std::make_unique<MyMapperClass>(args);
 *     },
 *     false, // not composite
 *     "my mapper short description\n"
 *     "my mapper long description is after the newline"
 *     { // required args. Should only use [_a-z] for consistency and avoiding regex active characters
 *         {"max_threads", 1, ""}, // standard option
 *         {"seed", 0, ""}, // standard option
 *         {"verbosity", 0, ""}, // standard option
 *         {"required_arg_1", false, "A short description"},
 *         {"another_required_arg", "default", "Another short description"},
 *         {"req_arg_3", "", "short desc\n"
 *             "long desc after the newline"},
 *     },
 *     { // optional arg regexes
 *         {"my_prefix_.*", "hidden options"},
 *         {"do_[a-z]+_faster", "do something faster"},
 *     }
 * );
 * @endcode
 */
class AutoRegisterMapper {
public:
    /**
     * @brief Register a mapper with the default registry. See MapperRegistry::registerMapper for documentation
     */
    AutoRegisterMapper(
        std::string mapper_id, MapperMaker maker, bool is_composite, std::string mapper_description,
        std::vector<ImplicitTuple<std::string, ImplicitlyToString, std::string>> required_arg_names_defaults_and_descriptions,
        std::vector<std::pair<std::string, std::string>> optional_arg_regexes_and_descriptions
    );

    /**
     * @brief Public read-only access to the mapper registry that this class automatically adds mappers to
     */
    static const MapperRegistry& getDefaultRegistry() { return getDefaultMutableRegisty(); }
private:
    static MapperRegistry& getDefaultMutableRegisty();
};

#endif

