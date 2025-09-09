/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

/**
 * This file should be limited to containing the MRRG definition
 * and very basic closely-related functions. Most headers than require
 * the MRRG should only have to include this file. General procedures
 * can be placed in `MRRGProcedures.h`.
 */

#ifndef MRRG__H_
#define MRRG__H_

#include <CGRA/ConfigStore.h>
#include <CGRA/Exception.h>
#include <CGRA/OpGraph.h>
#include <CGRA/Util.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

enum MRRGNode_Type {
    MRRG_NODE_ROUTING,
    MRRG_NODE_FUNCTION,
    MRRG_NODE_ROUTING_FUNCTION
};

enum NodePortType {
    UNSPECIFIED,
    MUX_OUT,
    MUX_IN,
};

enum HWEntityType {
    HW_WIRE, //note: buffer/tristate-buffer included
    HW_REG,
    HW_MUX,
    HW_COMB, //note: memory units included
    HW_UNSPECIFIED,
};

class OpGraph;
class OpGraphOp;
class Module;

class MRRGNode
{
    public:
        using SupportedOpTags = std::set<std::string>;   // Operand ids that can use this node
        template<typename SupportedOps = std::vector<OpGraphOpCode>>
        MRRGNode(
            Module* parent,
            int cycle,
            std::string name,
            MRRGNode_Type type,
            int latency,
            int bitwidth,
            SupportedOps&& supported_ops,
            HWEntityType et,
            SupportedOpTags supported_operand_tags,
            int max_cap = 1
        )
            : name(name)
            , type(type)
            , et(et)
            , supported_ops(std::forward<SupportedOps>(supported_ops))
            , fanout()
            , fanin()
            , supported_operand_tags(supported_operand_tags)
            , parent(parent)
            , cycle(cycle)
            , bitwidth(bitwidth)
            , latency(latency)
            , capacity(max_cap)
        {
            if (latency < 0) {
                cgrame_model_error("Illegal latency value specified!");
            }
        }

    private:
        // the MRRG is a doubly linked graph, so don't copy nodes!
        // these are needed for implementation reasons, though.
        MRRGNode(const MRRGNode&) = default;
        MRRGNode& operator=(const MRRGNode&) = default;

    public:
        MRRGNode(MRRGNode&&) = default;
        MRRGNode& operator=(MRRGNode&&) = default;

        // is this node in a MRRG? consider `MRRG::renameNode` if changing this
        std::string name;
        MRRGNode_Type type;
        HWEntityType et = HW_UNSPECIFIED; // this is a helper flag for generating timing graph

        bool canMapOp(OpGraphOp const* op) const;
        std::vector<OpGraphOpCode> supported_ops;

        std::vector<MRRGNode*>  fanout;
        std::vector<MRRGNode*>  fanin;

        // Possible tags include: any2input, any3input, pred, LHS, RHS, etc. see OpGraph.h
        SupportedOpTags supported_operand_tags;

        Module* parent;

        std::string getFullName() const;
        const std::string& getHierarchyQualifiedName() const { return name; }

        auto getContextNum() const { return cycle; }
        auto getLatency() const { return latency; }

        unsigned int     cycle;
        unsigned int     delay = 0;  // Delay in picoseconds
        unsigned int     bitwidth = 32;

        // Latency in cycles through the node min, max if latency is programmable
        int     latency;
        
        // the following two variables may be used during mapping
        // the capacity of the node, how many things can be mapped to it
        int     capacity = 1;

/*
        // fixed mapper data
        float   base_cost;
        int     occupancy;

        float getCost(float penalty_factor = 1);


        //find which op node that the MRRG node corresponds to
        OpGraphOp* getOpNodePtr (const OpGraph& graph);
*/        

    template<typename STR>
    static MRRGNode make_routing(Module* parent, int bitwidth, int cycle, STR&& name, int latency = 0, int max_cap = 1) {
        auto type = MRRG_NODE_ROUTING;
        HWEntityType et;
        if (latency == 0) {
            et = HW_WIRE;
        } else if (latency >= 1) {
            et = HW_REG;
        }
        int capacity = 1;
        return {parent, cycle, std::forward<STR>(name), type, latency, bitwidth, std::vector<OpGraphOpCode>(), et, SupportedOpTags(), capacity};
    }

    template<typename STR>
    static MRRGNode make_operand_pin(Module* parent, int bitwidth, int cycle, STR&& name, SupportedOpTags operand_tags, int latency = 0, int max_cap = 1) {
        auto type = MRRG_NODE_ROUTING;
        HWEntityType et;
        if (latency == 0) {
            et = HW_WIRE;
        } else if (latency >= 1) {
            et = HW_REG;
        }
        int capacity = 1;
        return {parent, cycle, std::forward<STR>(name), type, latency, bitwidth, std::vector<OpGraphOpCode>(), et, operand_tags, capacity};
    }

    template<typename STR, typename SupportedOps = std::vector<OpGraphOpCode>>
    static MRRGNode make_routing_function (Module* parent, int bitwidth, int cycle, STR&& name, int latency, SupportedOpTags operand_tags, SupportedOps supported_ops, int max_cap = 1) {
        auto type = MRRG_NODE_ROUTING_FUNCTION;
        HWEntityType et;
        if (latency == 0) {
            et = HW_WIRE;
        } else {
            et = HW_REG;
        }
        int capacity = max_cap;
        return {parent, cycle, std::forward<STR>(name), type, latency, bitwidth, supported_ops, et, SupportedOpTags(), capacity};
    }
    friend std::ostream& operator<< (std::ostream& out, const MRRGNode& node);

    template<typename STR, typename SupportedOps = std::vector<OpGraphOpCode>>
    static MRRGNode make_function(Module* parent, int bitwidth, int cycle, STR&& name, int latency, SupportedOps supported_ops, int max_cap = 1, bool is_const_unit = false) {
        auto type = MRRG_NODE_FUNCTION;
        HWEntityType et;
        if (is_const_unit) {
            et = HW_REG;
        } else if (latency == 0) {
            et = HW_COMB;
        } else {
            et = HW_REG;
        }
        int capacity = max_cap;
        return {parent, cycle, std::forward<STR>(name), type, latency, bitwidth, supported_ops, et, SupportedOpTags(), capacity};
    }

    friend std::ostream& operator<< (std::ostream& out, const MRRGNode& node);

 private:
    friend class MRRG;
    /**
     * Renames references in this class according to `rename_map`.
     * If a reference cannot be found in the map, then it will not be changed
     */
    void applyReferenceRename(const std::unordered_map<const MRRGNode*, MRRGNode*>& rename_map);
};

class MRRG
{
    public:
        using NodeDescriptor = const MRRGNode*;

        MRRG(int II)
            : nodes(II)
            , subMRRGs{}
        { }

        MRRG(const MRRG& src)
            : nodes()
            , subMRRGs{}
        { *this = src; }

        MRRG(MRRG&&) = default;
        MRRG& operator=(const MRRG& rhs);
        MRRG& operator=(MRRG&&) = default;

        /**
         * Add `node` to this MRRG by `std::move`ing it to the heap.
         * Overloads with `fanin` or `fanins` arguments add the given node and also add
         * link(s) from the node(s) in that argument.
         */
        std::pair<NodeDescriptor,bool> insert(MRRGNode node);
        std::pair<NodeDescriptor,bool> insert(NodeDescriptor fanin, MRRGNode node) { return insertMultiFanin(std::array<NodeDescriptor,1>{{fanin}}, std::move(node)); }

        template<typename NodeDescList = std::vector<NodeDescriptor>>
        std::pair<NodeDescriptor,bool> insertMultiFanin(NodeDescList fanins, MRRGNode node) {
            auto ir = insert(std::move(node));
            if (ir.second) for (auto& fanin : fanins) link(fanin, ir.first);
            return ir;
        }

        /**
         * For creating and destroying edges between nodes in an MRRG
         * All may perform some checks (especially when unlinking) on the validity of the graph,
         * throwing if something is wrong. Overall, better than manipulating the
         * fanout & fanin lists directly.
         */
        void link(MRRG::NodeDescriptor driver, MRRG::NodeDescriptor fanout);
        void unlink(MRRG::NodeDescriptor driver, MRRG::NodeDescriptor fanout);

        /**
         * Remove the node from the graph. Disconnects all fanout & fanin
         */
        void erase(MRRG::NodeDescriptor n);

        /**
         * Get a node descriptor.
         */
        NodeDescriptor getNode(int cycle, const std::string& name) const;
        NodeDescriptor getNodeByPropertiesOf(const MRRGNode& node) const { return getNode(node.cycle, node.name); }

        /**
         * Get an actual node object from your descriptor.
         * Useful for "laundering" your const pointer if you have a non-const MRRG.
         * Undefined Behaviour if this node is not in this MRRG.
         */
              MRRGNode& getNodeRef(NodeDescriptor ndesc)       { return *const_cast<MRRGNode*>(ndesc); }
        const MRRGNode& getNodeRef(NodeDescriptor ndesc) const { return *ndesc; }

        /**
         * Node getters that include the step of getting a reference, for making code simpler
         */
              MRRGNode& getNodeRef(int cycle, const std::string& name)       { return getNodeRef(getNode(cycle, name)); }
        const MRRGNode& getNodeRef(int cycle, const std::string& name) const { return getNodeRef(getNode(cycle, name)); }
              MRRGNode& getNodeRefByPropertiesOf(const MRRGNode& node)       { return getNodeRef(getNodeByPropertiesOf(node)); }
        const MRRGNode& getNodeRefByPropertiesOf(const MRRGNode& node) const { return getNodeRef(getNodeByPropertiesOf(node)); }

        /**
         * Gives a list of elements that fan-out from the given descriptor.
         * Allows a MRRG to be used with most graph algorithms.
         */
        auto& fanout(NodeDescriptor ndesc) const { return getNodeRef(ndesc).fanout; }

        /**
         * Gives a list of elements that fan-in to the given descriptor.
         */
        auto& fanin(NodeDescriptor ndesc) const { return getNodeRef(ndesc).fanin; }

        /**
         * Analyses this MRRG to find violations of various invariants, such as:
         *  - all links are setup properly
         *
         * `options`:
         *  - "check_mux_exclusivity" -- should the mux exclusivity invariant be checked
         *     - allowed values: "error" "warning" "info" "ignore". Default is "ignore".
         */
        struct VerifyMessage {
            enum class Type : char { Info, Warning, Error, };
            Type type;
            std::string message;
        };
        std::vector<VerifyMessage> verify(const ConfigStore& options = {}) const;

        /**
         * Print a machine readable text form of this graph.
         * Uses the classic .dot format, openable with xdot and other programs.
         */
        void printDot(std::ostream& os, Module*, const ConfigStore& archAttrs) const;

        /**
         * Print a machine readable text form of this graph.
         * Uses the classic .dot format, openable with xdot and other programs.
         */
        void printSubmoduleDot(std::ostream& os, Module*) const;

        /**
         * Print a machine readable text form of this graph.
         * Uses the classic .dot format, openable with xdot and other programs.
         */
        void printDot(std::ostream& os) const;

        /**
         * Same as above, but clustered by module
         */
        void printDotClustered(std::ostream& os) const;

        /**
         * Print the supported ops by this architecture
         */
        void printSupportedOps(std::ostream& os) const;

        /**
         * Print some basic info like no. nodes, no. edges, no. unique modules.
         */
        void printBasicStats(std::ostream& os) const;

        /**
         * The initiation interval this MRRG is supporting
         */
        int initiationInterval() const { return nodes.size(); }

        /**
         * The number of nodes. No guarantee on time complexity.
         */
        std::ptrdiff_t size() const;

        /**
         * Direct (const) access to the graph.
         * Likely to be phased out eventually. Use getNode* instead.
         */
        const auto& allNodesByCycle() const { return nodes; }

        /**
         * Changes the name of node corresponding to `ndesc`, and updates
         * any internal structures.
         */
        void renameNode(NodeDescriptor ndesc, std::string new_name);
private:
        std::vector<std::map<std::string, std::unique_ptr<MRRGNode>>> nodes;
        std::map<std::string, std::unique_ptr<MRRG>> subMRRGs;

        // Module::createMRRG needs to do some wizardry when merging MRRGs
        friend Module;
};

struct MrrgNodeSpan {
    MRRG::NodeDescriptor const* _data;
    std::size_t _size;
    std::size_t size() const { return _size; }
    MRRG::NodeDescriptor const* data() const { return _data; }
    MRRG::NodeDescriptor const* begin() const { return data(); }
    MRRG::NodeDescriptor const* end() const { return data() + size(); }
    MRRG::NodeDescriptor const& front() const { return *data(); }
    MRRG::NodeDescriptor const& back() const { return *(data() + size() - 1); }
};

inline std::ostream& operator<<(std::ostream& os, const MRRG::NodeDescriptor& ndesc) {
    os << (void*)ndesc << ':';
    if (ndesc) {
        os << *ndesc;
    } else {
        os << "nullptr";
    }
    return os;
}

/**
 * Makes a function object that takes a name and returns
 * that node within a fixed MRRG and cycle.
 * eg:
 *     const auto& nodes_this_cycle = makeNodeGetterForCycle(result, this_cycle);
 *     link(nodes_this_cycle("in"),        nodes_this_cycle("m_in"));
 */
template<typename M> // so we can capture const and mutable versions
auto makeNodeGetterForCycle(M& mrrg, int cycle) {
    return [&mrrg, cycle](const std::string& name) {
        return &mrrg.getNodeRef(cycle, name);
    };
}

/**
 * A routine for verifying the MRRG.
 * Prints out a success message, unless told not to.
 * Can throw if errors are found.
 * Returns true if no errors found
 * `extra_opts` is passed to MRRG::verify
 */
bool verifyAndPrintReport(const MRRG& mrrg, std::ostream& os,
    bool silent_on_no_errors, bool throw_if_errors, const ConfigStore& extra_opts = {});

/*********************
 * ################# *
 * ################# *
 *********************/

inline char operator+(const MRRG::VerifyMessage::Type& vm_type) { return (char)vm_type; }

inline std::ostream& operator<<(std::ostream& os, const MRRG::VerifyMessage::Type& vm_type) {
    using Type = MRRG::VerifyMessage::Type;
    switch (vm_type) {
        case Type::Info:      os << "Info";      break;
        case Type::Warning:   os << "Warning";   break;
        case Type::Error:     os << "Error";     break;
        default: os << "MRRGVMTNotImplementedByPrinter" << +vm_type; break;
    }
    return os;
}

#endif

