//CGRAME Includes
#include "CGRA/dotparse.h"
#include <CGRA/utility/ConfigGraph.h>
#include "CGRA/Util.h"
#include <CGRA/OpGraph.h>
#include <CGRA/user-inc/UserArchs.h>
#include <CGRA/CGRA.h>
#include <CGRA/MRRGProcedures.h>

//Cpp includes
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <unordered_set>
#include <cmath>

// Define your OpCodeEdge and OpGraphOpCode types and pair_hash function here
using OpCodeEdge = std::pair<OpGraphOpCode, OpGraphOpCode>;

class Scheduler {
public:
    std::unordered_map<const OpGraphOp*, int> asap_schedule;
    std::unordered_map<const OpGraphOp*, int> alap_schedule;
    std::unordered_map<const OpGraphOp*, int> fixed_schedule;
    
    std::unordered_map<OpCode, int> resourcesByOpCode;
    int max_cycles;

    Scheduler(OpGraph* opGraph, std::string arch_name);

    bool calculateASAP();
    bool calculateALAP();
    std::unordered_map<const OpGraphOp*, int> initializeSched();
    
    int getLowerBound(OpGraphOpCode op1, OpGraphOpCode op2) {
        if (lower_bound_op_code_edge.find({op1, op2}) == lower_bound_op_code_edge.end()) {
            throw make_from_stream<cgrame_model_error>([&](auto&& s){
                s << "Could not find edge of opcode " << op1 << " -> " << op2;
            });
        } 
        return lower_bound_op_code_edge[{op1, op2}];
    }

    int getUpperBound(OpGraphOpCode op1, OpGraphOpCode op2) {
        if (upper_bound_op_code_edge.find({op1, op2}) == upper_bound_op_code_edge.end()) {
            throw make_from_stream<cgrame_model_error>([&](auto&& s){
                s << "Could not find edge of opcode " << op1 << " -> " << op2;
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
private:
    OpGraph* opGraph;
    CGRA* arch;
    std::unordered_map<OpCodeEdge, int, pair_hash> lower_bound_op_code_edge;
    std::unordered_map<OpCodeEdge, int, pair_hash> upper_bound_op_code_edge;
    std::unordered_map<OpGraphOpCode, int> op_latency;


    std::unordered_set<OpCode> aluOpCodes = {
        OpCode::ADD,
        OpCode::SUB,
        OpCode::MUL,
        OpCode::DIV,
        OpCode::AND,
        OpCode::OR,
        OpCode::XOR,
        OpCode::SHL,
        OpCode::ASHR,
        OpCode::LSHR,
        OpCode::ICMP,
        OpCode::ICMP,
        OpCode::CMP,
        OpCode::SQRT,
        OpCode::FADD,
        OpCode::FMUL,
        OpCode::FDIV,
        OpCode::FP2INT,
        OpCode::SELECT,
        OpCode::INT2FP
    };


    void parse_latency_file(std::string arch_name);

    //General scheduling:
    std::vector<OpGraphOp*> rescheduleStore(OpGraphOp *load, OpGraphOp *store, int dist);
    std::vector<OpGraphOp*> reschedulePredicateEdge(OpGraphOp *pred, OpGraphOp *output);
    //Verification
    bool verifyDependenceConstrainedSchedule(std::unordered_map<const OpGraphOp*, int> sched);
    //ASAP Scheduling:
    int ASAPII;
    int scheduleASAPOperation(OpGraphOp* op);
    bool rescheduleASAP(OpGraphOp* op, int cycle);
    std::vector<OpGraphOp*> verifyBoundsASAP(OpGraphOp* op);
    int unscheduleASAP(OpGraphOp* op, std::vector<OpGraphOp*>* rescheduledOps);
    int rescheduleForward(OpGraphOp* op, OpGraphOp* consumer,std::vector<OpGraphOp*>* rescheduledOps);
    //ALAP Scheduling:
    int scheduleALAPOperation(OpGraphOp* op);
    int verifyBoundsALAP(OpGraphOp* op);
    int unscheduleALAP(OpGraphOp* op);
    int rescheduleBackward(OpGraphOp* op, OpGraphOp* producer);


};
