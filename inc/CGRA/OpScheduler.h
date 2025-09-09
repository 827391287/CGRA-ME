/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef INC_CGRA_OPSCHEDULER_H_
#define INC_CGRA_OPSCHEDULER_H_

#include <CGRA/MRRG.h>
#include <CGRA/OpGraph.h>

#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <stack>

#include <gurobi_c++.h>

enum class SchedType {
    ASAP,
    ALAP,
    SDCLIST,
    SDCLRO  // Live range optimization
};

using OpCodeEdge = std::pair<OpGraphOpCode, OpGraphOpCode>;

// Scheduler class to create a schedule for the opgrah
class OpScheduler {
 public:
        OpScheduler(
            const OpGraph& opgraph,
            const MRRG& mrrg,
            std::string arch_name,
            std::string supported_ops_file_name ="");
        ~OpScheduler() {}

        // Sets the different operation constraints to the GRB model
        int addOpConstr(GRBmodel* model, unsigned II, int max_step, bool ignore_backedge, int exit_id);

        // Caclulate the edge distance for the different edge types
        int calcEdgeDist(OpGraphVal* val, OpGraphOp* sink_op, unsigned II, bool ignore_backedge);

        // Calculates the node prioity depending on the asap schedule
        int calcNodePrio(OpGraphOp*, int);

        // Parses the network schedule file
        void parseNetworkSchedule(std::string netwrok_latency_filename);

        // Checks if the operation has a backedge
        bool isBackEdge(OpGraphOp*);

        // Schedules either asap or alap depending on the schedule type
        int schedASLAP(unsigned II, unsigned max_cycles, SchedType algo);

        // Schedules either LIST or LRO depening on the schedule type
        int schedSDCMod(unsigned II, SchedType algo);

        // Builds the last use table and sets the constriants for them
        void buildLastUseTable(int& ext_node_count, int exit_id,
                std::map<OpGraphOp*, int>& last_use);
        int addLastUseConstr(GRBmodel* model , std::map<OpGraphOp*, int> last_use);

        int getLowerBound(OpGraphOpCode op1, OpGraphOpCode op2) {
            if (lower_bound_op_code_edge.find({op1, op2}) == lower_bound_op_code_edge.end()) {
                throw make_from_stream<cgrame_model_error>([&](auto&& s){
			    	s << "Could not find edge of opcode" << op1 << " and opcode " << op2;
			    });
            } 
            return lower_bound_op_code_edge[{op1, op2}];
        }

        int getUpperBound(OpGraphOpCode op1, OpGraphOpCode op2) {
            if (upper_bound_op_code_edge.find({op1, op2}) == upper_bound_op_code_edge.end()) {
                throw make_from_stream<cgrame_model_error>([&](auto&& s){
			    	s << "Could not find edge of opcode" << op1 << " and opcode " << op2;
			    });
            } 
            return upper_bound_op_code_edge[{op1, op2}];
        }

        int getOpLatency(OpGraphOpCode op) {
        if (op_latency.find(op) == op_latency.end()) {
            throw make_from_stream<cgrame_model_error>([&](auto&& s){
                s << "Could not find opcode latency " << op;
            });
        }
        return op_latency[op];
    }
        // Gets the schedule of ASAP, ALAP or LRO depending on the schedule type
        std::unordered_map<const OpGraphOp*, int> getSchedule(SchedType type);

        // Not needed anymore
        int calcLowerBound(OpGraphOp* op);
        using opPair = std::pair<OpGraphOp*, OpGraphOp*>;
        std::unordered_map<opPair , int, pair_hash> extended_sched_const;

 private:
        const OpGraph& op_graph;
        const MRRG& mrrg;
        std::unordered_map<const OpGraphOp*, int> asap_schedule;
        std::unordered_map<const OpGraphOp*, int> alap_schedule;
        std::unordered_map<const OpGraphOp*, int> sdc_mod_schedule;
        std::unordered_map<const OpGraphOp*, int> sdc_lro_schedule;
        std::vector<const MRRGNode*> function_nodes;
        std::unordered_map<const OpGraphOp*, int> schedule;
        std::unordered_map<OpGraphOpCode, int> op_latency;
        std::unordered_map<OpCodeEdge, int, pair_hash> lower_bound_op_code_edge;
        std::unordered_map<OpCodeEdge, int, pair_hash> upper_bound_op_code_edge;
};

#endif  // INC_CGRA_OPSCHEDULER_H_
