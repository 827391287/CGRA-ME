/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __MODULE__H__
#define __MODULE__H__

#include <CGRA/BitSetting.h>
#include <CGRA/Exception.h>
#include <CGRA/MRRG.h>
#include <CGRA/Mapping.h>
#include <CGRA/OpGraph.h>

#include <coreir/ir/fwd_declare.h>

#include <unordered_map>
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iosfwd>

// template<typename T>
// auto MOD_II(const T& x) { return x % II; }
#define MOD_II(x) ((x) % II)

// Data types
using StringMatrix = std::vector<std::vector<std::string>>;
// For indenting the code
const char* const SET_INDENT = "    ";
const char* const SET_DOUBLE_INDENT = "        ";
const char* const SET_TRIPLE_INDENT = "            ";
const char* const SET_QUAD_INDENT = "                ";
const char* const SET_PENTA_INDENT = "                    ";
const char* const SET_HEXA_INDENT = "                        ";
// If port is not connected, this is the default print message
const char* const PORT_DEFAULT_CONNECTION = "/* A PORT IS UNCONNECTED */";
// Determine if we want to print out debug messages
const bool MODULE_DEBUG_PRINTING = false;
// DEFAULT DATA SIZE FOR MODULES, can change here
const int DEFAULT_SIZE = 32;
// A port size of zero in memory indicates that it is parameterized. The actual port size will then be determined by the module size
const int PARAMETERIZED = 0;

class Module;
class PrintList;
class ConfigCell;
class ContextCell;
class ContextCounter;


// Different types of ports (input, output, output reg, inout, etc) 
// Furthermore config ports that could be handled differently for elastic and static cases
typedef enum
{
    PORT_INPUT,
    PORT_OUTPUT,
    PORT_OUTPUT_REG,
    PORT_BIDIR,
	PORT_CONFIG,
    PORT_UNSPECIFIED,
} port_type;

std::istream& operator >>(std::istream &is, port_type &porttype);

// Different types of CGRA blocks
typedef enum
{
    STANDARD_NOBYPASS, // CGRA block, NSEW connection, no bypass mux enabled
    STANDARD_BYPASS, // CGRA block, NSEW connection, bypass mux exists
    STANDARD_DIAGONAL, // CGRA blocks that have diagonal as well as NSEW Connections
} CGRABlockType;

std::istream& operator >>(std::istream &is, CGRABlockType &blocktype);

// Port object: contains all information for a given port
struct Port
{
    const std::string& getName() const { return name; }
    Module& getModule() const { return *parent; }

    std::string makeVerilogDeclaration() const;

    std::string name = {}; // Name of the port
    port_type pt = PORT_UNSPECIFIED; // The type of port
    std::string parameter = {}; // If the port is parameterized, this is the name of the parameter
    unsigned size = -1; // Size of the port
    Module* parent = nullptr; // The module which the port belongs to (its parent)
};

// Connection object: contains all information describing connections between a source port and multiple destination ports
struct Connection
{
    std::string name = {}; // Name of the connection object
    Port* src = nullptr; // Source port
    std::vector<Port*> dst = {}; // Vector of destination port(s)
    bool isInMRRG = true; // Flag specifying whether ot not the nodes that this connects are in the MRRG
};

// PrintList class: it's a buffer that is used for printing module contents (WireList, SubmodList, and AssignList are of type PrintList)
// This is used because we don't want to print its output immediately after its generated
class PrintList
{
    public:
        void add(std::vector<std::string> Data)
        {
            // Add information into the buffer, taking a vector of strings as input
            std::string ToAdd = "";
            for (unsigned i = 0; i < Data.size(); i++)
                ToAdd.append(Data[i]);
            ToPrint.push_back(ToAdd); // Adding the information into the buffer
        };
        void print() // Printing the buffer
        {
            for (unsigned i = 0; i < ToPrint.size(); i++)
                std::cout << ToPrint[i];
        };
        bool empty() // Check if the buffer is empty
        {
            return ToPrint.empty();
        }
        void pop() // Removes the last thing added into the buffer
        {
            ToPrint.pop_back();
        }
        std::string last()
        {
            return ToPrint[ToPrint.size() - 1];
        }
    private:
        std::vector<std::string> ToPrint = {}; // Vector of strings that stores information in the buffer
};

struct ResolvedVeroligModuleParameter {
    std::string name;
    std::string value;
};

using MRRGNodesFromOpNode  = std::map<OpGraphOp*,  std::set<MRRG::NodeDescriptor>>;
using MRRGNodesFromValNode = std::map<OpGraphVal*, std::set<MRRG::NodeDescriptor>>;

// Modes of Verilog Generation
enum class VerilogType
{
    CGRAME = 0,
    CoreIR = 1
};

struct Location {
    unsigned x_coord;
    unsigned y_coord;
    Location(unsigned x, unsigned y) : x_coord(x), y_coord(y) {}
    Location(int x, int y) : Location(static_cast<unsigned>(x), static_cast<unsigned>(y)) {}
    Location(unsigned x, int y) : Location(x, static_cast<unsigned>(y)) {}
    Location(int x, unsigned y) : Location(static_cast<unsigned>(x), y) {}
};

std::ostream& operator<<(std::ostream& os, VerilogType vt);

class Module
{
    public:
        Module(std::string name, Location, unsigned size = DEFAULT_SIZE,  bool isElastic = false);
        Module(std::string name, std::string template_name, Location loc = {0,0}, unsigned size = DEFAULT_SIZE,  bool isElastic = false);
        virtual ~Module();

        Module(const Module&) = delete;
        Module(Module&&) = default;
        Module& operator=(const Module&) = delete;
        Module& operator=(Module&&) = default;

        // Other functions for debugging purposes
        void print();
        void print_dot();
        void print_ports();
        void print_connections();
        void print_submodules();
        void print_configcells();

        // Functions to modify a module
        void addConfig(ConfigCell* c, std::vector<std::string> ConnectTo); // add a config cell (configcell, {ports to connect to})
        void addConfig(std::string name, std::vector<std::string> ConnectTo, int contexts, bool isElastic); // add a config cell (configcell, {ports to connect to})
        void addSubModule(Module* m); // add a submodule (with pointer to module we are adding)
        void addSubModule(Module* m, double xPos, double yPos, double width, double height); // add a submodule (with pointer to module we are adding)

        // add connection ("submodulename.portname", "submodulename.portname") Also assumes that port widths are the same size, returns true if sucessful
        void connectPorts(std::string src, std::string dst, bool isElastic);
        // add connection ("submodulename.portname", "submodulename.portname") Also assumes that port widths are the same size, returns true if sucessful
        void addConnection(std::string src, std::string dst, bool isInMRRG = true);
        // add  elastic connection ("submodulename.portname", "submodulename.portname") Also assumes that port widths are the same size, returns true if sucessful
        void addElasticConnection(std::string src, std::string dst);
        // adds a port ("portname", port type, size);
        void addPort(std::string portname, port_type pt, unsigned size);
        void addPort(std::string portname, port_type pt, unsigned size, bool isElastic);
	    // adds an elastic port ("portname", port type, size)
	    void addElasticPort(std::string portname, port_type pt, unsigned size);
        // adds a parameterized port ("portname", port type, parameter name, parameter size), parameter size is used to specify default size for a parameter
        void addPort(std::string portname, port_type pt, std::string ParameterName, unsigned size);
        // adds a parameterized port ("portname", port type, parameter name, parameter size), parameter size is used to specify default size for a parameter
        void addPort(std::string portname, port_type pt, std::string ParameterName, unsigned size, bool isElastic);
        // adds a parameterized port ("portname", port type, parameter name, parameter size), parameter size is used to specify default size for a parameter
        void addElasticPort(std::string portname, port_type pt, std::string ParameterName, unsigned size);
        void addParameter(std::string parameterName, unsigned parameterValue);
        // Adds a verilog only port to the module. The port is not part of the mrrg but will be propagated to the top level cgra.
        void addVerilogPort(std::string name, port_type pt, std::string parameter, unsigned size);

        // Function that returns ALL the modules that have config cells attached to them, and their order
        void genConfigOrder(std::vector<ConfigCell*> & ConfigTable) const;

        Module* getSubModule(std::string);
        bool isSubModule(Module*);
		Module* getModule(std::string, std::string err_context = "");
		Module* getModuleFromPortName(std::string full_port_name, std::string err_context = "");
		Port* getPort(std::string full_port_name, std::string err_context = "");
 
        // VARIABLES
        // internal parameters, ports, connections and modules, and configuration cells
        // submodules variable MUST be public, as used in the CGRA.cpp file

        // RE. parameterlist: it may be wanted to add an state meaning that the parent's value should be used
        std::map<std::string, unsigned> parameterlist;
        std::map<std::string, Port*>    ports;
        std::map<Port*, Connection*>    connections;
        std::map<std::string, Module*>  submodules;
        std::map<std::string, ConfigCell*>  configcells; // configcellname -> configcell
        std::vector<Port> portsToPropagate; // Ports to propagate to the top level of the cgra, e.g. memory

        // Variables exclusively set and accessed by PerfEngine:
        // - area, and helper flags used to print hierarchical area
        double area = 0.;
        unsigned int hierarchyLevel = 0; // increments from top-level(root) to base-modules(bottom children)
        bool isLastInHierarchy; // indicate this being the last among submodules in the same level
        bool isElastic = false;
        bool submodsSet; // whether the submodules' area/hierarchyLevel/isLastInHierarchy were set
        // x & y coord assigned to each module to specify the relative position in 2D plannar
        Location loc;
        // FIXME: very hacky... bandaid
        int overridenFanoutCount = -1; // used for overriden synthesized fanout count; -1 indicates using MRRG-based count

        Module* parent = nullptr; // Stores parent module

        // Accessor functions
        int getSize() const { return data_size; }
        auto& getName() const { return name; }
        std::string ReturnPath() const // Returns the full path from the CGRA to a module (useful for CAD tools to simulate CGRA)
        {
            if (parent->name == "CGRA")
                return name;
            return parent->ReturnPath() + "/" + name;
        }

        bool hasConfigCells() const { return not configcells.empty(); }

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& /*map*/,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const {
            std::cout << getName() << '\n';
            for (const auto& op_and_nodes : mrrg_nodes_from_op_node) {
                std::cout << '\t' << *op_and_nodes.first << '\n';
                for (const auto& mrrg_node : op_and_nodes.second) {
                    std::cout << "\t\t" << mrrg_node << '\n';
                }
            }
            for (const auto& val_and_nodes : mrrg_nodes_from_val_node) {
                std::cout << '\t' << *val_and_nodes.first << '\n';
                for (const auto& mrrg_node : val_and_nodes.second) {
                    std::cout << "\t\t" << mrrg_node << '\n';
                }
            }
            (void)ccell;
            (void)mrrg_nodes_from_val_node;
            (void)og;
            (void)mrrg;
            throw cgrame_error("getBitConfig: uninplemented for " + getName());
        }
        virtual std::string GenericName(); // Generates a generic name for the module, can be overridden by inherited virtual functions.

        // Recursively create the MRRG for a given number of contexts
        virtual MRRG* createMRRG(unsigned contexts);

        // implementations for verilog generation
        void genVerilogCGRAME(std::string dir);
        void genVerilogCoreIR(std::string dir, int contexts);

        // Direct helper functions for GenVerilog()
        void GetModulesToPrint(std::queue<Module*> & ToPrint, std::set<std::string> & PrintedModMap); // Function that returns what modules need to be printed in Verilog (avoiding duplicate printing)
        virtual void GenModuleVerilog(); // Function that generates verilog for a module (submodules not included)
        CoreIR::TypeGenFun makeCoreIRInterfaceGenerator();
        CoreIR::ModuleDefGenFun makeCoreIRModuleDefinitonGenerator();
        virtual void CoreIRGenModuleVerilog(CoreIR::Context* c, int contexts); // Function that generates verilog for a module using CoreIR (submodules not included)

        // Visualization
        bool setNodePosition(const std::string & nodeName, double x, double y);
        std::pair<bool, std::pair<double, double>> getSubModulePosition(const std::string & submodName);
        std::pair<bool, std::pair<double, double>> getNodePosition(const std::string & nodeName);
        // END OF HELPER FUNCTIONS

    protected:
        void GetConfigsToPrint(std::queue<ConfigCell*>& q, std::set<unsigned>& uniq);

        // Helper functions used in function GenModuleVerilog
        void GenModuleHeader(bool HasConfig, bool HasRegisters); // Generates module header
        void GenParameters(); // Prints out module parameters
        void GenPortSpecs(bool HasConfig, bool HasRegisters); // Prints out the ports, and their specification
        virtual void GenConnections(); // Prints out submodules and their connectivity, can be overriden (by FuncUnit)
        virtual void GenFunctionality(); // Generates the functionality for the module, e.g. always block, can be overriden by inherited virtual functions
        // END OF HELPER FUNCTIONS

        // Helper functions used in function CoreIRGenModuleVerilog
        void CoreIRGetModulesToPrint(std::queue<Module*> & ToPrint, std::set<std::string> & PrintedModMap); // Function that returns what modules need to be printed in Verilog (avoiding duplicate printing)
        virtual nlohmann::json CoreIRGenFunctionality(); // CoreIR Implementation of GenFunctionality()
        // END OF HELPER FUNCTIONS

        // Helper functions used in function GenConnections
        void GenerateMatrix(StringMatrix & Matrix); // Generates a matrix to store connections between submodules
        void DetermineConnections(StringMatrix & Matrix, PrintList & WireList, PrintList & SubmodList, PrintList & AssignList); // Print out the connections between submodules in Verilog
        // END OF HELPER FUNCTIONS

        /**
         * Function to override parameters within a module during Verilog declaration.
         * returns list of key-value pairs that will be part of the module instantiation.
         * Provided implementation just scrapes this->parameterlist.
         */
        virtual std::vector<ResolvedVeroligModuleParameter> ResolveVerilogParameters() const;

        // Helper functions used in DetermineConnections, used to help insert connection information into a matrix
        int FindPortIndex(std::string PortName);
        int FindSubmoduleIndex(std::string SubmoduleName);
        // END OF HELPER FUNCTIONS

        unsigned data_size; // Bitwidth of the data the module is handling

        std::string templateName; // Module name
        std::string name; // Module name

        // Visualization
        struct VisualPositionRect {
            /** Values are fractional, based on the parent's box
             * e.g. x= 0.5, y = 0.5, w = 0.5. h = 0.25 would create a box with
             * top left corner at the center of the parent, that extends from
             * center to the right border of the parent (0.5 width), and a
             * quarter of the parent's height down.
             */
            double x, y, w, h;
        };

        struct VisualPositionPoint {
            double x, y;
        };

        // Most positions are set in constructor
        std::map<std::string, VisualPositionRect> submodule_relative_position = {};
        std::map<std::string, VisualPositionPoint> node_relative_position = {};
    public:
        /**
         * Does *this* module add the requirement for clocks, regardless of submodules?
         * That is, to know if this module requires clock ports, a recursive search must be done.
         * See moduleRequiresClockPorts in ModuleProcedures.h
         */
        bool addsSynchronousCircuitry() const { return adds_synchronous_circuitry; }

    protected:
        bool adds_synchronous_circuitry; //< Does *this* module add the requirement for clocks? (regardless of submodules)
};

struct PipelineMode {
    int II;
    int latency;

    PipelineMode(int II, int latency)
        : II(II)
        , latency(latency)
    { }
    PipelineMode(const PipelineMode&) = default;
    PipelineMode(PipelineMode&&) = default;
    PipelineMode& operator=(const PipelineMode&) = default;
    PipelineMode& operator=(PipelineMode&&) = default;
};

// Struct to hold information of one possible mode of a FuncUnit (e.g. add)
typedef struct
{
    std::string ModuleName; // Name of the module to achieve that function
    std::string OpName; // Name of the operation
    std::vector<std::string> Functionality; // Verilog code necessary to achieve that function
    std::string WireName; // Name of the wire we will use when connecting that module to others
} LLVMMode;


/**
 * @brief Functional Unit, does one of a set of arithmetic computations.
 *
 * Ports:
 *  - 'in_a' is an input
 *  - 'in_b' is an input
 *  - 'out' is an output
 *  - 'select' is the configuration port. Only present if more than one @p supported_modes .
 */
class FuncUnit : public Module
{
    public:

        // Constructor takes in a name, the operations that the unit supports, and the size of the module
        FuncUnit(std::string name, Location, std::vector<OpGraphOpCode> supported_modes, unsigned size, int II, int latency, bool isElastic = false);
        virtual ~FuncUnit();

        //virtual void GenFunctionality() override;
        //virtual void GenConnections() override;
        virtual MRRG* createMRRG(unsigned II) override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;

        virtual std::string GenericName() override; // Generates a generic name for the module

        int getII() const { return pipeline_mode.II; }
        int getLatency() const { return pipeline_mode.latency; }
        auto& getSupportedModes() const { return supported_modes; }

        static const std::map<OpGraphOpCode, LLVMMode> all_modes;
    protected:
        PipelineMode pipeline_mode;
        std::vector<OpGraphOpCode> supported_modes;
};

class FracUnit : public Module
{
    public:
        FracUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes, unsigned size, int II, int latency);
        virtual ~FracUnit();

        virtual std::string GenericName() override; // Generates a generic name for the module

        int getII() const { return pipeline_mode.II; }
        int getLatency() const { return pipeline_mode.latency; }
        auto& getSupportedModes() const { return supported_modes; }

    protected:
        PipelineMode pipeline_mode;
        std::vector<OpGraphOpCode> supported_modes;
};

class FracMulUnit : public Module {
    public:
        FracMulUnit(std::string name, Location loc, unsigned size, int II, int latency);
        virtual ~FracMulUnit();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual nlohmann::json CoreIRGenFunctionality() override;

        virtual MRRG* createMRRG(unsigned II) override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;

        int getII() const { return pipeline_mode.II; }
        int getLatency() const { return pipeline_mode.latency; }

        static const std::map<OpGraphOpCode, LLVMMode> all_modes;
        
    protected:
        PipelineMode pipeline_mode;
        static const std::vector<OpGraphOpCode> mul_modes_ordered;
};

class FracAddUnit : public Module {
    public:
        FracAddUnit(std::string name, Location loc, unsigned size, int II, int latency);
        virtual ~FracAddUnit();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual nlohmann::json CoreIRGenFunctionality() override;

        virtual MRRG* createMRRG(unsigned II) override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;

        int getII() const { return pipeline_mode.II; }
        int getLatency() const { return pipeline_mode.latency; }

        static const std::map<OpGraphOpCode, LLVMMode> all_modes;
        
    protected:
        PipelineMode pipeline_mode;
        static const std::vector<OpGraphOpCode> add_modes_ordered;
};

/**
 * @brief Single load & store memory operations.
 *
 * Ports:
 *  - 'addr' is the address to read or write to
 *  - 'data_in' is the input data
 *  - 'data_out' is the output data
 */
class MemoryUnit : public Module {
    public:
        // Constructor takes in a name, the operations that the unit supports, and the size of the module
        MemoryUnit(std::string name, Location,  unsigned size = DEFAULT_SIZE, bool isElastic = false, bool pred = false);
        //        virtual void GenConnections();
        virtual ~MemoryUnit();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
        MRRG* createMRRG(unsigned II) override;

        // For the bitstream generation. Maps an operation to a set of bits
        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
    private: 
        bool pred;
};

class ConstUnit : public Module {
    public:
        // Constructor takes in a name, the operations that the unit supports, and the size of the module
        ConstUnit(std::string name, Location, int size = DEFAULT_SIZE, int contexts = 1,  bool isElastic = false);
        virtual ~ConstUnit();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual void GenFunctionality() override;
        MRRG* createMRRG(unsigned II) override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
};

/**
 * @brief A simple latency element with an enable signal; a data flip-flop.
 *
 * Ports:
 *  - 'in' is the input data
 *  - 'out' is the output data
 *  - 'enable' update 'out' with the value of 'in' next cycle.
 */
class Register : public Module {
    public:
        Register(std::string, Location, int size = DEFAULT_SIZE, bool isElastic = false); // ("name", size)
        virtual ~Register();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
        MRRG* createMRRG(unsigned II) override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
};

/**
 * @brief Zero-cycle latency multiplexer.
 *
 * Ports:
 *  - 'inN' where N is replaced by the number of the input port you are refering to (counting starts at 0)
 *  - 'out' is the output of the mux
 *  - 'select' decides which input to take
 */
class Multiplexer : public Module {
    public:
        Multiplexer(std::string, Location, unsigned mux_size, unsigned size = DEFAULT_SIZE, bool isElastic = false); // ("name", mux size, size)
        virtual ~Multiplexer();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
        int getMuxSize() { return mux_size; };
        MRRG* createMRRG(unsigned II) override;
    private:
        int mux_size; // size of multiplexer (e.g. how many inputs)
};

/**
 * @brief Zero-cycle latency split input.
 *
 * Ports:
 *  - 'input' the input that will be split
 *  - 'out0' the output from position to size
 *  - 'out1' the output from size to position
 */
class TruncateInput : public Module {
    public:
        TruncateInput(std::string, Location, unsigned pos, unsigned size = DEFAULT_SIZE, bool isElastic = false); // ("name", mux size, size)
        virtual ~TruncateInput();

	    //nlohmann::json genFunctionalityHeader();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual nlohmann::json CoreIRGenFunctionality() override;

        MRRG* createMRRG(unsigned II) override;
    private:
        int pos; // size of multiplexer (e.g. how many inputs)
};


/**
 * @brief 2 Zero-cycle latency multiplexers for predication support.
 *
 * Ports:
 *  - 'inN' where N is replaced by the number of the input port you are refering to (counting starts at 0)
 *  - Taking the most significant bit of inN to make a small 1b predication mux 
 *  - 'out' is the output of the mux
 *  - 'select' chooses input 
 *  - 'predsel' chooses output
 */

class SelMultiplexer : public Module {
    public:
        static const std::map<OpGraphOpCode,LLVMMode> all_modes;
        SelMultiplexer(std::string, Location, unsigned mux_size, unsigned size = DEFAULT_SIZE, bool isElastic = false); // ("name", mux size, size)
        virtual ~SelMultiplexer();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual nlohmann::json CoreIRGenFunctionality() override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
        int getMuxSize() { return mux_size; };
        MRRG* createMRRG(unsigned II) override;
    private:
        int mux_size; // size of multiplexer (e.g. how many inputs)
};
/**
 * @brief Zero-cycle latency DeMultiplexer.
 *
 * Ports:
 * - 'in' is the input of the mux
 * - 'outN' where N is replaced by the numer of the output port you are refering to (counting starts at 0)
 * - 'select' decides which output to take
 */	

class DeMux : public Module {
    public:
        DeMux(std::string, Location, unsigned demux_size, unsigned size = DEFAULT_SIZE, bool isElastic = false); // ("name", mux size, size)
        virtual ~DeMux();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual nlohmann::json CoreIRGenFunctionality() override;

        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
        int getDeMuxSize() { return demux_size; };
        MRRG* createMRRG(unsigned II) override;
    private:
        int demux_size; // size of multiplexer (e.g. how many inputs)
};


/**
 * @brief Multiplexed registers.
 *
 * Ports
 *  - 'inN' refers to the Nth input port
 *  - 'address_inN' refers to the Nth input address
 *  - 'WEN' refers to the Nth write enable port
 *  - 'outN' refers to the Nth output port
 *  - 'address_outN' refers to the Nth port for output address
 */
class RegisterFile : public Module {
    public:
        RegisterFile(std::string name, Location, int numInputPorts, int numOutputPorts, int log2Registers, int size = DEFAULT_SIZE, int contexts = 1, bool isElastic = false);
        virtual ~RegisterFile();
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;

        virtual std::string GenericName() override;
        // MRRG* createMRRG(unsigned II) override;
    private:
        int numInputPorts, numOutputPorts, log2Registers;
};

class Crossbar: public Module
{
    public:
        Crossbar(std::string name, Location, int num_inputs, int num_outputs, int data_size, bool predExist = false, int contexts = 1);
        virtual std::string GenericName() override;
    private:
        int num_inputs;
        int num_outputs;
};

class TriState : public Module {
    public:
        enum class Mode : char {
            PROVIDES_IO_OP,
            PROVIDES_IO_PRED_OP,
            PLAIN
        };

        TriState(Mode mode, std::string, Location, unsigned size = DEFAULT_SIZE, bool isElastic = false);
        virtual std::string GenericName() override;
        virtual ~TriState();
        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;
        virtual MRRG* createMRRG(unsigned II) override;
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
    private:
        Mode mode;
};

/**
 * @brief Supports the input and output ops. Will add an external port at the top-level of generated verilog.
 *
 * Ports:
 *  - 'in' is the input port
 *  - 'out' is the output port
 */
class IO : public Module {
    public:
        IO(std::string, Location, unsigned size = DEFAULT_SIZE, int contexts = 1, bool isElastic = false); // ("name", size)
        Module* getTriState(); // This is used for the bitstream generation
        virtual std::string GenericName() override;
        virtual ~IO();
};

/** @brief For when an input is unused, but a connection is required anyway. */
class DisconnectedSource : public Module {
    public:
        DisconnectedSource(std::string name, Location, unsigned size, int value = 0);
        virtual std::string GenericName() override;
        virtual ~DisconnectedSource();
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
    private:
        int value;
};

/** @brief For when an output is unused, but a connection is required anyway. */
class DisconnectedSink : public Module {
    public:
        DisconnectedSink(std::string name, Location, unsigned size);
        virtual std::string GenericName() override;
        virtual ~DisconnectedSink();
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
};

class CustomModule : public Module {
    public:
        CustomModule(std::string name, Location, std::vector<std::string> Function, unsigned size = DEFAULT_SIZE, std::string pred = ""); // ("name", {"functionality"}, size)
        virtual ~CustomModule();
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
        virtual std::string GenericName() override;
    private:
        std::vector<std::string> Function; // functionality lines of code (this will be printed in GenFunctionality()
};

class CustomModuleSingleInput : public Module {
    public:
        CustomModuleSingleInput(std::string name, Location, std::vector<std::string> Function, unsigned size = DEFAULT_SIZE, std::string pred = ""); // ("name", {"functionality"}, size)
        virtual ~CustomModuleSingleInput();
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
        virtual std::string GenericName() override;
    private:
        std::vector<std::string> Function; // functionality lines of code (this will be printed in GenFunctionality()
};

class UserModule : public Module {
    public:
        UserModule(std::string prototype, std::string name, Location, std::vector<std::string> Ports); // ("prototype name", "module name", {"list of all ports"})
        virtual void GenModuleVerilog() override { }; // by default, have no implementation
        virtual void CoreIRGenModuleVerilog(CoreIR::Context*, int)  override { } // by default, have no implementation
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual ~UserModule();
    private:
        std::string prototype; // prototype name of module
};

class ConfigCell : public Module {
    public:
        ConfigCell(std::string name, int contexts = 1, Location loc = {0,0}); // Constructor of the configuration cell; initializes the name
        void GenModuleVerilog() override; // Generates the verilog for a configuration cell
        virtual nlohmann::json CoreIRGenFunctionality() override;
        const std::string& getName() const { return name; } // returns the instance name
        virtual std::string GenericName() override; // returns the module name
        int getStorageSize() const { return Module::getSize(); }
        Port& getSingleConnectedPort() const {
            if (connected_ports.size() < 1) { throw cgrame_error("trying to get single connected port on ConfigCell with multiple (or zero) ports"); }
            return *connected_ports.at(0);
        }
        const std::vector<Port*>& getAllConnectedPorts() const { return connected_ports; }

        void addControledPorts(const std::vector<Port*>& new_ports);
        int l_contexts = 1;

    private:
        std::vector<Port*> connected_ports; // All ports that the config cell is connected to
        std::vector<unsigned> contexts; // vector containing contexts
};

class EventTransitionTable : public Module {
public:
    static const std::map<OpGraphOpCode,LLVMMode> all_modes;    
    EventTransitionTable(std::string name, int contexts, Location loc = {0,0}); // Constructor of the event dirven transition table; initializes the name
    virtual nlohmann::json CoreIRGenFunctionality() override;
    virtual std::string GenericName() override; // returns the module name
    virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	    const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
    ) const override;
    virtual MRRG* createMRRG(unsigned II) override;
};

class ContextCell : public Module {
public:
    ContextCell(std::string name, int contexts, Location loc = {0,0}); // Constructor of the configuration cell; initializes the name
    virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
    ) const override;
    virtual std::string GenericName() override; // returns the module name
private: 
    int l_contexts = 1;
};

class ContextCounter : public Module {
public:
    ContextCounter(std::string name, int contexts, Location loc = {0,0}); // Constructor of the configuration cell; initializes the name
    virtual nlohmann::json CoreIRGenFunctionality() override;
    virtual std::string GenericName() override; // returns the module name
};

class CaseStatement : public Module {
    public:
        CaseStatement(std::string name, Location, unsigned numInputs, unsigned size, int latency, bool isElastic = false);
        virtual std::string GenericName() override;
        virtual void GenFunctionality() override;
        virtual nlohmann::json CoreIRGenFunctionality() override;
        virtual ~CaseStatement();
        unsigned getNumInputs(){return numInputs;};
    private:
        unsigned numInputs;
        int latency;
};

class Compare : public Module {
    public:
        Compare(std::string name, Location, unsigned size, int II);
        virtual ~Compare();
        virtual std::string GenericName() override; // Generates a generic name for the module
        virtual nlohmann::json CoreIRGenFunctionality() override;
        virtual BitConfig getBitConfig(
            const MRRG& mrrg, const OpGraph & og,
	        const Mapping& map,
            const ConfigCell& ccell,
            const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
            const MRRGNodesFromValNode& mrrg_nodes_from_val_node
        ) const override;

        MRRG* createMRRG(unsigned II) override;
    
	static const std::map<OpGraphOpCode, LLVMMode> all_modes;
    protected:

};
#endif
