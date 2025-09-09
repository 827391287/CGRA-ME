/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef OPGRAPH_H___
#define OPGRAPH_H___

enum class OpCode : signed char;
using OpGraphOpCode = OpCode;

#include <CGRA/MRRG.h>
#include <CGRA/BitSetting.h>

#include <cstdint>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class OpCode : signed char
{
    NOP = -1,
    SEXT = 1,
    ZEXT,
    TRUNC,
    INPUT,
    INPUT_PRED,
    OUTPUT_PRED,
    OUTPUT,
    PHI,
    CONST,
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    OR,
    XOR,
    SHL,
    ASHR,
    LSHR,
    LOAD,
    STORE,
    GEP,
    ICMP,
    CMP,
    BR,
    SQRT,
    FADD,
    FMUL,
    FDIV,
    FP2INT,
    INT2FP,
    MULU_FULL_LO,
    MULU_HALF_LO,
    MULU_QUART_LO,
    MULU_FULL_HI,
    MULU_HALF_HI,
    MULU_QUART_HI,
    MULS_FULL_LO,
    MULS_HALF_LO,
    MULS_QUART_LO,
    MULS_FULL_HI,
    MULS_HALF_HI,
    MULS_QUART_HI,
    ADD_FULL,
    ADD_HALF,
    ADD_QUART,
    SELECT
};

using OperandTag = std::string;

/**
 * Standard operand names. Any string is allowed, but the MRRG must support it
 */
namespace Operands {
    static constexpr auto& UNTAGGED = "";
    static constexpr auto& PREDICATE = "pred";
    static constexpr auto& BINARY_LHS = "LHS";
    static constexpr auto& BINARY_RHS = "RHS";
    static constexpr auto& BINARY_ANY = "any2input";
    static constexpr auto& TERNARY_ANY = "any3input";
    static constexpr auto& MEM_ADDR = "addr";
    static constexpr auto& MEM_DATA = "data";
    static constexpr auto& BR_TRUE = "branch_true";
    static constexpr auto& BR_FALSE = "branch_false";
};

enum class EdgeKind {
    kDataFlow,
    kAlias
};

std::ostream& operator <<(std::ostream &os, const OpGraphOpCode &opcode);
const std::string& to_string(const OpGraphOpCode& opcode);
OpGraphOpCode opcode_from_string(const std::string& s);
std::istream& operator >>(std::istream &is, OpGraphOpCode &opcode);
inline auto operator+(const OpCode& oc) { return static_cast<std::underlying_type_t<OpCode>>(oc); }

class MRRG;
class MRRGNode;

class OpGraphNode {
 public:
        OpGraphNode(std::string name)
            : name(name)
        { }

        virtual ~OpGraphNode();

        const std::string& getName() const { return name; }

        std::string name;

        bool operator==(const OpGraphNode& rhs) const { return this->name == rhs.name; }
        bool operator!=(const OpGraphNode& rhs) const { return not (*this == rhs); }
};

class OpGraphVal;

class OpGraphOp : public OpGraphNode
{
 public:
        OpGraphOp(std::string name, int bitwidth);
        OpGraphOp(std::string name, int bitwidth, OpGraphOpCode opcode);
        OpGraphOp(std::string name, int bitwidth, OpGraphOpCode opcode, BitConfig* bitConfig);
        OpGraphOp(std::string name, int bitwidth, OpGraphOpCode opcode, std::int64_t constVal);
        OpGraphOp(std::string name, int bitwidth, OpGraphOpCode opcode, std::string cmpMode, bool cmp);
        ~OpGraphOp();

        OpGraphOp(const OpGraphOp&) = delete;
        OpGraphOp(OpGraphOp&&) = default;
        OpGraphOp& operator=(const OpGraphOp&) = delete;
        OpGraphOp& operator=(OpGraphOp&) = default;

        /**
         * Make a copy that is the same, but with no connections
         */
        OpGraphOp propertyClone() const { return propertyClone(name); }
        OpGraphOp propertyClone(std::string new_name) const;

        std::int64_t getConstValue() const { return constVal; }
        std::string getCmpMode() const { return cmpMode; }
        std::string getMemName() const { return memName; }

        auto getOpCode() const { return opcode; }

        OpGraphOpCode opcode;
        int bitwidth = 32;  // Vlaue of the op bitwidth
        std::int64_t constVal = 0;  // Value of this op if it is const
        std::string cmpMode = "";   // Value of compare operation
        std::string memName = "";   // Name of the memory

        std::vector<OpGraphVal*> input;

        OpGraphVal* output;
        BitConfig* bitConfig = nullptr;
        // doesn't compare fanout or fanin -- those are properties of the graph
        bool operator==(const OpGraphOp& rhs) const;
        bool operator!=(const OpGraphOp& rhs) const { return !(*this == rhs); }

 private:
        friend std::ostream& operator <<(std::ostream& output, const OpGraphOp& op);
        friend std::ostream& operator <<(std::ostream &os, const OpGraphOpCode &opcode);
        friend std::istream& operator >>(std::istream &is, OpGraphOpCode &opcode);
};

class OpGraphVal : public OpGraphNode
{
 public:
        OpGraphVal(std::string name, int bitwidth, int dist = 0, EdgeKind ek = EdgeKind::kDataFlow);
        OpGraphVal(std::string name, int bitwidth, OpGraphOp* input_op, int dist = 0, EdgeKind ek = EdgeKind::kDataFlow);
        ~OpGraphVal();

        OpGraphVal(const OpGraphVal&) = delete;
        OpGraphVal(OpGraphVal&&) = default;
        OpGraphVal& operator=(const OpGraphVal&) = delete;
        OpGraphVal& operator=(OpGraphVal&) = default;

        OpGraphOp* input;
        std::string getOperandForOutput(const OpGraphOp*);
        bool getPredicateForOutput(const OpGraphOp*);
        int getBitwidthForOutput(const OpGraphOp*);
        EdgeKind getKind() {return kind;}
        EdgeKind kind;
        // all fanouts
        std::vector<OpGraphOp*>         output;
        // this is the operand port that this value feeds to at the downstream Op. Parallel array to the output vector
        std::vector<std::string> output_operand;
         std::vector<int> output_bitwidth;
        int dist = 0;
        int bitwidth = 0;
         std::vector<bool> output_predicate;

        friend std::ostream& operator<<(std::ostream& output, const OpGraphVal& val);

        // doesn't compare fanout or fanin -- those are properties of the graph
        bool operator==(const OpGraphVal& rhs) const {
            return *(const OpGraphNode*)this == rhs;
        }
        bool operator!=(const OpGraphVal& rhs) const { return !(*this == rhs); }

 private:
};

// Multiple output DFG with val/net nodes between op nodes.
class OpGraph
{
    public:
        using NodeDescriptor = const OpGraphNode*;
        using OpDescriptor = const OpGraphOp*;
        using ValDescriptor = const OpGraphVal*;

        // Uniquely describes a hyper-edge output pin.
        // Lets you interact with this graph as-if it were a multi-graph
        struct EdgeDescriptor {
            OpGraph::ValDescriptor val = null_val; int output_index = -1;
            explicit operator bool() const { return (bool)val; }
            friend std::ostream& operator<<(std::ostream& os, const EdgeDescriptor& ed);
        };

        // A list of hyper-edge output pins describing a path through this graph
        using Walk = std::vector<EdgeDescriptor>;

        /**
         * Create an empty graph.
         * see also `createOpGraphFromConfig` in OpGraphProcedures.h
         */
        OpGraph();

        /**
         * Move-only for now, as copying unimplemented.
         */
        OpGraph(const OpGraph&);
        OpGraph(OpGraph&&) = default;
        OpGraph& operator=(const OpGraph&);
        OpGraph& operator=(OpGraph&&) = default;

        /**
         * Assumes ownership of all nodes
         */
        ~OpGraph();

        /**
         * Move op into the graph, returning it's descriptor.
         * The op will have no fanout.
         * Returns the op's descriptor.
         */
        OpDescriptor insert(OpGraphOp op);

        /**
         * Like insert, but directly construct a Op
         */
        template<typename... Args>
        OpDescriptor emplace(Args&&... args) { return insert(OpGraphOp(std::forward<Args>(args)...)); }

        /**
         * Move fanout into the graph as a fanout of driver, creating
         * the val if necessary.
         * Returns the descriptors for the linking val and the new op.
         */
        struct OpOpInsertResult {
            ValDescriptor link; OpDescriptor newOp;
        };
        OpOpInsertResult insert(OpDescriptor driver,
            OpGraphOp fanout,
            std::string operand_group,
            int bitwidth = 32,
            int dist = 0,
            EdgeKind kind = EdgeKind::kDataFlow);

        /**
         * Makes fanout a fanout of diver, creating the val if necessary.
         * Returns the linking vals's descriptor.
         */
        ValDescriptor link(OpDescriptor driver,
            OpDescriptor fanout,
            std::string operand_group,
            int bitwidth = 32,
            int dist = 0,
            EdgeKind kind = EdgeKind::kDataFlow,
            bool predicate = false);
        ValDescriptor link(ValDescriptor val, OpDescriptor fanout, std::string operand_group);

        /**
         * Same as the regular link, but get the edge properties from base
         */
        ValDescriptor link_like(OpDescriptor driver, OpDescriptor fanout, EdgeDescriptor base);

        /**
         * Remove this link
         * Does not affect existing operand indexes
         */
        void unLink(ValDescriptor driver_val, OpDescriptor fanout);

        /**
         * Remove op from this DFG, as well as connecting edges
         * Does not affect existing operand indexes
         */
        void erase(OpDescriptor op);

        /**
         * Access to a range containing each input type or class
         */
        auto& opNodes() const { return op_nodes; }
        auto& valNodes() const { return val_nodes; }
        auto& aliasNodes() const { return alias_nodes; }

        /**
         * Get the val that this op drives/vals that drive this op.
         * May return the null descriptor or an empty list
         */
        ValDescriptor outputVal(OpDescriptor op) const;
        std::vector<ValDescriptor> outputVals(OpDescriptor op_desc) const;
        const std::vector<OpGraphVal*>& inputVals(OpDescriptor op) const;

        /**
         * Get the ops that op drives. (looks through val node)
         * Returns an empty range if this op drives no val
         */
        const std::vector<OpGraphOp*>& outputOps(ValDescriptor op) const;
        const std::vector<OpGraphOp*>& outputOps(OpDescriptor op) const;
            std::vector<OpDescriptor>  inputOps(OpDescriptor op) const;

        /** same as the above, but uses a standard name for compatibility with generic algorithms */
        const std::vector<OpGraphOp*>& fanout(OpDescriptor op) const { return outputOps(op); };

        /**
         * Get the op that drives val. May return null op
         */
        OpDescriptor inputOp(ValDescriptor val) const;

        /**
         * Get the operand group name that is associated with the i'th output of val
         */
        const OperandTag& getOperandTag(EdgeDescriptor edge) const;


        /**
         * Get the dist output of val
         */
        int getDist(EdgeDescriptor edge) const;
        int getBitwidth(EdgeDescriptor edge) const;
        EdgeKind getKind(EdgeDescriptor edge) const;

        /**
         * Get the op that is at the target end of this edge. null edge gives null op
         */
        OpDescriptor targetOfEdge(EdgeDescriptor ed) const;

        /**
         * Hyper-edge output pins that originate from/terminate at `op`
         */
        std::vector<EdgeDescriptor> outEdges(const OpDescriptor& op) const;
        std::vector<EdgeDescriptor> inEdges(const OpDescriptor& op) const;

        /**
         * Allows you to treat an OpGraph as a multi-graph instead of a hyper-graph
         */
        decltype(auto) fanout(const EdgeDescriptor& ed) const { return outEdges(targetOfEdge(ed)); }
        decltype(auto) fanin(const EdgeDescriptor& ed) const { return inEdges(targetOfEdge(ed)); }

        /**
         * Get an actual node object from your descriptor.
         * Useful for "laundering" your const pointer if you have a non-const OpGraph.
         * Undefined Behaviour if this node is not in this OpGraph.
         */
              OpGraphOp&  getNodeRef(OpDescriptor ndesc)        { return *const_cast<OpGraphOp*>(ndesc); }
        const OpGraphOp&  getNodeRef(OpDescriptor ndesc)  const { return *ndesc; }
              OpGraphVal& getNodeRef(ValDescriptor ndesc)       { return *const_cast<OpGraphVal*>(ndesc); }
        const OpGraphVal& getNodeRef(ValDescriptor ndesc) const { return *ndesc; }
              OpGraphNode& getNodeRef(NodeDescriptor ndesc)       { return *const_cast<OpGraphNode*>(ndesc); }
        const OpGraphNode& getNodeRef(NodeDescriptor ndesc) const { return *ndesc; }

        /**
         * Get node by it's name. Throws it it doesn't exist.
         */
        OpDescriptor getOp(const std::string& name)  const { return  ops_by_name.at(name); }
        ValDescriptor getVal(const std::string& name) const { return vals_by_name.at(name); }
              OpGraphOp&  getOpRef(const std::string& name)        { return getNodeRef( getOp(name)); }
        const OpGraphOp&  getOpRef(const std::string& name)  const { return getNodeRef( getOp(name)); }
              OpGraphVal& getValRef(const std::string& name)       { return getNodeRef(getVal(name)); }
        const OpGraphVal& getValRef(const std::string& name) const { return getNodeRef(getVal(name)); }

        /**
         * Get the index of the operation within the vector
         * 
         */
        int getOpIndex (OpGraphOp* op) const;
        int getValIndex (OpGraphVal* op) const;
        int getAliasIndex (OpGraphVal* op) const;
       /**
        *  Get the operation at index x within the vector
        */
        const OpGraphOp* getOpByIndex (int index) const;
        const OpGraphVal* getValByIndex (int index) const;
        
        /**
         * Cast to descriptor subtype
         * May crash if called from no-RTTI code, eg LLVM plugins
         */
        OpDescriptor  asOp(NodeDescriptor ndesc) const;
        ValDescriptor asVal(NodeDescriptor ndesc) const;

        //float getCost(float pfactor);
        int getMaxCycle();

        /**
         * A hold-over until this information is annotated directly on the OpGraph.
         * Finds edges that cause cycles, and assumes that they require a latency of 1.
         * All other edges are assumed to have latency 0.
         * The returned map is surjective; it covers the entire domain of edges.
         */
        std::map<EdgeDescriptor,int> edgeLatencies() const;

        void print_dot(std::ostream & s) const;
        void printDOTwithOps(std::ostream &s) const;

        /**
         * Print this graph in a way that looses no information.
         * Reading it back should produce the same logical graph
         *
         * The second overload allows you to specify an order to the ops.
         * Lower means print earlier. Is also used for edge ordering.
         */
        void serialize(std::ostream& s) const;
        void serialize(std::ostream& s, const std::map<OpDescriptor,int>& op_print_ranking) const;

        /**
         * Make this graph empty, free all memory.
         */
        void clear();

        /**
         * Analyses this OpGraph to find violations of various invariants, such as:
         *  - all links are setup properly
         *  - no nullptr nodes
         */
        struct VerifyMessage {
            enum class Type : char { Info, Warning, Error, };
            Type type;
            std::string message;
        };
        std::vector<VerifyMessage> verify() const;

        friend bool operator==(const OpGraph& lhs, const OpGraph& rhs);
        friend bool operator!=(const OpGraph& lhs, const OpGraph& rhs) { return not (lhs == rhs); }
        friend std::ostream& operator<<(std::ostream& os, const OpGraph& og) { og.serialize(os); return os; }

    private:
        // list of all nodes & hyper-edges in the graph
        std::vector<OpGraphOp*>     op_nodes;
        std::vector<OpGraphVal*>    val_nodes;
        std::vector<OpGraphVal*>    alias_nodes;

        // lookups
        std::unordered_map<std::string, OpDescriptor> ops_by_name;
        std::unordered_map<std::string, ValDescriptor> vals_by_name;

        // so we can return const& from fanout/fanin functions
        static const std::vector<OpGraphOp*> empty_op_vector;
        static const std::vector<OpGraphVal*> empty_val_vector;
        static const std::vector<EdgeDescriptor> empty_edge_vector;

        static constexpr ValDescriptor null_val = nullptr;
        static constexpr OpDescriptor null_op = nullptr;
        static constexpr EdgeDescriptor null_edge = {nullptr, -1};

        // erased nodes are placed here to ensure descriptors are unique
        std::vector<std::unique_ptr<OpGraphNode>> defunct_nodes = {};
};

/**
 * A routine for verifying the OpGraph.
 * Prints out a success message, unless told not to.
 * Can throw if errors are found.
 * Returns true if no errors found
 */
bool verifyAndPrintReport(const OpGraph& opgraph, std::ostream& os, bool silent_on_no_errors, bool throw_if_errors);

/**
 * A helper for the verifyAndPrintReport function
 */
bool analyzeOpgraphVerifyResults(std::ostream& os, const std::vector<OpGraph::VerifyMessage>& messages, const bool silent_on_no_errors);

struct OpSchedule {
    std::unordered_map<const OpGraphOp*, int> cycle = {};
    int latency = -1;
};

OpSchedule computeASAP(const OpGraph& op_graph);
OpSchedule computeALAP(const OpGraph& op_graph, unsigned int max_cycles);

struct OpGraphTransformResult {
    OpGraph transform_result = {};
    std::unordered_map<OpGraph::NodeDescriptor, OpGraph::NodeDescriptor> forward_mappings = {};
    std::unordered_map<OpGraph::NodeDescriptor, OpGraph::NodeDescriptor> reverse_mappings = {};
};

/**
 * Return a new graph with only the `allowed_ops` present
 */
OpGraphTransformResult filter(
    const OpGraph& src,
    const std::set<OpGraph::OpDescriptor>& allowed_ops
);

/**
 * Starting from `starting_points`, does a fanout BFS to
 *  find at least the given number of ops, including the starting points.
 * If this function must return an op to find enough ops,
 *  then it will return all ops at that distance from the starting points.
 * This way, it is independent of OpGraph's implementation details.
 */
std::set<OpGraph::OpDescriptor> findNDownstreamOps(
    const OpGraph& opgraph,
    const std::vector<OpGraphOp*>& starting_points,
    const std::ptrdiff_t n_ops
);

inline std::ostream& operator<<(std::ostream& os, const OpGraph::VerifyMessage::Type& vm_type);

inline char operator+(const OpGraph::VerifyMessage::Type& vm_type) { return (char)vm_type; }

inline bool operator<(const OpGraph::EdgeDescriptor& lhs, const OpGraph::EdgeDescriptor& rhs) {
    return std::tie(lhs.val, lhs.output_index) < std::tie(rhs.val, rhs.output_index);
}
inline bool operator==(const OpGraph::EdgeDescriptor& lhs, const OpGraph::EdgeDescriptor& rhs) {
    return std::tie(lhs.val, lhs.output_index) == std::tie(rhs.val, rhs.output_index);
}

#endif

