/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/CGRA.h>
#include <CGRA/Control.h>
#include <CGRA/ModuleComposites.h>
#include <CGRA/ModuleElastic.h>

#include <coreir.h>
#include <coreir/passes/analysis/verilog.h>

#include <algorithm>
#include <fstream>

class PropagatePorts : public CoreIR::InstanceGraphPass {
    public:
        //The static keyword is important here!
        static std::string ID;
        PropagatePorts() : InstanceGraphPass(ID, "Propagate special ports from bottom module to top"), modulesToPropagateFrom() {modulesToPropagateFrom.clear();}

        bool runOnInstanceGraphNode(CoreIR::InstanceGraphNode& node) override;
        void print() override {
            std::cout << "Propagating ports\n";
        }

    private:
        // Tracker of modules that have ports that need to be propagated
        std::unordered_map<CoreIR::Module *, std::vector<std::string>> modulesToPropagateFrom;
};
std::string PropagatePorts::ID = "propagatePorts";

CGRA::CGRA(std::string name, std::string templateName)
    : top_level_module(std::make_unique<Module>(std::move(name), std::move(templateName)))
{
}

const MRRG& CGRA::getMRRG(int II)
{
    if((std::ptrdiff_t)mrrgs.size() < II)
    {
        mrrgs.resize(II, std::shared_ptr<MRRG>(nullptr));
    }

    auto& mrrg_ptr = mrrgs[II-1];
    if(!mrrg_ptr.get())
    {
        auto initial_mrrg = std::unique_ptr<const MRRG>(getTopLevelModule().createMRRG(II));
        verifyAndPrintReport(*initial_mrrg, std::cout, true, true);
        mrrg_ptr = std::make_shared<const MRRG>(std::move(*initial_mrrg));
    }

    return *mrrg_ptr;
}


// Function that generates the bitstream
BitStream CGRA::genBitStream(const Mapping& mapping)
{
    // figure out what the scanchain order is
    std::vector<ConfigCell*> config_table;
    getTopLevelModule().genConfigOrder(config_table);

    // result for this function
    BitStream bitstream;

    // Dump some info
    std::cout << "config table: \n\t";
    for (auto* p : config_table) {
        std::cout << p << ' ';
    }
    std::cout << '\n';

    // create an index from (Module, OpGraphOp) to a set of MRRG nodes mapped to that OpGraphOp within that Module
    std::map<const Module*, MRRGNodesFromOpNode> mrrgnodes_for_op_node_for_module;
    for(auto* op : mapping.getOpGraph().opNodes())
    {
        for (auto* n : mapping.getMappingList(op)) {
            mrrgnodes_for_op_node_for_module[n->parent][op].insert(n);
        }
    }

    // dump some more info
    std::cout << "Value mappings: (val node: {mrrg node })\n";
    for(auto* val : mapping.getOpGraph().valNodes()) {
        std::cout << val << ":";
        for (auto* nptr : mapping.getMappingList(val)) {
            const auto& mrrg_node = *nptr;
            std::cout << mrrg_node << " ";
        }
        std::cout << '\n';
    }

    // create an index from (Module, OpGraphVal) to a set of MRRG nodes mapped to that OpGraphVal within that Module
    std::map<const Module*, MRRGNodesFromValNode> mrrgnodes_for_val_node_for_module;
    for(auto* val : mapping.getOpGraph().valNodes()) {
        for(auto* n : mapping.getMappingList(val)) {
            mrrgnodes_for_val_node_for_module[n->parent][val].insert(n);
        }
    }

    if (mrrgnodes_for_val_node_for_module.find(nullptr) != end(mrrgnodes_for_val_node_for_module)) {
        std::cout << "detected mrrg node(s) with unset parent (no containing Module)!:\n";
    }

    int II = mapping.getII();
    // in scanchain order, pass items from the indexes created earlier to the module's getBitConfig functions
    for (const auto* ccell : config_table) {
        const auto* module = &ccell->getSingleConnectedPort().getModule();

        const auto module_and_mrrg_nodes_for_val_node = mrrgnodes_for_val_node_for_module.find(module);
        const auto module_and_mrrg_nodes_for_op_node = mrrgnodes_for_op_node_for_module.find(module);
        const bool val_nodes_available = module_and_mrrg_nodes_for_val_node != end(mrrgnodes_for_val_node_for_module);
        const bool op_nodes_available = module_and_mrrg_nodes_for_op_node != end(mrrgnodes_for_op_node_for_module);
        const auto value_for_bitstream = [&](){
            try {
                // separate statements so we can avoid modifying the indexes
                if (val_nodes_available) {
                    if (op_nodes_available) {
                        return module->getBitConfig(getMRRG(II), mapping.getOpGraph(), mapping, *ccell, module_and_mrrg_nodes_for_op_node->second, module_and_mrrg_nodes_for_val_node->second);
                    } else {
                        return module->getBitConfig(getMRRG(II), mapping.getOpGraph(), mapping, *ccell, {}, module_and_mrrg_nodes_for_val_node->second);
                    }
                } else {
                    if (op_nodes_available) {
                        return module->getBitConfig(getMRRG(II), mapping.getOpGraph(), mapping, *ccell, module_and_mrrg_nodes_for_op_node->second, {});
                    } else {
                        return module->getBitConfig(getMRRG(II), mapping.getOpGraph(), mapping, *ccell, {}, {});
                    }
                }
            } catch (...) {
                // print out some diagnostic information. The virtual functions of Module are user configuration points,
                // and so it's probably a good idea to provide some diagnostics when things go wrong.
                std::cout << "Exception thrown when getting bitconfig from " << module->getName() << " with the following input:" << std::endl;
                std::cout << "ConfigCell name: " << ccell->getName() << std::endl;
                std::cout << "Op Nodes:" << std::endl;
                if (op_nodes_available) {
                    for (const auto& op_and_nodes : module_and_mrrg_nodes_for_op_node->second) {
                        std::cout << '\t' << *op_and_nodes.first << std::endl;
                        for (const auto& mrrg_node : op_and_nodes.second) {
                            std::cout << "\t\t" << *mrrg_node << std::endl;
                        }
                    }
                } else {
                    std::cout << "\t[ none ]" << std::endl;
                }
                std::cout << "Value Nodes:" << std::endl;
                if (val_nodes_available) {
                    for (const auto& val_and_nodes : module_and_mrrg_nodes_for_val_node->second) {
                        std::cout << '\t' << *val_and_nodes.first << std::endl;
                        for (const auto& mrrg_node : val_and_nodes.second) {
                            std::cout << "\t\t" << *mrrg_node << std::endl;
                        }
                    }
                } else {
                    std::cout << "\t[ none ]" << std::endl;
                }
                std::cout << "End listing" << std::endl;
                throw;
            }
        }();

        // Adding padding config bits when used contexts (II) is less than supported contexts (SII)
        auto padded_bitstream = BitConfig(ccell->l_contexts);
        for (auto cycle=0; cycle<ccell->l_contexts ; cycle++) {
          if (cycle < II) {
            padded_bitstream.add(value_for_bitstream.getBitSetting()[cycle], cycle);
          }
          else {
            padded_bitstream.add({(size_t)ccell->getStorageSize(), BitSetting::LOW}, cycle);
          }
        }

        // check that we get the expected number of bits
        if ((std::ptrdiff_t)value_for_bitstream[0].size() != ccell->getStorageSize()) {
            std::cout << ("wrong number of bits returned for " + module->getName() + "::" + ccell->getName()) << ": " << value_for_bitstream[0] << ". Expected " << ccell->getStorageSize() << " bits. Got " << value_for_bitstream[0].size() << " bits\n";
            throw cgrame_error("wrong number of bits returned for " + module->getName() + "::" + ccell->getName());
        } else {
            if(dynamic_cast<const ElasticConfigCell*>(ccell)){
                int used_cycles = value_for_bitstream.getUsedCycles();
                 bitstream.append(ccell, value_for_bitstream.getBitSetting(), used_cycles);
            } else {
                bitstream.append(ccell, padded_bitstream.getBitSetting());
            }
        }
    }

    return bitstream;
}

/*
// Generates timing constraints for CGRA using an opgraph that is already mapped
void CGRA::genTimingConstraints(OpGraph * opgraph)
{
    // the Tickle script we will be writing to
    std::ofstream Timing;
    Timing.open("primetime.tcl");

    // Similar to the bitstream generator, we want all modules that we need to configure
    std::vector<Module*> ConfigTable; // Table that holds modules that need to be configured
    genConfigOrder(ConfigTable); // Initializing the table

    // Disabling all IO's, Muxes, and FuncUnits
    Timing << "# disabling required timing paths\n";
    for (unsigned i = 0; i < ConfigTable.size(); i++)
    {
        if (ConfigTable[i]->getModuleType() == MOD_PRIM_MUX) // If it's a configurable mux, disable it (no need to disable output port if all inputs are disabled
        {
            Timing << "set_disable_timing " << ConfigTable[i]->ReturnPath() << "/in*\n"; // "in" is hardcoded for mux inputs"
        }
        else if (ConfigTable[i]->getModuleType() == MOD_PRIM_FUNC)
        {
            for (auto it = ConfigTable[i]->submodules.begin(); it != ConfigTable[i]->submodules.end(); it++) // If it's a FuncUnit, disable all submodules
            {
                Timing << "set_disable_timing " << it->second->ReturnPath() << "\n";
            }
        }
        else if (ConfigTable[i]->parent->getModuleType() == MOD_PRIM_IO) // If its parent is an IO device
        {
            Timing << "set_disable_timing " << ConfigTable[i]->parent->ReturnPath() << "\n";
        }
        else if (ConfigTable[i]->getModuleType() == MOD_PRIM_TRI) // If it's just a Tristate, and it's parent is not an IO device
        {
            Timing << "set_disable_timing " << ConfigTable[i]->ReturnPath() << "\n";
        }
    }

    // Re-enabling the required paths
    Timing << "# now re-enabling required timing paths\n";
    for(auto & op: opgraph->opNodes()) // For all OPNodes, we want to get bits for IO devices and FuncUnits
    {
        if (op->opcode == OpCode::INPUT) // If it's an input node -> IO
        {
            Timing << "remove_disable_timing " << op->mapped_nodes[0]->parent->ReturnPath() << "\n";
            Timing << "set_disable_timing " << op->mapped_nodes[0]->parent->ReturnPath() << "/OE\n";
        }
        else if (op->opcode == OpCode::OUTPUT) // If it's an output node -> IO
        {
            Timing << "remove_disable_timing " << op->mapped_nodes[0]->parent->ReturnPath() << "\n";
        }
        else // If it's a function node->FuncUnit
        {
            Timing << "remove_disable_timing " << op->mapped_nodes[0]->parent->ReturnPath()
                << "/" << ((FuncUnit*)op->mapped_nodes[0]->parent)->all_modes[op->opcode].ModuleName << "\n";
        }
    }
    for(auto & val: opgraph->valNodes()) // Dealing with the Multiplexers
    {
        for(auto & n: val->mapped_nodes)
        {
            if (n->pt == MUX_IN) // If it's a mux in node
            {
                Timing << "remove_disable_timing " << n->parent->ReturnPath() << "/in";
                Timing << getNumAfterString(n->name, ".in") << "\n";
            }
        }
    }
    Timing.close();
}
*/

// Floorplans CGRABlocks on ASIC chip
void CGRA::genFloorPlan()
{
    auto& submodules = getTopLevelModule().submodules;
    // the Tickle script we will be writing to
    std::ofstream Floorplan;
    Floorplan.open("encounter.tcl");
    // Setting up the variables in the tcl script
    Floorplan << "variable blockarea\n"; // Area of a CGRA block
    Floorplan << "variable blocklength [expr sqrt($blockarea) / 0.85]\n"; // How long a block will be
    Floorplan << "variable N " << num_floorplan_rows << "\n"; // # of rows in CGRA
    Floorplan << "variable M " << num_floorplan_columns << "\n"; // # of cols in CGRA
    Floorplan << "variable cgrawidth [expr $N*$blocklength]\n"; // CGRA width
    Floorplan << "variable cgraheight [expr $M*$blocklength]\n"; // CGRA height
    Floorplan << "variable border 20\n"; // Width of the border used on ASIC chip
    // Loading the libraries required
    Floorplan << "# loading libraries\n";
    Floorplan << "loadConfig encounter.conf 0\n";
    Floorplan << "setUIVar rda_Input ui_netlist CGRA_comp.v\n";
    Floorplan << "setUIVar rda_Input ui_topcell CGRA\n";
    Floorplan << "commitConfig\n";
    // Setting up the chip
    Floorplan << "# set up chip\n";
    Floorplan << "floorPlan -site core -s $cgrawidth $cgraheight $border $border $border $border\n";

    // Floorplanning the CGRABlocks
    Floorplan << "# floorplan blocks\n";

    int CurrentROW = 0, CurrentCOL = 0;
    for (auto it = submodules.begin(); it != submodules.end(); it++) // Iterate through all the non-IO modules in CGRA
    {
        // At the time this was comment out, nothing set MOD_PRIM_IO
        // if (it->second->getModuleType() != MOD_PRIM_IO) // If it's not an IO device, floorplan it
        {
            Floorplan << "setObjFPlanBox Module " << it->second->getName();
            // First argument
            Floorplan << " [expr $border+" << CurrentCOL << "*$blocklength] ";
            // Second argument
            Floorplan << "[expr $cgraheight+$border-" << (CurrentROW+1) << "*$blocklength] ";
            // Third argument
            Floorplan << "[expr $border+" << (CurrentCOL+1) << "*$blocklength] ";
            // Fourth argument
            Floorplan << "[expr $cgraheight+$border-" << CurrentROW << "*$blocklength]\n";
            CurrentCOL++; // Move to next position in CGRA
            if (CurrentCOL == num_floorplan_columns) // Change to a new row if we reached the end of it
            {
                CurrentCOL = 0;
                CurrentROW++;
            }
        }
    }

    // Placing the design
    Floorplan << "# placing design\n";
    Floorplan << "getMultiCpuUsage -localCpu\n";
    Floorplan << "setPlaceMode -fp false\n";
    Floorplan << "placeDesign -prePlaceOpt\n";

    // Routing the design
    Floorplan << "# routing design\n";
    Floorplan << "setNanoRouteMode -quiet -drouteStartIteration default\n";
    Floorplan << "setNanoRouteMode -quiet -routeTopRoutingLayer default\n";
    Floorplan << "setNanoRouteMode -quiet -routeBottomRoutingLayer default\n";
    Floorplan << "setNanoRouteMode -quiet -drouteEndIteration default\n";
    Floorplan << "setNanoRouteMode -quiet -routeWithTimingDriven false\n";
    Floorplan << "setNanoRouteMode -quiet -routeWithSiDriven false\n";
    Floorplan << "routeDesign -globalDetail\n";

    // Saving the design
    Floorplan << "saveDesign CGRA.enc\n";
    // Getting spef file for Primetime
    Floorplan << "rcOut -spef CGRA.spef\n";
    // Getting netlist for Primetime
    Floorplan << "saveNetlist CGRA_Golden.v\n";

    Floorplan.close();
}

namespace {

std::vector<Module*> computeModulePostorder(Module* module, std::vector<Module*> unique_postorder) {
    for(const auto& nameAndSubmodule : module->submodules) {
        auto submod = nameAndSubmodule.second;
        unique_postorder = computeModulePostorder(submod, std::move(unique_postorder));
    }

    for(const auto& nameAndConfigCell : module->configcells) {
        auto ccell = nameAndConfigCell.second;
        unique_postorder = computeModulePostorder(ccell, std::move(unique_postorder));
    }

    unique_postorder.push_back(module);

    return std::move(unique_postorder);
}


void genVerilogCoreIR(CGRA& cgra, std::string dir, const int& contexts) {
    CoreIR::Context* c = CoreIR::newContext();
    CoreIR::Namespace* cgrame = c->newNamespace("cgrame");
    (void)cgrame; // don't have a use for the namespace here, yet. It just needs to exist
    auto cgra_top = &cgra.getTopLevelModule();

    const auto generate_order = computeModulePostorder(cgra_top, {});
    std::set<std::string> generated_names;
    for (const auto& module : generate_order) {
        if (
            not generated_names.insert(module->GenericName()).second // Can only have one of each generator
        ) {
            continue;
        }
        module->CoreIRGenModuleVerilog(c, contexts);
    }

    CoreIR::Module* topMod = cgrame->getGenerator(cgra_top->GenericName())->getModule({});
    topMod->runGenerator(); // top module must have a definition before calling setTop
    c->setTop(topMod);
    std::string fileName = dir + "/cgrame.v";
    std::ofstream fout(fileName);
    c->addPass(new PropagatePorts());
    c->runPasses({"rungenerators", "removebulkconnections", "flattentypes", "propagatePorts", "verilog"}, {"cgrame"});
    auto& vpass = dynamic_cast<CoreIR::Passes::Verilog&>(*c->getPassManager()->getAnalysisPass("verilog"));
    std::vector<CoreIR::Passes::VModule*> modList = vpass.getModList();
    cgra.hybridPorts = modList[modList.size()-1]->getPorts();
    vpass.writeToStream(fout);
    CoreIR::deleteContext(c);
}

void genHybridCoreIR(CGRA& cgra, std::string dir, int mem_size) {
    std::ofstream myfile;
    std::string fileName = dir + "/control.sv";
    myfile.open(fileName);
    buildComp(myfile, cgra.hybridPorts, mem_size);
}

}

void CGRA::genVerilog(VerilogType vt, std::string dir, const int& contexts) {
    switch(vt) {
        case VerilogType::CGRAME: return getTopLevelModule().genVerilogCGRAME(dir);
        case VerilogType::CoreIR: return genVerilogCoreIR(*this, dir, contexts);
    }
}

void CGRA::genHybrid(VerilogType vt, std::string dir, int mem_size) {
    switch(vt) {
        case VerilogType::CGRAME: std::cout << "Not yet" << "";
        case VerilogType::CoreIR: return genHybridCoreIR(*this, dir, mem_size);
    }
}


/**
 * PropagatePorts is a CoreIR InstanceGraph Pass. An instance graph pass allows
 * iterates from the bottom level modules to the top level module.
 * runOnInstanceGraphNode operates on each type of module only once, which
 * means you cannot try to use a slighlty altered instance of a module to
 * propagate ports specifically.
 * The pass starts from the lowest level modules and checks if they have ports
 * to propagate. If they do, then it keeps note of this module and the ports in
 * a unordered_map. At the higher level modules it checks if its children
 * contains one of the modules with ports to propagate. If true, then it adds
 * those ports to itself, connects them to the children port and once again
 * marks itself.
 */
bool PropagatePorts::runOnInstanceGraphNode(CoreIR::InstanceGraphNode& node) {
    CoreIR::Context* c = this->getContext();

    CoreIR::Module* m = node.getModule();

    if (m->isGenerated()) {
        CoreIR::Generator* g = m->getGenerator();
        if (g->getMetaData().find("propagatePorts") != g->getMetaData().end()) {
            std::vector<std::string> portsToPropagate = g->getMetaData()["propagatePorts"].get<std::vector<std::string>>();
            if (!portsToPropagate.empty()) {
                modulesToPropagateFrom.emplace(m, portsToPropagate);
            }
        }
    }

    if (!m->hasDef()) return false;
    CoreIR::ModuleDef* def = m->getDef();
    // Check if the child has a special port that needs to be propagated, add the wire and/or connect it
    for (auto imap : def->getInstances()) {
        CoreIR::Instance* inst = imap.second;
        CoreIR::Module* mref = imap.second->getModuleRef();

        if (modulesToPropagateFrom.find(mref) != modulesToPropagateFrom.end()) {
            // std::cout << mref->getName() << " is in " << m->getName() << "\n";
            string iname = imap.first;
            std::vector<string> portsToPropagate = modulesToPropagateFrom.at(mref);
            auto thisRecord = m->getType()->getRecord();
            auto submodRecord = mref->getType()->getRecord();
            for (auto & port : portsToPropagate) {
                // std::cout << "Looking for " << port << "\n";
                // Iterate through the instance's wireables to find the port's type
                if (submodRecord.find(port) != submodRecord.end()) {
                    if (thisRecord.find(iname+"_"+port) == thisRecord.end()) {
                        node.appendField(iname+"_"+port, mref->getType()->getRecord().at(port));
                        // std::cout << "Adding port: " << port << "\n";
                    }
                    // std::cout << "Connecting " << port << " from " << iname << "\n";
                    def->connect("self." + iname + "_" + port, iname + "." + port);
                    port = iname+"_"+port;
                }
            }
            if (modulesToPropagateFrom.find(m) == modulesToPropagateFrom.end()) {
                modulesToPropagateFrom.emplace(m, portsToPropagate);
            }
            else {
                // Case where a module has multiple children with ports to propagate
                std::copy(portsToPropagate.begin(), portsToPropagate.end(), std::back_inserter(modulesToPropagateFrom[m]));
            }
        }
    }

    return true;
}
