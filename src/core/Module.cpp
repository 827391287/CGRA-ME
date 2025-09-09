/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

// Define statements included in .h file
#include <CGRA/Module.h>
#include <CGRA/ModuleProcedures.h>
#include <CGRA/ModuleElastic.h>

#include <coreir/passes/analysis/coreirjson.h>

#include <ios>
#include <fstream>
#include <ostream>
#include <sstream>
#include <regex>

namespace {
    std::string makeParameterListSyntaxItem(const std::vector<ResolvedVeroligModuleParameter>& resolved_params) {
        if (resolved_params.empty()) {
            return "";
        } else {
            std::string paramstr = "#(";

            for (const auto& resolved_param : resolved_params) {
                paramstr += '.' + resolved_param.name + '(' + resolved_param.value + "), ";
            }

            paramstr[paramstr.size() - 2] = ')'; // overwrite trailing comma with closing paren! (keep the space)
            return paramstr;
        }
    }

    std::pair<Port,Port> splitBidirPort(const Port& p) {
        auto result =  std::make_pair(p, p);
        result.first.name += "_in";
        result.second.name += "_out";
        result.first.pt = PORT_INPUT;
        result.second.pt = PORT_OUTPUT;
        return result;
    }

    std::string makePortDeclaration(const Port& p, const char* indent) {
        std::string result;
        if (p.pt == PORT_BIDIR) {
            const auto ioport_copies = splitBidirPort(p);
            result += indent + ioport_copies.first.makeVerilogDeclaration() + '\n';
            result += indent + ioport_copies.second.makeVerilogDeclaration() + '\n';
        } else {
            result += indent + p.makeVerilogDeclaration() + '\n';
        }
        return result;
    }

    template<typename Ports, typename Ports2>
    std::string makePortsDeclaration(const Ports& ports, const Ports2& portsToPropagate, const char* indent) {
        std::string result;
        for (auto& port : ports) {
            result += makePortDeclaration(*port.second, SET_INDENT);
        }
        for (auto& port : portsToPropagate) {
            result += makePortDeclaration(port, SET_INDENT);
        }
        return result;
    }

    template<typename Ports, typename Ports2>
    nlohmann::json makeCoreIRInterface(const Ports& ports, const Ports2& portsToPropagate) {
        nlohmann::json result = {};

        for (auto& port : ports) {
            if (port.second->pt == port_type::PORT_BIDIR) {
                const auto ioport_copies = splitBidirPort(*port.second);
                result.push_back(ioport_copies.first.name);
                result.push_back(ioport_copies.second.name);
            } else {
                std::string portName = port.second->getName();
                result.push_back(portName);
            }
        }

        for (auto& port : portsToPropagate) {
            std::string portName = port.getName();
            result.push_back(portName);
        }

        return result;
    }

} // end anon namespace

std::ostream& operator<<(std::ostream& os, VerilogType vt) {
    switch(vt) {
        case VerilogType::CGRAME: return os << "CGRAME";
        case VerilogType::CoreIR: return os << "CoreIR";

        default: return os << "FIXME_UNHANDLED_VerilogType_PRINTED";
    }
}

std::string Port::makeVerilogDeclaration() const {
    std::string result;
    switch (pt) {
        case port_type::PORT_INPUT:         result += "input"; break;
        case port_type::PORT_OUTPUT:        result += "output"; break;
        case port_type::PORT_OUTPUT_REG:    result += "output reg"; break;
        case port_type::PORT_BIDIR:         result += "inout"; break;
        default: throw make_from_stream<cgrame_error>([&](auto&& s) {
            s << "unhandled port_type " << pt << " in " << __func__;
        });
    }
    if (not parameter.empty()) {
        result += " [" + parameter + "-1:0]";
    } else {
        result += " [" + std::to_string(size - 1) + ":0]";
    }

    result += ' ' + name + ';';

    return result;
}

// Constructor
Module::Module(std::string name, Location loc, unsigned size, bool isElastic)
    : Module(name, "", loc, size,  isElastic)
{ }

Module::Module(std::string name, std::string template_name, Location loc, unsigned size,  bool isElastic)
    : parameterlist()
    , ports()
    , connections()
    , submodules()
    , configcells()
    , parent(nullptr)
    , data_size(size)
    , templateName(template_name)
    , name(name)
    , adds_synchronous_circuitry(false)
    , isElastic(isElastic)
    , loc(loc)
{ }

// Destructor, handling dynamically declared data
Module::~Module()
{
    for(auto it = connections.begin(); it != connections.end(); ++it) // Deleting connections
        delete it->second;
    for(auto it = ports.begin(); it != ports.end(); ++it) // Deleting ports
        delete it->second;
    for(auto it = submodules.begin(); it != submodules.end(); ++it)  // Deleting submodules
        delete it->second;
    for(auto it = configcells.begin(); it != configcells.end(); ++it)  // Deleting configcells
        delete it->second;
}

// Function that returns ALL the modules that have config cells attached to them, and their order
// This is a void function insead of a std::vector<Module*> function because it would otherwise use lots of memory (as it is recursive)
void Module::genConfigOrder(std::vector<ConfigCell*> & ConfigTable) const
{
    for(const auto& name_and_configcell : configcells) {
        ConfigTable.push_back(name_and_configcell.second);
    }

    for(const auto& name_and_submodule : submodules) {
        name_and_submodule.second->genConfigOrder(ConfigTable);
    }
}

void Module::genVerilogCGRAME(std::string dir) {
    std::set<std::string> PrintedModules; // Data structure to keep track of the modules that were printed
    std::queue<Module*> ToPrint; // Modules to print
    GetModulesToPrint(ToPrint, PrintedModules); // Get the modules that need to be printed (puts them in queue)
    // FIXME: currently follows how modules are queued;
    // however, these variable names are horrible and
    // needs an update
    std::set<unsigned> PrintedConfigs;
    std::queue<ConfigCell*> ConfigsToPrint;
    GetConfigsToPrint(ConfigsToPrint, PrintedConfigs);

    while (!ToPrint.empty()) // Printing out the module prototypes in the queue
    {
        std::string filename = dir + ToPrint.front()->GenericName() + ".v";
        std::cout << "    Generating \"" << filename << "\" ... ";

        std::streambuf* original_sbuf = std::cout.rdbuf();
        std::ofstream hdl_filebuf(filename);
        std::cout.rdbuf(hdl_filebuf.rdbuf()); // Redirects to new file

        // Dumping module Verilog
        ToPrint.front()->GenModuleVerilog(); // Printing the top
        ToPrint.pop(); // Shrinking the queue

        std::cout.rdbuf(original_sbuf); // Restores printout
        std::cout << "Done!" << std::endl;
    }
    while (!ConfigsToPrint.empty())
    {
        std::string cellSize = std::to_string(ConfigsToPrint.front()->getStorageSize());
        std::string filename = dir + "configCell_" + cellSize + "b.v";
        std::cout << "    Generating \"" << filename << "\" ... ";

        std::streambuf* original_sbuf = std::cout.rdbuf();
        std::ofstream hdl_filebuf(filename);
        std::cout.rdbuf(hdl_filebuf.rdbuf()); // Redirects to new file
        ConfigsToPrint.front()->GenModuleVerilog();
        ConfigsToPrint.pop();

        std::cout.rdbuf(original_sbuf); // Restores printout
        std::cout << "Done!" << std::endl;
    }
}

// Determine the modules necessary to print out
void Module::GetModulesToPrint(std::queue<Module*> & ToPrint, std::set<std::string> & PrintedModules)
{
    // Checking to see if the module has been processed already
    std::string UniqueModuleName = GenericName(); // Getting unique string to identify the module

    // Check to see if the module was processed
    if (PrintedModules.find(UniqueModuleName) != PrintedModules.end()) // Already set to be printed
        return;

    // Otherwise, the module has not been printed yet, add it to the printed module list and generate verilog code (IMPORTANT TO DO THIS AFTER SUBMODULE CALLS)
    PrintedModules.insert(UniqueModuleName); // Mappping the hash to the name of the function that will be printed
    ToPrint.push(this); // Adding the print job to the queue

    // Recursive calls for submodules - get necessary submodules to print
    for(const auto& name_and_submodule : submodules)
        name_and_submodule.second->GetModulesToPrint(ToPrint, PrintedModules);
}

// Use `uniq` as reference to determine whether module itself contains config
// cell of new size, and populate `q`
void Module::GetConfigsToPrint(std::queue<ConfigCell*>& q, std::set<unsigned>& uniq)
{
    if (hasConfigCells())
    {
        for (const auto& it : configcells)
        {
            auto ccell = it.second;
            auto size = ccell->getStorageSize();
            if (uniq.find(size) == uniq.end()) // new size found
            {
                uniq.insert(size);
                q.push(ccell);
            }
        }
    }
    for(const auto& it : submodules)
        it.second->GetConfigsToPrint(q, uniq);
}

// This generates a generic name for a module... This can be overriden!
std::string Module::GenericName()
{
    //NOTE: This method used to return the name member after removing
    //all numeric characters within the string, for unclear reasons
    return this->templateName;
}

// Generates the Verilog code for a module
void Module::GenModuleVerilog()
{
    // Determining if there are config cells in the module
    bool HasConfig = moduleRequiresConfigPorts(*this);
    // Determining if there are any registers in the module
    bool HasRegs = moduleRequiresClockPorts(*this);

    // Printing out the module header
    GenModuleHeader(HasConfig, HasRegs);

    // Generating the parameters of the module
    GenParameters();

    // Printing out the port specifications (input or output)
    GenPortSpecs(HasConfig, HasRegs);

    // Printing out verilog for connections and submodules
    GenConnections();

    // Then add the module's functionality
    GenFunctionality();

    // Printing out the end of the module
    std::cout << "endmodule\n\n";
}

CoreIR::TypeGenFun Module::makeCoreIRInterfaceGenerator() {
    return [
        has_synchronous_cells = moduleRequiresClockPorts(*this),
        needs_config_ports = moduleRequiresConfigPorts(*this),
        is_cgra_top = this->name == "CGRA",
        size = getSize(),
        ports = this->ports,
        portsToPropagate = this->portsToPropagate
    ] (
        CoreIR::Context* c, CoreIR::Values genargs
    ) -> CoreIR::RecordType* {
        CoreIR::RecordParams interface = {};
        if (needs_config_ports) {
            interface.push_back({"ConfigIn", c->BitIn()});
            interface.push_back({"ConfigOut", c->Bit()});
            interface.push_back({"Config_Clock", c->BitIn()});
            interface.push_back({"Config_Reset", c->BitIn()});
            interface.push_back({"CGRA_Clock", c->BitIn()});
            interface.push_back({"CGRA_Reset", c->BitIn()});
            interface.push_back({"CGRA_Enable", c->BitIn()});
        } else if (has_synchronous_cells) {
            interface.push_back({"CGRA_Clock", c->BitIn()});
            interface.push_back({"CGRA_Reset", c->BitIn()});
            interface.push_back({"CGRA_Enable", c->BitIn()});
        }

        for (auto& port : ports)
        {
            std::string portName = port.second->getName();
            port_type portType = port.second->pt;
            unsigned portSize = port.second->size;
            unsigned portSizeForInterface; // Stores the port size, after checking for parameters
            if (portSize == PARAMETERIZED)
            {
                CoreIR::Value* paramArg = genargs.at(port.second->parameter); // Use the stored parameter name to get the port size from input arguments
                portSizeForInterface = (portSize > 0)? portSize : paramArg->get<int>();
            }
            else
            {
                portSizeForInterface = portSize;
            }
            CoreIR::ArrayType* portTypeForInterface; // Stores the port type in CoreIR's desired format
            if(portType == port_type::PORT_INPUT)
            {
                portTypeForInterface= c->Array(portSizeForInterface,c->BitIn());
            }
            else if (portType == port_type::PORT_OUTPUT || portType == port_type::PORT_OUTPUT_REG)
            {
                // CoreIR doesn't care if the output of an in-memory representation is a reg or not (reg is implemented in the actual verilog printing)
                portTypeForInterface = c->Array(portSizeForInterface,c->Bit());
            }
            else if (portType == port_type::PORT_BIDIR)
            {
                // replace bidirs with and in+out; CoreIR verilog gen doesn't support bidirs
                const auto ioport_copies = splitBidirPort(*port.second);
                interface.push_back({ioport_copies.first.name, c->Array(portSizeForInterface,c->BitIn())});
                interface.push_back({ioport_copies.second.name, c->Array(portSizeForInterface,c->Bit())});
                continue;
                // portTypeForInterface = c->Array(portSizeForInterface,c->BitInOut()); // Adding an inout port portSizeForInterface bits wide
            } else {
                throw make_from_stream<cgrame_error>([&](auto& s) {
                    s << __func__ << " port "<< portName << " doesn't handle " << portType << " port type";
                });
            }
            interface.push_back({port.second->getName(), portTypeForInterface});
        }

        for (auto& port : portsToPropagate)
        {
            std::string portName = port.getName();
            if (ports.find(portName) != ports.end()) {
                continue;
            }
            port_type portType = port.pt;
            unsigned portSize = port.size;
            unsigned portSizeForInterface; // Stores the port size, after checking for parameters
            if (portSize == PARAMETERIZED)
            {
                CoreIR::Value* paramArg = genargs.at(port.parameter); // Use the stored parameter name to get the port size from input arguments
                portSizeForInterface = (portSize > 0)? portSize : paramArg->get<int>();
            }
            else
            {
                portSizeForInterface = portSize;
            }
            CoreIR::ArrayType* portTypeForInterface; // Stores the port type in CoreIR's desired format
            if(portType == port_type::PORT_INPUT)
            {
                portTypeForInterface= c->Array(portSizeForInterface,c->BitIn());
            }
            else if (portType == port_type::PORT_OUTPUT || portType == port_type::PORT_OUTPUT_REG)
            {
                // CoreIR doesn't care if the output of an in-memory representation is a reg or not (reg is implemented in the actual verilog printing)
                portTypeForInterface = c->Array(portSizeForInterface,c->Bit());
            }
            else if (portType == port_type::PORT_BIDIR)
            {
                // replace bidirs with and in+out; CoreIR verilog gen doesn't support bidirs
                const auto ioport_copies = splitBidirPort(port);
                interface.push_back({ioport_copies.first.name, c->Array(portSizeForInterface,c->BitIn())});
                interface.push_back({ioport_copies.second.name, c->Array(portSizeForInterface,c->Bit())});
                continue;
                // portTypeForInterface = c->Array(portSizeForInterface,c->BitInOut()); // Adding an inout port portSizeForInterface bits wide
            } else {
                throw make_from_stream<cgrame_error>([&](auto& s) {
                    s << __func__ << " doesn't handle " << portType << " port type";
                });
            }
            interface.push_back({port.getName(), portTypeForInterface});
        }

        return c->Record(interface);
    };
}

CoreIR::ModuleDefGenFun Module::makeCoreIRModuleDefinitonGenerator() {
    return [
       =
    ] (
        CoreIR::Context* c, CoreIR::Values genargs, CoreIR::ModuleDef* def
    ) -> void {
        (void)genargs;
        // Add submodules
        for (auto submodule : submodules)
        {
            CoreIR::Generator* submodGen = c->getGenerator("cgrame." + submodule.second->GenericName());
            // Change/add submodule parameters below ( {"size", CoreIR::Const::make(c, 32)} )
            CoreIR::Values submoduleParams = {};
            CoreIR::Module* submod = submodGen->getModule(submoduleParams);
            // Change submodule values/initializing values below ( {"value", CoreIR::Const::make(c, 1)} )
            CoreIR::Values submoduleInitializer = {};
            CoreIR::Instance* submoduleinstance = def->addInstance(submodule.first, submod, submoduleInitializer);
            (void)submoduleinstance;
        }

        // Add ConfigCells
        for (auto configcell : configcells)
        {
            CoreIR::Generator* configGen = c->getGenerator("cgrame." + configcell.second->GenericName());

            // Change/add ConfigCell parameters below ( {"size", CoreIR::Const::make(c, 32)} )
            CoreIR::Values configCellParams = {};
            CoreIR::Module* configMod = configGen->getModule(configCellParams);
            // Change ConfigCell values/initializing values below ( {"value", CoreIR::Const::make(c, 1)} )
            CoreIR::Values configCellInitializer = {};
            CoreIR::Instance* configCellInstance = def->addInstance(configcell.first, configMod, configCellInitializer);
            (void)configCellInstance;
        }

        // Add connections
        // Special Cases:
        auto is_cgra_top = this->name == "CGRA";
        for (auto submodule : submodules)
        {
            if (moduleRequiresConfigPorts(*submodule.second))
            {
                def->connect("self.Config_Clock", submodule.first + ".Config_Clock");
                def->connect("self.Config_Reset", submodule.first + ".Config_Reset");
                def->connect("self.CGRA_Clock", submodule.first + ".CGRA_Clock");
                def->connect("self.CGRA_Reset", submodule.first + ".CGRA_Reset");
                def->connect("self.CGRA_Enable", submodule.first + ".CGRA_Enable");
            }
            else if (moduleRequiresClockPorts(*submodule.second))
            {
                def->connect("self.CGRA_Clock", submodule.first + ".CGRA_Clock");
                def->connect("self.CGRA_Reset", submodule.first + ".CGRA_Reset");
                def->connect("self.CGRA_Enable", submodule.first + ".CGRA_Enable");
            }
        }

        for (auto configCell : configcells)
        {
            def->connect("self.CGRA_Clock", configCell.first + ".CGRA_Clock");
            def->connect("self.CGRA_Reset", configCell.first + ".CGRA_Reset");
            def->connect("self.CGRA_Enable", configCell.first + ".CGRA_Enable");
            def->connect("self.Config_Clock", configCell.first + ".Config_Clock");
            def->connect("self.Config_Reset", configCell.first + ".Config_Reset");
        }

        const auto addModulePath = [this_ptr=this](const Module& m, const std::string& port_name) {
            if (&m == this_ptr) {
                return "self." + port_name;
            } else {
                return m.getName() + '.' + port_name;
            }
        };

        // Submodule->Submodule / Submodule->Port connections:
        for (auto portAndConnection : connections)
        {
            Connection* connectionSpecifier = portAndConnection.second;

            for (auto dstConnection : connectionSpecifier->dst)
            {
                const Port* src = connectionSpecifier->src;
                const Port* dst = dstConnection;
                if (&src->getModule() == this) { std::swap(src,dst); }

                const auto src_ioport_copy = splitBidirPort(*src);
                const auto dst_ioport_copy = splitBidirPort(*dst);
                const bool src_is_bidir = src->pt == PORT_BIDIR;
                const bool dst_is_bidir = dst->pt == PORT_BIDIR;

                std::string srcPortName = src->getName();
                std::string dstPortName = dst->getName();
                if (src_is_bidir) {
                    switch (dst->pt) {
                        case port_type::PORT_OUTPUT:
                        case port_type::PORT_OUTPUT_REG:
                        case port_type::PORT_BIDIR:
                            srcPortName = src_ioport_copy.second.getName();
                        break;
                        case port_type::PORT_INPUT:
                            srcPortName = src_ioport_copy.first.getName();
                        break;
                        default: break;
                    }
                }
                if (dst_is_bidir) {
                    switch (src->pt) {
                        case port_type::PORT_OUTPUT:
                        case port_type::PORT_OUTPUT_REG:
                        case port_type::PORT_BIDIR:
                            dstPortName = dst_ioport_copy.second.getName();
                        break;
                        case port_type::PORT_INPUT:
                            dstPortName = dst_ioport_copy.first.getName();
                        break;
                        default: break;
                    }
                }

                def->connect(
                    addModulePath(src->getModule(), srcPortName),
                    addModulePath(dst->getModule(), dstPortName)
                );

                // the above will handle out <-> out, so also connect in <-> in
                if (src_is_bidir && dst_is_bidir) {
                    def->connect(
                        addModulePath(src->getModule(), src_ioport_copy.first.getName()),
                        addModulePath(dst->getModule(), dst_ioport_copy.first.getName())
                    );
                }
            }
        }

        // ConfigCell->Submodule / ConfigCell->Port connections:
        std::string srcConfigInstance;
        std::string srcConfig;
        std::string stopSrcConfig;
        std::string validSrcConfig;
        std::string dstConfigInstance;
        std::string dstConfigPortName;
        std::string dstConfig;
        std::string stopDstConfig;
        std::string validDstConfig;
        for (auto configCell : configcells)
        {
            srcConfigInstance = configCell.second->getName();
            srcConfig = srcConfigInstance + ".select";
	        validDstConfig = srcConfigInstance + ".valid";
	        stopDstConfig = srcConfigInstance + ".stop";
            for (auto configCellConnection : configCell.second->getAllConnectedPorts())
            {
                if (&(configCellConnection->getModule()) == this)
                {
                    dstConfigInstance = "self";
                }
                else
                {
                    dstConfigInstance = configCellConnection->getModule().getName();
                }
                dstConfigPortName = configCellConnection->getName();
                dstConfig = dstConfigInstance + "." + dstConfigPortName;
		        validSrcConfig = dstConfig + "_valid_upstream";
		        stopSrcConfig = dstConfig + "_stop_upstream";
                def->connect(srcConfig, dstConfig);

		        if (dynamic_cast<ElasticConfigCell*>(configCell.second)) {
		            def->connect(validDstConfig, validSrcConfig);
		            def->connect(stopDstConfig, stopSrcConfig);
		        }
            }
        }

        // Configuration Cell chain
        std::string PreviousWire = "self.ConfigIn";
        std::string ConfigInput;
        for (auto configCell : configcells)
        {
            ConfigInput = PreviousWire;
            def->connect(ConfigInput, configCell.second->getName() + ".ConfigIn");
            PreviousWire = configCell.second->getName() + ".ConfigOut";
        }
        for (auto submodule : submodules)
        {
            if (moduleRequiresConfigPorts(*submodule.second))
            {
                ConfigInput = PreviousWire;
                def->connect(ConfigInput, submodule.first + ".ConfigIn");
                PreviousWire = submodule.first + ".ConfigOut";
            }
        }
        if(moduleRequiresConfigPorts(*this))
        {
            def->connect(PreviousWire, "self.ConfigOut"); // End the ConfigCell chain at the ConfigOut port if there were actually ConfigCells inside the module
        }
    };
}

// Generates the coreIR in-memory representation of a module
void Module::CoreIRGenModuleVerilog(CoreIR::Context* c, int contexts)
{
    CoreIR::Namespace* cgrame = c->getNamespace("cgrame");
    const auto makeIntParameter = [c](auto& param) -> CoreIR::Params::value_type { return {param.first, c->Int()}; };
    const auto makeConstParameter = [c](auto& param) -> CoreIR::Values::value_type { return {param.first, CoreIR::Const::make(c, param.second)}; };

    CoreIR::Params moduleParams;
    if (dynamic_cast<const ConfigCell*>(this)) {
        parameterlist.emplace("contexts", dynamic_cast<const ConfigCell*>(this)->l_contexts);
    }
    std::transform(parameterlist.begin(), parameterlist.end(), std::inserter(moduleParams, moduleParams.end()), makeIntParameter);
    moduleParams.insert({"contexts", c->Int()});
    CoreIR::Values defaultArgs = {};
    std::transform(parameterlist.begin(), parameterlist.end(), std::inserter(defaultArgs, defaultArgs.end()), makeConstParameter);
    defaultArgs.insert({"contexts", CoreIR::Const::make(c, contexts)});

    CoreIR::TypeGen* tg = cgrame->newTypeGen(GenericName() + "tg", moduleParams, makeCoreIRInterfaceGenerator());
    CoreIR::Generator* modGen = cgrame->newGeneratorDecl(GenericName(), tg, moduleParams);
    modGen->addDefaultGenArgs(defaultArgs);

    if (submodules.empty() && !hasConfigCells()) {
        // Case A: If the module is a primitive (no submodules or ConfigCells)
        modGen->getMetaData()["verilog"] = CoreIRGenFunctionality();
        std::vector<std::string> portNames;
    } else {
        // Case B: The Module is a composite: (has submodules and/or ConfigCells)
        modGen->setGeneratorDefFromFun(makeCoreIRModuleDefinitonGenerator());
    }

    std::vector<std::string> portNames;
    for (auto & port : portsToPropagate) {
        portNames.push_back(port.getName());
    }
    if (!portNames.empty()) {
        modGen->getMetaData()["propagatePorts"] = portNames;
    }
}

// Generating the module header in Verilog code
void Module::GenModuleHeader(bool HasConfig, bool HasRegisters)
{
    std::vector<std::string> PortList; // Buffer that contains ports that we want to print out
    std::cout << "module " << GenericName() << "("; // Printing module header and name, e.g. "module MyModuleName ( "
    if (HasConfig) // If there are config cells inside, we need to add some extra ports
    {
        PortList.push_back("Config_Clock");
        PortList.push_back("Config_Reset");
        PortList.push_back("ConfigIn");
        PortList.push_back("ConfigOut");
    }
    else if (HasRegisters) // If there are registers inside, we want to add some other extra ports
    {
        PortList.push_back("CGRA_Clock");
        PortList.push_back("CGRA_Reset");
    }
    // Getting all the ports that the module has (port specifications such as input, output, inout come later)
    for(const auto& name_and_port : ports)
        PortList.push_back(name_and_port.second->name);

    for (unsigned i = 0; i < PortList.size(); i++) // For all ports
    {
        std::cout << PortList[i]; // Print it out
        if (i != PortList.size() - 1) // Space out the ports with commas
            std::cout << ", ";
    }
    std::cout << ");\n";  // Ending the module header
}

// Printing out the parameters in the verilog module
void Module::GenParameters()
{
    for(std::map<std::string,unsigned>::iterator it = parameterlist.begin(); it != parameterlist.end(); ++it) // For all parameters
    {
        std::cout << SET_INDENT << "parameter " << it->first << " = " << it->second << ";\n"; // Print it along with its default size
    }
}

// Printing out all the ports, and their specifications (input or output)
void Module::GenPortSpecs(bool HasConfig, bool HasRegisters)
{
    std::cout << SET_INDENT << "// Specifying the ports\n"; // Port generation comment
    if (HasConfig) // If there are config cells, declare the following port specifications
    {
        std::cout << SET_INDENT << "input Config_Clock, Config_Reset, ConfigIn;\n";
        std::cout << SET_INDENT << "output ConfigOut;\n";
    }
    if (HasRegisters)
        std::cout << SET_INDENT << "input CGRA_Clock, CGRA_Reset;\n";
    // Now, go through all the other ports and print their specifications
    for(std::map<std::string,Port*>::iterator it = ports.begin(); it != ports.end(); ++it)
    {
        Port* curPort = it->second;
        if (curPort->pt == PORT_INPUT) // If port is input
            std::cout << SET_INDENT << "input ";
        else if  (curPort->pt == PORT_OUTPUT) // If the port is output
            std::cout << SET_INDENT << "output ";
        else if (curPort->pt == PORT_OUTPUT_REG) // If the port is output reg
            std::cout << SET_INDENT << "output reg ";
        else if (curPort->pt == PORT_BIDIR) // If the port is a bidirectional
            std::cout << SET_INDENT << "inout ";
        else // Otherwise, print out an error message
            std::cout << "<ERROR: DID NOT DECLARE PORT SPECIFICATION!>";

        // Printing the size of the port
        if (curPort->size == PARAMETERIZED) // If the port size is parameterized, include the parameter name in there
            std::cout << "[" << curPort->parameter << "-1:0] ";
        if (curPort->size > 1) // If the port size is greater than one, use array notation
            std::cout << "[" << curPort->size-1 << ":0] ";
        // Printing out the port name
        std::cout << curPort->name << ";\n";
    }
}

// Generates the verilog for connections
void Module::GenConnections()
{
    StringMatrix Matrix; // Declaring a matrix to hold connection information
    PrintList WireList, SubmodList, AssignList; // Buffers that will contain output... wirelist->wires, submodulelist->submodules and configcells, assignlist->assign statements
    GenerateMatrix(Matrix); // Generating the matrix

    // Deals with how to generate verilog code to describe all the connections between the module, submodule, and config cells. Also generates necessary wires
    DetermineConnections(Matrix, WireList, SubmodList, AssignList); // The verilog information will be stored in the three buffers (WireList, SubmodList, and AssignList)

    // Printing out those buffers (that contain the actual verilog code to be printed)
    WireList.print();
    SubmodList.print();
    AssignList.print();
}

// This function exists to be overridden
void Module::GenFunctionality()
{
    // Currently set to do nothing
}

// Generating the matrix that will hold all the connections between submodules
void Module::GenerateMatrix(StringMatrix & Matrix)
{
    // Intialization of matrix size (this does not change later)
    for(std::map<std::string,Module*>::iterator it = submodules.begin(); it != submodules.end(); ++it)
    {
        std::vector<std::string> StringVector;
        // XXX: This leaves dangling connections being hard-wired to 0s
        for (auto& port : it->second->ports)
        {
            StringVector.push_back("." + port.first + "(0) " + PORT_DEFAULT_CONNECTION); // overwritten by valid connections afterwards
        }
        Matrix.push_back(StringVector); // Adding vector to matrix
    }
}

nlohmann::json Module::CoreIRGenFunctionality()
{
    throw make_from_stream<cgrame_error>([&](auto&& s) {
        s << "module has no Module::CoreIRGenFunctionality";
    });
}

// Deals with how to generate verilog describing the connections between the module, submodule, and config cells. Also generates necessary wires
void Module::DetermineConnections(StringMatrix & Matrix, PrintList & WireList, PrintList & SubmodList, PrintList & AssignList)
{
    std::stringstream buffer; // Buffers to append strings
    std::string Signal; // Holds the wire name
    std::string PreviousWire, ConfigInput, ConfigOutput; // Stores the names of verilog ports/wires
    int SubmodIndex, PortIndex; // Variables used to keep track of position in matrix

    PreviousWire = "ConfigIn";

    // ************** CONFIG CELL WIRING IN MODULE ************** //

    if (!configcells.empty()) // If there are config cells, add comments
    {
        WireList.add({"\n", SET_INDENT, "// Wires for the the config cells\n"});
        SubmodList.add({"\n", SET_INDENT, "// Declaring the config cells\n"});
    }
    for(std::map<std::string, ConfigCell*>::iterator it = configcells.begin(); it != configcells.end(); ++it) // For all the config cells in the module
    {
        buffer << it->second->name << "_sig"; // Naming the wire (that will connect to the submodule)
        Signal = buffer.str();
        buffer.str(std::string()); // Clearing the buffer

        // Declaring the verilog wire for connecting the config cell to submodule
        if (it->second->getStorageSize() > 1) // If size is greater than one,
            WireList.add({SET_INDENT, "wire [", std::to_string(it->second->getStorageSize() - 1), ":0] ", Signal, ";\n"});
        else // Otherwise, if the size is 1
            WireList.add({SET_INDENT, "wire ", Signal, ";\n"});

        ConfigInput = PreviousWire; // Otherwise, wire the previous wire in to ConfigIn. This is to make a chain of config cells

        // Declaring the verilog wire necessary to connect the config cell chain
        buffer << it->second->name << "_config";
        ConfigOutput = buffer.str();
        buffer.str(std::string()); // Clearing the buffer
        WireList.add({SET_INDENT, "wire ", ConfigOutput, ";\n"});
        PreviousWire = ConfigOutput; // Setting the previous wire (necessary for making the chain)

        // Declaring the config cell
        SubmodList.add({SET_INDENT , "configCell_", std::to_string(it->second->getStorageSize()), "b ", it->second->name, " (\n",
                SET_DOUBLE_INDENT, ".CGRA_Clock(CGRA_Clock),\n", SET_DOUBLE_INDENT, ".CGRA_Reset(CGRA_Reset),\n",
                SET_DOUBLE_INDENT, ".Config_Clock(Config_Clock),\n", SET_DOUBLE_INDENT, ".Config_Reset(Config_Reset),\n",
                SET_DOUBLE_INDENT, ".ConfigIn(", ConfigInput, "),\n", SET_DOUBLE_INDENT, ".ConfigOut(" , ConfigOutput , "),\n",
                SET_DOUBLE_INDENT, ".select(", Signal, "));\n"});

        // Modify the matrix to show the connection between config cell and submodule
        for (auto* port : it->second->getAllConnectedPorts())
        {
            SubmodIndex = FindSubmoduleIndex(port->parent->name); // Find submodule index
            if (SubmodIndex < 0) continue; // Case: connecting to port instead of submodule
            PortIndex = port->parent->FindPortIndex(port->name); // Find port index that it is connected to
            Matrix[SubmodIndex][PortIndex] = "." + port->name  + "(" + Signal + ")"; // Update that entry in the matrix
        }
    }

    // ************** END OF CONFIG CELL WIRING IN MODULE ************** //

    // ************** WIRING OTHER CONNECTIONS WITHIN MODULE (between module and submodules, configcells excluded) ************** //

    if (!connections.empty()) // If there are connections, write a comment
        WireList.add({"\n",SET_INDENT, "// Wires connecting the main module and submodules\n"});

    // For all connection objects, between modules/submodules (config cells not included)
    for(std::map<Port*,Connection*>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
        if (it->second->src->parent == this) // If the source port is one of the main module's inputs
        {
            for(unsigned j = 0; j < it->second->dst.size(); j++) // For all destination ports
            {
                if (it->second->dst[j]->parent == this) // If the destination port is not in a submodule, connect directly to output
                {
                    AssignList.add({SET_INDENT, "assign ", it->second->dst[j]->name, " = ", Signal, ";\n"});
                    continue;
                }
                // Otherwise it is a submodule, and assign its connection appropriately
                SubmodIndex = FindSubmoduleIndex(it->second->dst[j]->parent->name); // Finding the submodule that the destination is connected
                PortIndex = it->second->dst[j]->parent->FindPortIndex(it->second->dst[j]->name); // Go to that submodule, and then find the port index of that port name
                buffer << "." << it->second->dst[j]->name << "(" << it->second->src->name << ")";
                Matrix[SubmodIndex][PortIndex] = buffer.str();
                buffer.str(std::string()); // Clearing the buffer
            }
        }
        else // If the source port is in one of the submodules
        {
            if (it->second->dst.size() == 1 && it->second->dst[0]->parent == this) // If we only have one port connection and its connected to the main module's port, we do not want a wire
            {
                int SubmodIndex = FindSubmoduleIndex(it->second->src->parent->name); // Finding the submodule that the destination is connected
                int PortIndex = it->second->src->parent->FindPortIndex(it->second->src->name); // Go to that submodule, and then find the port index of that port name
                buffer << "." << it->second->src->name << "(" << it->second->dst[0]->name << ")";
                Matrix[SubmodIndex][PortIndex] = buffer.str();
                buffer.str(std::string()); // Clearing the buffer
                continue;
            }
            // Naming and declaring the verilog wire necessary
            buffer << it->second->src->parent->name << "_" << it->second->src->name << "_sig"; // Naming the wire
            Signal = buffer.str(); // Assigning the wire the name
            buffer.str(std::string()); // Clearing the buffer

            // Declaring the verilog wire
            if (it->second->src->size == PARAMETERIZED && it->second->src->parameter == "size")
            {
                WireList.add({SET_INDENT , "wire [" , std::to_string(it->second->src->parent->getSize() - 1) , ":0] " , Signal , ";\n"});
            }
            else if (it->second->src->size > 1) // If the size is greater than one
                WireList.add({SET_INDENT , "wire [" , std::to_string(it->second->src->size - 1) , ":0] " , Signal , ";\n"});
            else // Otherwise, if the size is 1
                WireList.add({SET_INDENT , "wire " , Signal , ";\n"});

            // Assigning that wire to the corresponding input
            int SubmodIndex = FindSubmoduleIndex(it->second->src->parent->name); // Finding the submodule that the destination is connected
            int PortIndex = it->second->src->parent->FindPortIndex(it->second->src->name); // Go to that submodule, and then find the port index of that port name
            buffer << "." << it->second->src->name << "(" << Signal << ")";
            Matrix[SubmodIndex][PortIndex] = buffer.str();
            buffer.str(std::string()); // Clearing the buffer

            // Now, send this wire to all the destination ports
            for(unsigned j = 0; j < it->second->dst.size(); j++) // For all destination ports
            {
                if (it->second->dst[j]->parent == this) // If the destination port is not in a submodule, connect directly to output
                {
                    AssignList.add({SET_INDENT, "assign ", it->second->dst[j]->name, " = ", Signal, ";\n"});
                    continue;
                }

                // Otherwise it is a submodule, and assign its connection appropriately
                SubmodIndex = FindSubmoduleIndex(it->second->dst[j]->parent->name); // Finding the submodule that the destination is connected
                PortIndex = it->second->dst[j]->parent->FindPortIndex(it->second->dst[j]->name); // Go to that submodule, and then find the port index of that port name
                buffer << "." << it->second->dst[j]->name << "(" << Signal << ")";
                Matrix[SubmodIndex][PortIndex] = buffer.str();
                buffer.str(std::string()); // Clearing the buffer
            }
        }
    }

    // ************** END OF WIRING OTHER CONNECTIONS WITHIN MODULE ************** //


    // ************** DEAL WITH DECLARATION OF SUBMODULES (Wiring of them to create necessary config cell chain also handled!) ************** //

    // std::string PreviousWire, ConfigInput, ConfigOutput has been previously declared in above section!
    buffer.str(std::string()); // Clearing the previously declared buffer
    int i = 0; // Loop variable

    if (!submodules.empty()) // If there are submodules, add a comment
        SubmodList.add({"\n", SET_INDENT, "// Declaring the submodules\n"});

    for(std::map<std::string,Module*>::iterator it = submodules.begin(); it != submodules.end(); ++it) // For each submodule, print it and its connections
    {
        const auto& resolved_params = it->second->ResolveVerilogParameters();
        const auto& parameter_string = makeParameterListSyntaxItem(resolved_params);
        SubmodList.add({SET_INDENT, it->second->GenericName(), " ", parameter_string, it->second->name, "(\n"}); // Printing beginning of submodule
        if (moduleRequiresConfigPorts(*it->second)) // If the module has config cells somewhere inside it
        {
            ConfigInput = PreviousWire; // Otherwise, wire the previous module into ConfigIn (to create a chain)
            // Declaring the verilog wire for the config cell chain, and wiring it in
            buffer << it->second->name << "_config";
            ConfigOutput = buffer.str();
            buffer.str(std::string()); // Clearing the buffer
            PreviousWire = ConfigOutput;
            WireList.add({SET_INDENT, "wire ", PreviousWire, ";\n"});

            // Adding the config cell ports in (required if there are config cells)
            SubmodList.add({SET_DOUBLE_INDENT, ".Config_Clock(Config_Clock),\n", SET_DOUBLE_INDENT, ".Config_Reset(Config_Reset),\n", SET_DOUBLE_INDENT, ".ConfigIn(", ConfigInput, "),\n",
                    SET_DOUBLE_INDENT, ".ConfigOut(", ConfigOutput, ")"});
            SubmodList.add({",\n"}); // This needs to be on a seperate line, to indicate a comma was added last (this information is used below when printing the other ports)
        }
        if (moduleRequiresClockPorts(*it->second))
        {
            SubmodList.add({SET_DOUBLE_INDENT, ".CGRA_Clock(CGRA_Clock),\n", SET_DOUBLE_INDENT, ".CGRA_Reset(CGRA_Reset)"});
            SubmodList.add({",\n"}); // This needs to be on a seperate line, to indicate a comma was added last (this information is used below when printing the other ports)
        }
        // Now, print out the remaining ports that the module has
        for(unsigned j = 0; j < Matrix[i].size(); j++)
        {
            SubmodList.add({SET_DOUBLE_INDENT, Matrix[i][j]}); // Add the connection element
            // If the port currently being printed is not the last one...
            if (j+1 != Matrix[i].size())
                SubmodList.add({",\n"}); // Add a comma
        }
        SubmodList.add({");\n"}); // Printing end of submodule
        // Ending the module header
        i++; // Incrementing the index
    }
    // If there were submodules with config cells, we need to assign ConfigOut to complete the chain
    if (PreviousWire != "ConfigIn")
        AssignList.add({SET_INDENT, "assign ConfigOut = ", PreviousWire, ";\n"});

    // ************** END OF DEAL WITH DECLARATION OF SUBMODULES ************** //
}

std::vector<ResolvedVeroligModuleParameter> Module::ResolveVerilogParameters() const
{
    std::vector<ResolvedVeroligModuleParameter> result;

    for (auto& name_and_val : parameterlist) {
        result.emplace_back(ResolvedVeroligModuleParameter{
            name_and_val.first,
            std::to_string(name_and_val.second)
        });
    }

    return result;
}

// Finds the index in the module for a given port name, returns -1 if string is not found (0 is first index)
int Module::FindPortIndex(std::string PortName)
{
    int index = 0;
    for(std::map<std::string,Port*>::iterator it = ports.begin(); it != ports.end(); ++it)
    {
        if (it->second->name == PortName) // Compare the port name with the specified one
            return index; // If port name is found, return index location
        index++;
    }
    return -1; // If the string is not found, return -1
}


// Finds the index in the module for a given submodule name, returns -1 if string is not found (0 is first index)
int Module::FindSubmoduleIndex(std::string SubmoduleName)
{
    int index = 0;
    for(std::map<std::string,Module*>::iterator it = submodules.begin(); it != submodules.end(); ++it)
    {
        if (it->second->name == SubmoduleName) // Compare the port name with the specified one
            return index; // If port name is found, return index location
        index++;
    }
    return -1; // If the string is not found, return -1
}

// Debug information: Printing out module information
void Module::print()
{
    std::cout << this->name << ":\n";
    std::cout << "ports:\n";
    print_ports();
    std::cout << "connections:\n";
    print_connections();
    std::cout << "submodules:\n";
    print_submodules();
}


void Module::print_dot()
{
    // print node info
    std::cout << this->name << ";\n";
    // for all src ports
    for(std::map<Port*,Connection*>::iterator it = connections.begin(); it != connections.end(); ++it)
        // print all edges
        for(unsigned j = 0; j < it->second->dst.size(); j++)
        {
            std::cout << it->first->parent->name << "." << it->first->name << "->" << it->second->dst[j]->parent->name << "." << it->second->dst[j]->name << ";\n";
        }

    // print for all composite submodules
    for(std::map<std::string,Module*>::iterator it = submodules.begin(); it != submodules.end(); ++it)
    {
        it->second->print_dot();
    }
}

void Module::print_ports()
{
    for(std::map<std::string,Port*>::iterator it = ports.begin(); it != ports.end(); ++it)
        std::cout << it->second->name << "\n";

}

void Module::print_connections()
{
    for(std::map<Port*,Connection*>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
        // print all edges
        for(unsigned j = 0; j < it->second->dst.size(); j++)
        {
            std::cout << this->name << "<-" << it->second->dst[j]->parent->name << ";\n";
        }
    }
}

void Module::print_submodules()
{
    // print for all composite submodules
    for(std::map<std::string,Module*>::iterator it = submodules.begin(); it != submodules.end(); ++it)
    {
        std::cout << it->second->name << "\n";
    }
}

void Module::print_configcells()
{
    // print for all composite submodules
    for(std::map<std::string,ConfigCell*>::iterator it = configcells.begin(); it != configcells.end(); ++it)
    {
        std::cout << it->second->name << "\n";
    }
}

// Function that gives the module and port names, given a string
static bool getmoduleport(std::string name, std::string* module, std::string* port)
{
    std::size_t dot_loc;
    if(std::string::npos != (dot_loc = name.find_first_of(".")))
    {
        *module = name.substr(0,dot_loc);
    }
    *port = name.substr(dot_loc + 1);

    return (std::string::npos == port->find_first_of("."));
}

void Module::addConfig(std::string name, std::vector<std::string> ConnectTo, int contexts, bool isElastic) { 
    if (isElastic) {
        addConfig(new ElasticConfigCell(name, contexts), ConnectTo);
    } else {
        addConfig(new ConfigCell(name, contexts), ConnectTo);
    }
}

// Function that adds a config cell to a module
// Takes config cell, and vector of ports (via string name) to connect to
void Module::addConfig(ConfigCell* c, std::vector<std::string> ConnectTo)
{

    if(configcells.find(c->name) != configcells.end()) // If the config cell name already exists
    {
        auto error = make_from_stream<cgrame_model_error>([&](auto&& s) {
            s << "a config cell called " << c->name << " already exists";
        });
        delete c; // Delete the dynamically allocated memory
        throw error;
    }

    std::vector<Port*> new_ports_to_connect_to;
    // Now... find if the ports specified exists
    for (unsigned i = 0; i < ConnectTo.size(); i++) // For all the ports specified
    {
	Module* module = getModuleFromPortName(ConnectTo[i]);
    if (!module){ 
            throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << "Connection ERROR!(" << ConnectTo[i] << "): module and port do not exist\n";
            });
    }
	if (dynamic_cast<const ConfigCell*>(module)) {
	     delete c;
            throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << "Connection ERROR!(" << this->name << "): cannot connect a config cell to another config cell ("<< ConnectTo[i] << ")\n";
            });
	}
	Port* port = getPort(ConnectTo[i]);
        new_ports_to_connect_to.push_back(port);
    }

    c->addControledPorts(new_ports_to_connect_to);
    configcells[c->name] = c; // Add config cell to the list
}

// Function that adds a submodule to the module
void Module::addSubModule(Module* m)
{
    // If the submodule name exists, we do not add it again
    if(submodules[m->name])
    {
        std::stringstream msg;
        msg << "Submodule with same name: " << m->name << " already exists\n";
        delete m;
        throw cgrame_model_error(msg.str());
    }
    // Otherwise, add it
    m->parent = this; // Setting the parent
    submodules[m->name] = m; // Adding the module to list of submodules
}

// Function that adds a submodule to the module with position information
void Module::addSubModule(Module* m, double xPos, double yPos, double width, double height)
{
    addSubModule(m);
    submodule_relative_position[m->name] = {xPos, yPos, width, height};
}

// Returns a pointer to a submodule
Module* Module::getSubModule(std::string m)
{
    // If the submodule name exists, we do not add it again
    if(!submodules[m])
    {
        std::stringstream msg;
        msg << "Could not find submodule: '" << m << "'\n";
        throw cgrame_model_error(msg.str());
    }
    return submodules[m];
}

// Returns a true/false value if this is a submodule of this module
bool Module::isSubModule(Module* subModule) {
    // If the submodule name exists, we do not add it again
    if (submodules.end() != submodules.find(subModule->getName())) {
        return (submodules[subModule->getName()] == subModule);
    }
    return false;
}
Module* Module::getModule(std::string module_name, std::string err_context) {
    // find module and port
    Module* module = NULL;
    if (module_name == "this") {
        module = this;
    } else if (submodules.find(module_name) != submodules.end()) {
        module = submodules[module_name];
    } else if (configcells.find(module_name) != configcells.end()) {
        module = configcells[module_name];
    }
    if (module == NULL) {  // Error with finding module
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << err_context << "\nMODULE NOT FOUND!(" << this->name <<
                 "): module not found ("<< module_name << ")";
                });
    }
    return module;
}

Module* Module::getModuleFromPortName(std::string full_port_name, std::string err_context) {
    std::string module_name;
    std::string port_name;

    // split names
    if (!getmoduleport(full_port_name, &module_name, &port_name)) {
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << err_context <<  "\nMODULE NOT FOUND!(" << this->name <<
                "): incorrect module.port (" << full_port_name << ") name";
                });
    }
    Module* module = getModule(module_name, err_context);
    return module;
}

Port* Module::getPort(std::string full_port_name, std::string err_context) {
    std::string module_name;
    std::string port_name;

    // split names
    if (!getmoduleport(full_port_name, &module_name, &port_name)) {
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << err_context<< "\nPORT NOT FOUND!(" << this->name <<
                "): incorrect module.port (" << full_port_name << ") name";
                });
    }
    // find module 1 and port
    Module* module = getModule(module_name, err_context);
    if (module == NULL) {
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << err_context << "\nmodule not found!(" << this->name <<
                    "): port not found ("<< port_name <<
                    ") in module (" << module_name << ")";
                });
    }
    if (module->ports.find(port_name) == module->ports.end()) {
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << err_context << "\nPORT NOT FOUND!(" << this->name <<
                    "): port not found ("<< port_name <<
                    ") in module (" <<module->name << ")";
                });
    }
    return module->ports[port_name];;
}

// name1 is src, name2 is dst
void Module::connectPorts(std::string name1, std::string name2, bool isElastic) {
    if (isElastic) {
      addElasticConnection(name1, name2);
    } else {
	  addConnection(name1, name2);
    }
}

// name1 is src, name2 is dst
void Module::addConnection(std::string name1, std::string name2, bool isInMRRG)
{
    const auto& print_context = [&](auto&& s) {
        s << "Connection ERROR! Could not make connection \"" << name1
         << "\" to \""<< name2 << "\" within module \"" << this->name  << "\"";
    };

    std::string context = "Connection ERROR! Could not make connection \""
     + name1 + "\" to \"" + name2 + "\" within module \"" + this->name  + "\"";

    Module* module1 = getModuleFromPortName(name1, context);
    Module* module2 = getModuleFromPortName(name2, context);

    Port* port1 = getPort(name1, context);
    Port* port2 = getPort(name2, context);

    // Setting the source and destionation ports
    Port* srcport;  // Source port
    Port* dstport;  // Destination port  
    if (((port1->pt == PORT_OUTPUT || port1->pt == port_type::PORT_OUTPUT_REG)&& module1 != this) ||
        (port1->pt == PORT_INPUT && module1 == this)) { 
        srcport = port1;
    } else if (((port2->pt == PORT_OUTPUT || port2->pt == port_type::PORT_OUTPUT_REG) && module2 != this) || 
                (port2->pt == PORT_INPUT && module2 == this)) { 
        srcport = port2;
    } else{
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                print_context(s);
                s << "\nConnection ERROR!(" << this->name << "): source port could not be found. port 1 ("<< name1 << ") or port 2 ("<< name2 << ") are wrong";
                });

    }
    
    if ((port1->pt == PORT_INPUT && module1 != this) || 
        ((port1->pt == PORT_OUTPUT || port1->pt == port_type::PORT_OUTPUT_REG) && module1 == this)) { 
        dstport = port1;
    } else if ((port2->pt == PORT_INPUT && module2 != this) ||
         ((port2->pt == PORT_OUTPUT || port2->pt == port_type::PORT_OUTPUT_REG) && module2 == this)) { 
        dstport = port2;
    }else {
        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                print_context(s);
                s << "\nConnection ERROR!(" << this->name << "): destination port could not be found. port 1 ("<< name1 << ") or port 2 ("<< name2 <<") are wrong";
                });

    } 

    if (port1->size != port2->size){
         throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                print_context(s);
                s << "\nConnection ERROR!(" << this->name << "): source port and destination port do not have the same size. port 1 ("<< name1 << ") size 1 ("<<port1->size<<") and port 2 ("<< name2 << ") size 2 ("<<port2->size<<") ";
                });
    }

    // Make sure that the destination port is not already connected to anything
    for(auto & c : connections)
    {
        for(auto & d : c.second->dst)
        {
            if(d == dstport)
            {
                throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                    print_context(s);
                    s << "\nConnection ERROR! In module (" << this->name << "): destination(" << name2 << ") destination port ( "<<dstport->name << " ) already connected to (" << c.second->src->parent->name << "." << c.second->src->name <<")" ;
                });
            }
        }
    }

    if(!connections[srcport]) // If there is no connection object for the given source port, make one
    {
        connections[srcport] = new Connection();
        connections[srcport]->src = srcport;
    }
    else
    {
        // if we already have a srcport entry, check that we are not adding a previous dst
        for(auto & d: connections[srcport]->dst)
        {
            if(d == dstport)
                throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                    print_context(s); s << "\nConnection ERROR! destination already added";
                });
        }
    }

    // Add the connection
    connections[srcport]->dst.push_back(dstport);

    // Set the flag that specifies whether the source port is in the MRRG
    connections[srcport]->isInMRRG = isInMRRG;

    // If debug mode is on, print out connection information
    if (MODULE_DEBUG_PRINTING)
        std::cout << "New Connection:" << connections[srcport]->src->parent->name << "."
            << connections[srcport]->src->name << "->"
            << connections[srcport]->dst.back()->parent->name << "."
            << connections[srcport]->dst.back()->name << "\n";
}

void Module::addPort(std::string portname, port_type pt, unsigned size, bool isElastic)
{
    if (isElastic) {
        addElasticPort(portname, pt, size);
    } else {
        if (pt == PORT_CONFIG) {
            pt = PORT_INPUT;
	    }
        addPort(portname, pt, size);
    }

}

void Module::addPort(std::string portname, port_type pt, unsigned size)
{
    if(ports[portname]) // If the port already exists
    {
        std::stringstream msg;
        msg << "Port " << portname << " already exists\n";
        throw cgrame_model_error(msg.str());
    }

    // Assigning port parameters
    Port* p = new Port(); // Create port
    p->name = portname; // Assign name
    p->pt = pt; // Assign port type
    p->size = size; // Assign port size
    p->parent = this; // Connect port to its parent
    ports[portname] = p; // Adding port to list

    if (MODULE_DEBUG_PRINTING) // Debug information
        std::cout << this->name << ": added port - " << p->name << "\n";
}

void Module::addPort(std::string portname, port_type pt, std::string parameterName, unsigned size, bool isElastic)
{
    if (isElastic) { 
        addElasticPort(portname, pt, parameterName, size);
    } else {
        if (pt == PORT_CONFIG) {
            pt = PORT_INPUT;
	}
        addPort(portname, pt, parameterName, size);
    }

}

void Module::addPort(std::string portname, port_type pt, std::string ParameterName, unsigned size)
{
    if(ports[portname]) // If the port already exists
    {
        std::stringstream msg;
        msg << "Port " << portname << " already exists\n";
        throw cgrame_model_error(msg.str());
    }

    // Assigning port parameters
    Port* p = new Port(); // Create port
    p->name = portname; // Assign name
    p->pt = pt; // Assign port type
    p->size = size; // PARAMETERIZED = 0: it's a flag to say that the port is parameterized
    p->parameter = ParameterName; // Set the parameter name
    p->parent = this; // Connect port to parent
    ports[portname] = p; // Add port to list
    parameterlist[ParameterName] = size; // Setting the default size of the parameter

    if (MODULE_DEBUG_PRINTING) // Debug information
        std::cout << this->name << ": added port - " << p->name << p->size << "\n";
}

void Module::addElasticPort(std::string portname, port_type pt, std::string parameterName, unsigned size)
{
    std::string stream;
    port_type stop_port;
    port_type valid_port;
    if (pt == port_type::PORT_INPUT){
        stop_port = PORT_OUTPUT_REG;	
        valid_port = PORT_INPUT;
        stream = "upstream";
    } else if (pt == port_type::PORT_OUTPUT || pt == port_type::PORT_OUTPUT_REG){
        stop_port = PORT_INPUT;
        valid_port = pt;
        stream = "downstream";
    } else if (pt == port_type::PORT_CONFIG) {
        stop_port = PORT_OUTPUT;
        valid_port = PORT_OUTPUT;
        pt = PORT_INPUT;
        stream = "upstream";
    } else {
        std::stringstream msg;
        msg << "Port type is not supported for elastic\n";
        throw cgrame_model_error(msg.str());
    }

    addPort(portname, pt, parameterName, size);
    addPort(portname + "_valid_" + stream, valid_port, 1);
    addPort(portname + "_stop_" + stream, stop_port, 1);
}

void Module::addElasticPort(std::string portname, port_type pt, unsigned size)
{
    std::string stream;
    port_type stop_port;
    port_type valid_port;
    if (pt == port_type::PORT_INPUT){
        stop_port = PORT_OUTPUT_REG;	
        valid_port = PORT_INPUT;
        stream = "upstream";
    } else if (pt == port_type::PORT_OUTPUT || pt == port_type::PORT_OUTPUT_REG){
        stop_port = PORT_INPUT;
        valid_port = pt;
        stream = "downstream";
    } else if (pt == port_type::PORT_CONFIG) {
        stop_port = PORT_OUTPUT;
        valid_port = PORT_OUTPUT;
        pt = PORT_INPUT;
        stream = "upstream";
    } else {
        std::stringstream msg;
        msg << "Port type is not supported for elastic\n";
        throw cgrame_model_error(msg.str());
    }

    addPort(portname, pt, size);
    addPort(portname + "_valid_" + stream, valid_port, 1);
    addPort(portname + "_stop_" + stream, stop_port, 1);
}

void Module::addElasticConnection(std::string name1, std::string name2) {
    Port* port1 = getPort(name1);
    Port* port2 = getPort(name2);


    std::string stream1;
    if (port1->pt == port_type::PORT_INPUT){
        stream1 = "upstream";
    } else if (port1->pt == port_type::PORT_OUTPUT || port1->pt == port_type::PORT_OUTPUT_REG){
        stream1 = "downstream";
    } else {
          throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << "\nPORT NOT FOUND!(" << this->name << "): incorrect module.port (" << name1 << ")";
                });
    }

    
    std::string stream2;
    if (port2->pt == port_type::PORT_INPUT){
        stream2 = "upstream";
    } else if (port2->pt == port_type::PORT_OUTPUT || port2->pt == port_type::PORT_OUTPUT_REG){
        stream2 = "downstream";
    } else {
          throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                s << "\nPORT NOT FOUND!(" << this->name << "): incorrect module.port (" << name2 << ")";
                });
    }


    addConnection(name1, name2);
    addConnection(name1 + "_valid_" + stream1, name2 + "_valid_" + stream2, false);
    addConnection(name2 + "_stop_" + stream2, name1 + "_stop_" + stream1, false);
}

void Module::addParameter(std::string parameterName, unsigned parameterValue)
{
    if(parameterlist[parameterName]) // If the port already exists
    {
        std::stringstream msg;
        msg << "Parameter " << parameterName << " already exists\n";
        throw cgrame_model_error(msg.str());
    }

    parameterlist[parameterName] = parameterValue;
}

void Module::addVerilogPort(std::string name, port_type pt, std::string parameter, unsigned size) {
    portsToPropagate.emplace_back(Port{name, pt, parameter, size, this});
}


MRRG* Module::createMRRG(unsigned II)
{
    std::map<std::string, std::unique_ptr<MRRG>> subMRRGs;
    // create MRRG for all sub modules
    for(auto& name_and_submod : submodules)
    {
        auto temp = std::unique_ptr<MRRG>(name_and_submod.second->createMRRG(II));
        subMRRGs[name_and_submod.first] = std::move(temp);
    }

    // create current MRRG with port nodes
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    // for all ports, create and add node to MRRG
    int portNumber = 0;
    for(auto& name_and_port : ports)
    {
        for(unsigned i = 0; i < II; i++)
        {
            result.insert(MRRGNode::make_routing(this, this->getSize(), i, name_and_port.first));

            if (node_relative_position.find(name_and_port.first) == node_relative_position.end()) {
                node_relative_position.insert({name_and_port.first, {0.9, static_cast<double>(portNumber+0.5)/(ports.size()*2)}});
                portNumber++;
            }
        }
    }
    // also add all subMRRG nodes to this graph
    for(auto& submod_name_and_mrrg : subMRRGs)
    {
        for(auto& nodes_in_cycle : submod_name_and_mrrg.second->nodes)
        {
            for(auto& name_and_node : nodes_in_cycle)
            {
                // Copying MRRGNodes is unsupported ATM, so migrate all the nodes over
                // so the fanout, fanin, and operation pointer lists still work
                // Need to change the name to something unique, though
                auto& node = submod_name_and_mrrg.second->getNodeRef(name_and_node.second.get());
                node.name = submod_name_and_mrrg.first + '.' + node.name;
                result.nodes[node.getContextNum()][node.name] = std::move(name_and_node.second);
            }
        }
    }

    // make all connections
    // for all src ports
    for(auto& port_and_connection : connections)
    {
        const auto& port = *port_and_connection.first;
        const auto& connection = *port_and_connection.second;
        // TODO: this is hacky .... need to look at verilog generation to simplify the number of port types we have
        if(port.pt != PORT_INPUT && port.pt != PORT_OUTPUT && port.pt != PORT_OUTPUT_REG && port.pt != PORT_BIDIR)
            continue;

        const auto& src_module_name = port.parent->name;
        const auto& src_module_port_name = port.name;

        // Check if the ports that are being connected are actually in the MRRG, if they aren't don't connect them
        if (!connection.isInMRRG)
            continue;

        // connect to all destinations
        for(unsigned j = 0; j < connection.dst.size(); j++)
        {
            const auto& dst_module_name = connection.dst[j]->parent->name;
            const auto& dst_module_port_name = connection.dst[j]->name;

            for(unsigned k = 0; k < II; k++)
            {
                if (this->isElastic){
                    for (unsigned l = 0; l < II; l++){
                        const auto& src_ndesc = result.getNode(k, ( src_module_name == name ? "" : src_module_name + '.' ) + src_module_port_name);
                        const auto& dst_ndesc = result.getNode(l, ( dst_module_name == name ? "" : dst_module_name + '.' ) + dst_module_port_name);
                        if (!src_ndesc) {
                            throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                                s << "MRRG port not found for port (" << src_module_port_name<< ") in  MODULE (" << src_module_name << ")";
                            });
                        }
                        if (!dst_ndesc) {
                            throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                                s << "MRRG port not found for port (" << dst_module_port_name<< ") in  MODULE (" << dst_module_name << ")";
                            });
                        }    
                        result.link(src_ndesc, dst_ndesc);        
                    }
                }else {
                    const auto& src_ndesc = result.getNode(k, ( src_module_name == name ? "" : src_module_name + '.' ) + src_module_port_name);
                    const auto& dst_ndesc = result.getNode(k, ( dst_module_name == name ? "" : dst_module_name + '.' ) + dst_module_port_name);
                    if (!src_ndesc) {
                        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                            s << "MRRG port not found for port (" << src_module_port_name<< ") in  MODULE (" << src_module_name << ")";
                        });
                    }
                    if (!dst_ndesc) {
                        throw make_from_stream<cgrame_model_error>([&](auto&& s) {
                            s << "MRRG port not found for port (" << dst_module_port_name<< ") in  MODULE (" << dst_module_name << ")";
                        });
                    }
                    result.link(src_ndesc, dst_ndesc);
                }
            }
        }
    }

    return result_ptr;
}


std::pair<bool, std::pair<double, double>> Module::getSubModulePosition(const std::string & submodName) {
    if (name != submodName && parent != nullptr) {
        return parent->getSubModulePosition(submodName);
    }
    else if (parent == nullptr) {
        return {false, {}};
    }
    // std::cout << "In submodule " << name << " looking for " << submodName << "\n";   // Debugging

    float xPos, yPos;
    std::string cName = submodName;
    Module* cParent = parent;

    VisualPositionRect rel_pos;

    if (cParent->submodule_relative_position.find(cName) == cParent->submodule_relative_position.end()) {
        return{false, {}};
    }
    rel_pos = cParent->submodule_relative_position.at(cName); // This should not throw an exception

    // Put module in the top left corner of its box"
    xPos = rel_pos.x;
    yPos = rel_pos.y;

    cName = cParent->name;
    cParent = cParent->parent;
    while (cParent != nullptr) {
        if (cParent->submodule_relative_position.find(cName) ==     cParent->submodule_relative_position.end()) {
            return{false, {}};
        }

        rel_pos = cParent->submodule_relative_position.at(cName); // This should not throw an exception

        xPos = rel_pos.x + xPos*rel_pos.w;
        yPos = rel_pos.y + yPos*rel_pos.h;

        cName = cParent->name;
        cParent = cParent->parent;
    }
    return {true, {xPos, yPos}};
}

bool Module::setNodePosition(const std::string & nodeName, double x, double y) {
    node_relative_position.insert({nodeName,{x, y}});

    return true;
}


std::pair<bool, std::pair<double, double>> Module::getNodePosition(const std::string & nodeName) {
    // std::cout << "In submodule " << name << " looking for node" << nodeName << "\n"; // Debugging

    double xPos, yPos;
    if (node_relative_position.find(nodeName) == node_relative_position.end()) {
        return {false, {-1, -1}};
    }

    const auto node_pos = node_relative_position.at(nodeName);

    // Put module in the center of its "box"
    xPos = node_pos.x;
    yPos = node_pos.y;

    std::string cName = name;
    Module* cParent = parent;
    while (cParent != nullptr) {
        if (cParent->submodule_relative_position.find(cName) ==     cParent->submodule_relative_position.end()) {
            return{false, {-1, -1}};
        }
        const auto rel_pos = cParent->submodule_relative_position.at(cName); // This should not throw an exception

        xPos = rel_pos.x + xPos*rel_pos.w;
        yPos = rel_pos.y + yPos*rel_pos.h;

        cName = cParent->name;
        cParent = cParent->parent;
    }
    return {true, {xPos, yPos}};
}

/************ TriState **********/
TriState::TriState(Mode mode, std::string name, Location loc, unsigned size, bool isElastic)
    : Module(name, loc, size, isElastic)
    , mode(mode)
{
    // adding ports
    addPort("enable", PORT_CONFIG, 1, isElastic);
    addPort("in", PORT_INPUT, "size", size, isElastic);
    addPort("out", PORT_OUTPUT,"size", size, isElastic);

    addVerilogPort("bidir_in", PORT_INPUT, "size", size);
    addVerilogPort("bidir_out", PORT_OUTPUT, "size", size);
    if(isElastic) {
        addVerilogPort("bidir_in_valid_upstream", PORT_INPUT, "", 1);
        addVerilogPort("bidir_in_stop_upstream", PORT_OUTPUT, "", 1);
        addVerilogPort("bidir_out_valid_downstream", PORT_OUTPUT, "", 1);
        addVerilogPort("bidir_out_stop_downstream", PORT_INPUT, "", 1);
    }
    node_relative_position.insert({"io", {0.5, 0}});
    node_relative_position.insert({"in", {0.5, 0.33}});
}

void TriState::GenFunctionality()
{
    std::cout << SET_INDENT << "assign out = enable ? in : {size{1'bZ}};\n";
    std::cout << SET_INDENT << "assign bidir_out = out;\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json TriState::CoreIRGenFunctionality() // Virtual function override
{
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist) {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = makeCoreIRInterface(ports, portsToPropagate);

    // Module Definition
    std::string moduleDefinition = makePortsDeclaration(ports, portsToPropagate, SET_INDENT);

    if (isElastic) { 
        moduleDefinition += std::string(SET_INDENT) + "assign out = enable ? in : bidir_in;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign out_valid_downstream = enable ? in_valid_upstream : bidir_in_valid_upstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign in_stop_upstream = enable ? bidir_out_stop_downstream :  out_stop_downstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign bidir_out = out;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign bidir_out_valid_downstream = out_valid_downstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign bidir_in_stop_upstream = in_stop_upstream;\n";
    } else {
        moduleDefinition += std::string(SET_INDENT) + "assign out = enable ? in : bidir_in;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign bidir_out = out;";
    }
    vjson["definition"] = moduleDefinition;

    return vjson;
}

BitConfig TriState::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const std::map<OpGraphOp*,std::set<MRRG::NodeDescriptor>>& mrrg_nodes_from_op_node,
    const std::map<OpGraphVal*, std::set<MRRG::NodeDescriptor>>& mrrg_nodes_from_val_node
) const {
    const std::regex in_regex("\\.in$");
    const std::regex out_regex("\\.out$");
    (void)ccell;
    (void)mrrg_nodes_from_op_node;

    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            while (mrrg_node->cycle >= valNodesByCycle.size()) {
                valNodesByCycle.push_back({});
            }
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    int used_cycles = 0;
    for (auto & mrrg_nodes_from_val_by_cycle : valNodesByCycle) {
        if (mrrg_nodes_from_val_by_cycle.empty()) {
            bitConfig.add( {BitSetting::LOW}, cycle);
        } else if (mrrg_nodes_from_val_by_cycle.size() != 1) {
            throw cgrame_error("expect one val node");
        } else {
            // const auto& val_node = begin(mrrg_nodes_from_val_node)->first;
            const auto& mrrg_nodes = begin(mrrg_nodes_from_val_by_cycle)->second;
            if (mrrg_nodes.size() != 1) {
                throw cgrame_error("expect one MRRG node");
            } else {
                const auto& mrrg_node = **begin(mrrg_nodes);
                const auto& name = mrrg_node.getHierarchyQualifiedName();
                if (regex_search(name, in_regex)) {
                    used_cycles++;
                    bitConfig.add( {BitSetting::HIGH}, cycle);
                } else if (regex_search(name, out_regex)) {
                    used_cycles++;
                    bitConfig.add( {BitSetting::LOW}, cycle);
                } else {
                    throw cgrame_error("unexpected MRRG node");
                }
            }
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

MRRG* TriState::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;
    for(unsigned i = 0; i < II; i++)
    {
        // create nodes
        MRRG::NodeDescriptor io = result.insert([&]() {
            if (mode == Mode::PROVIDES_IO_OP) {
                return MRRGNode::make_function(this, data_size, i, "io", 0,
                    {OpCode::INPUT, OpCode::OUTPUT}
                );
            } else if (mode == Mode::PROVIDES_IO_PRED_OP) {
                return MRRGNode::make_function(this, data_size, i, "io", 0,
                    {OpCode::INPUT_PRED, OpCode::OUTPUT_PRED}
                );
            } else  {
                return MRRGNode::make_routing(this, data_size, i, "io");
            }
        }()).first;
        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;



        MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in")).first;

        // in is an input into the io block, which will be output from the CGRA
        // out is an output from the io block, which will be an input to the CGRA
        result.link(in, io);
        result.link(io, out);
    }

    return result_ptr;
}

std::string TriState::GenericName()
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "tristate_" + std::to_string(getSize()) + "b";
}

TriState::~TriState()
{
}


/************ IO **********/
IO::IO(std::string name, Location loc, unsigned size, int contexts, bool isElastic)
: Module(name, loc, size, isElastic)
{
    // adding ports
    addPort("in", PORT_INPUT, size, isElastic);
    addPort("out", PORT_OUTPUT, size, isElastic);
    if (contexts > 1) 
        addPort("Context", PORT_INPUT, ceil(log2(contexts)));

    node_relative_position.insert({"in", {0, 0}});
    node_relative_position.insert({"out", {0, 0.67}});

    addSubModule(new Register("reg_in", loc, size, isElastic), 0.1, 0.2, 0.9, 0.33);
    addSubModule(new Register("reg_out", loc, size, isElastic), 0.1, 0.5, 0.9, 0.33);
    addSubModule(new TriState(((size == 1)?TriState::Mode::PROVIDES_IO_PRED_OP : TriState::Mode::PROVIDES_IO_OP), "IOPin", loc, size, isElastic), 0.1, 0.33, 0.9, 0.33);

    addConfig("IOPinConfig", {"IOPin.enable"}, contexts, isElastic);
    addConfig("RegInConfig", {"reg_in.enable"}, contexts, isElastic);
    addConfig("RegOutConfig", {"reg_out.enable"}, contexts, isElastic);
    if (contexts > 1) {
        addConnection("this.Context", "IOPinConfig.Context", false);
        addConnection("this.Context", "RegInConfig.Context", false);
        addConnection("this.Context", "RegOutConfig.Context", false);
    }

    connectPorts("this.in", "reg_in.in", isElastic);
    connectPorts("reg_in.out", "IOPin.in", isElastic);
    connectPorts("IOPin.out", "reg_out.in", isElastic);
    connectPorts("reg_out.out", "this.out", isElastic);
}

std::string IO::GenericName()
{
    return "io_" + std::to_string(getSize()) + "b";
}

IO::~IO()
{
}

DisconnectedSource::DisconnectedSource(std::string name, Location loc, unsigned size, int value)
    : Module(name, loc, size)
    , value(value)
{
    addPort("out", PORT_OUTPUT, "size", size);
}

std::string DisconnectedSource::GenericName() { return "disconnected_source_" + std::to_string(getSize()) + "b" + std::to_string(value); }
DisconnectedSource::~DisconnectedSource() { }

void DisconnectedSource::GenFunctionality() {
    std::cout << SET_INDENT << "assign out = " << getSize() << "'d" << value << ";\n";
}

nlohmann::json DisconnectedSource::CoreIRGenFunctionality() {
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist) {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = makeCoreIRInterface(ports, portsToPropagate);

    // Module Definition
    std::string moduleDefinition = makePortsDeclaration(ports, portsToPropagate, SET_INDENT);
    moduleDefinition += SET_INDENT;
    moduleDefinition += "assign out = " + std::to_string(getSize()) + "'d" + std::to_string(value) + ";\n";
    vjson["definition"] = moduleDefinition;
    return vjson;
}

DisconnectedSink::DisconnectedSink(std::string name, Location loc, unsigned size)
    : Module(name, loc, size)
{
    addPort("in", PORT_INPUT, "size", size);
}

std::string DisconnectedSink::GenericName() { return "disconnected_sink_" + std::to_string(getSize()) + "b"; }
DisconnectedSink::~DisconnectedSink() { }

void DisconnectedSink::GenFunctionality() {
    std::cout << SET_INDENT << "// purposefully left blank\n";
}

nlohmann::json DisconnectedSink::CoreIRGenFunctionality() {
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist) {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = makeCoreIRInterface(ports, portsToPropagate);

    // Module Definition
    std::string moduleDefinition = makePortsDeclaration(ports, portsToPropagate, SET_INDENT);
    moduleDefinition += SET_INDENT;
    moduleDefinition += "// purposefully left blank\n";
    vjson["definition"] = moduleDefinition;
    return vjson;
}

/************ CustomModule **********/
CustomModule::CustomModule(std::string name, Location loc, std::vector<std::string> Function, unsigned size, std::string pred)
    : Module(name, loc, size)
    , Function(Function)
{
    // Create ports
    addPort("a", PORT_INPUT, "size", size);
    addPort("b", PORT_INPUT, "size", size);
    addPort("c", PORT_OUTPUT, "size", size);    
    if(!pred.empty()){
        addPort(pred, PORT_INPUT, 1);
    }
} 

// Virtual function that overrides Module::GetFunctionality. Generates the functionality for CustomModule
void CustomModule::GenFunctionality()
{
    for (unsigned i = 0; i < Function.size(); i++)
        std::cout << SET_INDENT << Function[i] << "\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json CustomModule::CoreIRGenFunctionality() // Virtual function override
{
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist) {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
    for (auto& port : ports) {
        if (port.second->pt == port_type::PORT_BIDIR) {
            const auto ioport_copies = splitBidirPort(*port.second);
            vjson["interface"].push_back(ioport_copies.first.name);
            vjson["interface"].push_back(ioport_copies.second.name);
        } else {
            std::string portName = port.second->getName();
            vjson["interface"].push_back(portName);
        }
    }

    // Module Definition
    std::string moduleDefinition;
    for (auto& port : ports) {
        moduleDefinition += makePortDeclaration(*port.second, SET_INDENT);
    }

    // Functional Definition
    for (unsigned i = 0; i < Function.size(); i++)
    {
        moduleDefinition += std::string(SET_INDENT) + Function[i] + "\n";
    }
    vjson["definition"] = moduleDefinition;
    return vjson;
}

std::string CustomModule::GenericName()
{
    return name;
}

CustomModule::~CustomModule()
{
}

/************ CustomModuleSingleInput **********/
CustomModuleSingleInput::CustomModuleSingleInput(std::string name, Location loc, std::vector<std::string> Function, unsigned size, std::string pred)
    : Module(name, loc, size)
    , Function(Function)
{
    // Create ports
    addPort("a", PORT_INPUT, "size", size);
    addPort("c", PORT_OUTPUT, "size", size);
    if(!pred.empty()){
        addPort(pred, PORT_INPUT, 1);
    }
}

// Virtual function that overrides Module::GetFunctionality. Generates the functionality for CustomModuleSingleInput
void CustomModuleSingleInput::GenFunctionality()
{
    for (unsigned i = 0; i < Function.size(); i++)
        std::cout << SET_INDENT << Function[i] << "\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json CustomModuleSingleInput::CoreIRGenFunctionality() // Virtual function override
{
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist) {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
    for (auto& port : ports) {
        if (port.second->pt == port_type::PORT_BIDIR) {
            const auto ioport_copies = splitBidirPort(*port.second);
            vjson["interface"].push_back(ioport_copies.first.name);
            vjson["interface"].push_back(ioport_copies.second.name);
        } else {
            std::string portName = port.second->getName();
            vjson["interface"].push_back(portName);
        }
    }

    // Module Definition
    std::string moduleDefinition;
    for (auto& port : ports) {
        moduleDefinition += makePortDeclaration(*port.second, SET_INDENT);
    }

    // Functional Definition
    for (unsigned i = 0; i < Function.size(); i++)
    {
        moduleDefinition += std::string(SET_INDENT) + Function[i] + "\n";
    }
    vjson["definition"] = moduleDefinition;
    return vjson;
}

std::string CustomModuleSingleInput::GenericName()
{
    return name;
}

CustomModuleSingleInput::~CustomModuleSingleInput()
{
}

/************ UserModule **********/
UserModule::UserModule(std::string prototype, std::string name, Location loc, std::vector<std::string> Ports)
    : Module(name, loc)
    , prototype(prototype)
{

    // adding ports
    for (unsigned i = 0; i < Ports.size(); i++)
    {
        addPort(Ports[i], PORT_UNSPECIFIED, DEFAULT_SIZE); // We do not care if the port is input or output
    }

}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string UserModule::GenericName() // Virtual function override
{
    return prototype;
}

UserModule::~UserModule()
{
}

/************ ConfigCell **********/
ConfigCell::ConfigCell(std::string name, int contexts, Location loc)
    : Module(name, loc, 0)
    , connected_ports()
    , l_contexts(contexts)
{
    parameterlist.emplace("size", getSize());
    addPort("select", port_type::PORT_OUTPUT, "size", PARAMETERIZED);
    if (contexts > 1) addPort("Context", PORT_INPUT, ceil(log2(contexts)));
    adds_synchronous_circuitry = true;
}

// Prints out the Verilog code for the config cell
void ConfigCell::GenModuleVerilog()
{
    std::string cellSize = std::to_string(getStorageSize());
    // Module header
    std::cout << "module configCell_" + cellSize + "b(ConfigIn, ConfigOut, Config_Clock, Config_Reset, select);\n";
    // Parameter specification
    std::cout << "    parameter size = " + cellSize + ";\n";
    // Port specifications
    std::cout << "    input Config_Clock, Config_Reset, ConfigIn;\n";
    std::cout << "    output ConfigOut;\n";
    std::cout << "    output [size-1:0] select;\n";
    // Functionality
    std::cout << "    reg [size-1:0] temp;\n\n";
    std::cout << "    always @(posedge Config_Clock, posedge Config_Reset)\n";
    std::cout << "        if (Config_Reset)\n";
    std::cout << "            temp = 0;\n";
    std::cout << "        else\n";
    std::cout << "        begin\n";
    std::cout << "            temp = temp >> 1;\n";
    std::cout << "            temp[size-1] = ConfigIn;\n";
    std::cout << "        end\n";
    std::cout << "    assign select = temp;\n";
    std::cout << "    assign ConfigOut = temp[0];\n";
    std::cout << "endmodule\n";
}

void ConfigCell::addControledPorts(const std::vector<Port*>& new_ports) {

    const auto port_is_new = [&](auto&& port) { auto& cp = connected_ports; return cp.end() == std::find(cp.begin(), cp.end(), port); };
    std::copy_if(new_ports.begin(), new_ports.end(), std::back_inserter(connected_ports), port_is_new);

    for (auto& port : connected_ports) {
        int port_size = port->size;
        if (port->size == PARAMETERIZED) {
            port_size = port->parent->parameterlist.at(port->parameter);
        }
        data_size = std::max(getSize(), port_size);
    }
    parameterlist["size"] = getSize(); // manually sync the param with data_size
}

nlohmann::json ConfigCell::CoreIRGenFunctionality()
{
    auto context_size = (parameterlist["contexts"] == 1) ? "0" : "$clog2(contexts)-1";
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
    vjson["interface"].push_back("ConfigIn");
    vjson["interface"].push_back("ConfigOut");
    vjson["interface"].push_back("Config_Clock");
    vjson["interface"].push_back("Config_Reset");
    vjson["interface"].push_back("CGRA_Clock");
    vjson["interface"].push_back("CGRA_Reset");
    vjson["interface"].push_back("CGRA_Enable");
    if (l_contexts > 1) vjson["interface"].push_back("Context");
    vjson["interface"].push_back("select");

    // module definition
    std::string moduleDefinition;
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Enable;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input ConfigIn;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input Config_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input Config_Reset;\n";
   if (l_contexts > 1)  moduleDefinition += std::string(SET_INDENT) +    "input [" + context_size + ":0] Context;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output ConfigOut;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output [size-1:0] select;\n";
    moduleDefinition += std::string(SET_INDENT) +    "reg [size-1:0] config_reg [contexts-1:0];\n\n";
    moduleDefinition += std::string(SET_INDENT) +    "reg [contexts-1:0] context_counter = {size{1'b0}};\n\n";
    moduleDefinition += std::string(SET_INDENT) +    "integer i;\n\n";

    moduleDefinition += std::string(SET_INDENT) + "always @(posedge Config_Clock, posedge Config_Reset)\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (Config_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "for (i = 0; i < contexts; i = i+1) begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "config_reg[i] <= 'd0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "for (i = 0; i < contexts; i = i+1) begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "if(i == 0) begin\n";

    if (parameterlist["size"] > 1) {
        moduleDefinition += std::string(SET_PENTA_INDENT) +         "config_reg[i] <= {ConfigIn,config_reg[i][size-1:1]};\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) +      "end\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) +      "else begin\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +         "config_reg[i] <= {config_reg[i-1][0],config_reg[i][size-1:1]};\n";
    }
    else {
        moduleDefinition += std::string(SET_PENTA_INDENT) +         "config_reg[i] <= ConfigIn;\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) +      "end\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) +      "else begin\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +         "config_reg[i] <= config_reg[i-1];\n";
    }

    moduleDefinition += std::string(SET_QUAD_INDENT) +          "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n\n";
    if (l_contexts <= 1) {
        moduleDefinition += std::string(SET_INDENT) +    "assign select = config_reg[0];\n";
        moduleDefinition += std::string(SET_INDENT) +    "assign ConfigOut = config_reg[contexts-1][0];";
    } else {
        moduleDefinition += std::string(SET_INDENT) +    "assign select = config_reg[Context];\n";
        moduleDefinition += std::string(SET_INDENT) +    "assign ConfigOut = config_reg[contexts-1][0];";
    }

    vjson["definition"] = moduleDefinition;
    return vjson;
}

std::string ConfigCell::GenericName()
{
    return "configcell_context_II_" + std::to_string(l_contexts) + "_" + std::to_string((contexts.size() > 0) ? contexts.size() : 1) + "_size" + std::to_string(getStorageSize());
}
/************ ContextCounter **********/
/* Context counters are binary counters present in PEs, IOs, MemPorts and RegisterFiles to count the context for
 * multi-context support in CGRAs. The Context port with the context count is connected to the configuration cells in
 * the tile. Having separate counters in each tile in the CGRA counting in tandem reduces the fan-out of the counter
 * when compared to having a single counter in the CGRA connected to all configuration cells in the CGRA*/
ContextCounter::ContextCounter(std::string name, int contexts, Location loc)
        : Module(name, loc, 0)
{
    addPort("Context_Used", PORT_INPUT, ceil(log2(contexts)));
    addPort("Context", PORT_OUTPUT_REG, ceil(log2(contexts)));  
    parameterlist.emplace("size", getSize());
    parameterlist.emplace("contexts", contexts);
    adds_synchronous_circuitry = true;
}

nlohmann::json ContextCounter::CoreIRGenFunctionality()
{
    auto context_size = (parameterlist["contexts"] == 1) ? "0" : "$clog2(contexts)-1";
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
    vjson["interface"].push_back("CGRA_Clock");
    vjson["interface"].push_back("CGRA_Reset");
    vjson["interface"].push_back("CGRA_Enable");
    vjson["interface"].push_back("Context_Used");
    vjson["interface"].push_back("Context");

    // module definition
    std::string moduleDefinition;
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Enable;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input [" + context_size + ":0] Context_Used;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output reg [" + context_size + ":0] Context;\n\n";
    moduleDefinition += std::string(SET_INDENT) +    "integer i;\n\n";

    moduleDefinition += std::string(SET_INDENT) +    "always @(posedge CGRA_Clock, posedge CGRA_Reset)\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "Context <= 'd0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (CGRA_Enable) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "if (Context < Context_Used) begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "Context <= (Context+1);\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "else begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "Context <= 'd0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

std::string ContextCounter::GenericName()
{
    return "contextcounter";
}

/************ ContextCell **********/
/* ContextCell stores the number of contexts to be used by the CGRA (i.e. initiation interval, II). So, a CGRA
 * with supported number of contexts, contexts, can be mapped with an application to work in any number of contexts
 * 1<=II<=contexts. The II of the mapped application is configured into the CGRA with the configuration bitstream
 * and is stored in ContextCell. The Context_Used port is connected to all ContextCounters in the CGRA tiles. */
ContextCell::ContextCell(std::string name, int contexts, Location loc)
        : Module(name, loc, 0),
        l_contexts(contexts)
{
    addPort("Context_Used", PORT_OUTPUT, ceil(log2(contexts)));
    parameterlist.emplace("size", getSize());
    parameterlist.emplace("contexts", contexts);
    addConfig(new ConfigCell("ContextsUsed", 1), {"this.Context_Used"});
}

BitConfig ContextCell::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;

    BitConfig bitConfig(1);
    std::vector<BitSetting> result = bitsettings_from_int(mrrg.initiationInterval() - 1, ceil(log2(l_contexts)));
    bitConfig.add(result, 0);

    return bitConfig;
}
std::string ContextCell::GenericName()
{
    return "contextcell";
}

/************ Extractor ***********/
static const std::map<std::string, port_type> port_type_map =
{
    {"input", PORT_INPUT},
    {"output", PORT_OUTPUT},
    {"output-reg", PORT_OUTPUT_REG},
    {"bidir", PORT_BIDIR},
    {"unspecified", PORT_UNSPECIFIED},
};

std::istream& operator >>(std::istream &is, port_type &type)
{
    std::string str;
    is >> str;
    if (port_type_map.count(str) > 0)
        type = port_type_map.at(str);
    else
        is.setstate(std::ios_base::failbit);

    return is;
}
