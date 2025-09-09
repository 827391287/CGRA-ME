/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#pragma once

#include <CGRA/CGRA.h>
#include <CGRA/OpGraph.h>
#include <CGRA/Mapper.h>
#include <CGRA/Mapping.h>
#include <CGRA/Scheduler.h>
#include <CGRA/utility/CodeProfiling.h>

#include <map>
#include <memory>
#include <random>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <utility>
#include <bits/stdc++.h>  // NOLINT

using MRRGNodeDesc  = MRRG::NodeDescriptor;  // const MRRGNode*
using OpGraphOpDesc = OpGraph::OpDescriptor;  // const OpGraphOp*
using OpGraphValDesc = OpGraph::ValDescriptor;  // const OpGraphVal*
using Latency = int;

/**
 * @brief Proxy class that makes a new instance of the for ever call.
 * Holds constant configuration info,
 */
struct ClusteredMapperDriver : public Mapper {
    static constexpr Latency kUndefLatency = -1;

    ClusteredMapperDriver(std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args);
    /**
    *@brief main function to map the OpGraph to the device description
    */
    Mapping mapOpGraph(std::shared_ptr<OpGraph> opgraph,
      const int II,
      const MRRG& mrrg,
      std::unordered_map<std::string, std::string> fix_port) override;


 public:
    const int l_print;

 protected:
    Mapping mapStatic(std::shared_ptr<OpGraph> opgraph,
      const int II,
      const MRRG& mrrg,
      std::unordered_map<std::string, std::string> fix_port,
      PrintOnDestructionChronoSequence& timing_seq);
    Mapping mapElastic(std::shared_ptr<OpGraph> opgraph,
      const int II,
      const MRRG& mrrg,
      std::unordered_map<std::string, std::string> fix_port, 
      PrintOnDestructionChronoSequence timing_seq);
    const Latency getCyclesToSink(OpGraphOpDesc source, OpGraphOpDesc sink);
    bool isAllOpsSupported(const OpGraph& opgraph, const MRRG& mrrg); 
    void printPlacementNeato(int i, const OpGraph& opgraph, Mapping placed,  std::unordered_map<OpGraphOpDesc, int> sched);
    const int          l_rand_seed;
    const double       l_p_factor_factor;
    const double       l_initial_pfactor;
    const double       l_initial_hfactor;
    const double       l_h_factor_factor;
    const int          l_swap_factor;
    const int          l_fanout_threshold;
    const int          l_ram_ports;
    const int          l_verbosity;
    const bool         l_cluster;
    bool               l_isElastic;
    int                l_iteration_interval;
    const std::string  l_arch_name;
    const ConfigStore  l_mapper_args;
    const int          l_cost_func;
    Scheduler        *l_op_sched;
    std::vector<std::vector<OpGraphOpDesc>> l_clusters;
    std::map<OpGraphOpDesc, int> l_op_cluster_index;
    std::unordered_map<OpGraphOpDesc, std::vector<std::string>> l_ops_to_node_names;
    std::unordered_map<OpGraphOpDesc, int> l_schedule;
    Mapping l_empty_mapping_result;
};


struct NodeAttributes {
  NodeAttributes() : occupancy(0), hCost(0.0), baseCost(1)
  {}
  int occupancy = 0;
  double baseCost = 0.0;
  double hCost = 0.0;  // historical cost
  std::vector<OpGraphValDesc> mapped_values {};
};

struct BoundingBox {
  int x_max = 0;
  int x_min = 0;
  int y_max = 0;
  int y_min = 0;
  int z_max = 0;
  int z_min = 0;
};

/**
 * @brief Implementation of the
 *
 * Instatiated for every mapping call.
 * Inherits from the driver to get easy access to configuration members.
 */
class PathFinder : ClusteredMapperDriver {
 public:
  static constexpr Latency kUndefLatency = -1;

  PathFinder(ClusteredMapperDriver driver,
      const OpGraph& opgraph,
      const MRRG& mrrg,
      bool isElastic = false,
      const std::string placement_filename = "");
    Mapping routeOpGraph(Mapping placement);

    struct UnmappableException : std::runtime_error { using runtime_error::runtime_error; };
    //void printCriticalPath();
 private:
    void parsePlacementFile();
    void printNumberOfResourcesUsed();

    bool routeVal(OpGraphValDesc);
    std::vector<const MRRGNode*> dijkstraVisit(const MRRGNode*,
            const MRRGNode* , OperandTag,
            OpGraphValDesc, OpGraphOpDesc, 
            std::set<const MRRGNode*>, int, int);
    void mapMRRGNode(OpGraphValDesc, const MRRGNode*);

    bool isOpgraphCovered();

    bool checkOveruse();

    double getCost(const MRRGNode*);
    void ripUpOpVal(OpGraphValDesc);

    void setMappingInto(Mapping& m) const {  // NOLINT
        for (const auto& op_and_mapping : l_mapping) {
            std::vector<MRRGNodeDesc> as_mrrg;
            for (const auto& n : op_and_mapping.second) as_mrrg.push_back(n);
            m.setNodeMapping(op_and_mapping.first, std::move(as_mrrg));
        }
    }

    bool            l_isElastic;
    double           l_p_factor;
    double           l_h_factor;
    const MRRG&     l_mrrg;
    const OpGraph&  l_opgraph;
    std::string     l_placement_filename;
    std::vector<MRRGNodeDesc>                               l_critical_path;
    std::set<MRRGNodeDesc>                                  l_used_rounting_function_nodes;
    std::map<const OpGraphNode*, const MRRGNode*>           l_placement;
    std::unordered_map<const MRRGNode*, NodeAttributes>     l_routing_nodes;
    std::map<OpGraphValDesc, std::vector<const MRRGNode*>>  l_mapping = {};
};

class TMPack : ClusteredMapperDriver {
 public:
  static constexpr Latency kUndefLatency = -1;
  TMPack(ClusteredMapperDriver &driver,
    Module* topLevelModule,
    std::shared_ptr<OpGraph> opgraph,
    const MRRG& mrrg,
    int rows, int cols,
    int II, bool isElastic = false);

  using OpsNotToCluster = std::pair<OpGraphOpDesc, OpGraphOpDesc>;
  // Sets clusters for memory operations that needs to be set to the same memory port
  void clusterMemoryOperations(std::set<OpGraphOpDesc>*);

  // Gets the operations that have no inputs
  void getFirstOps(std::queue<OpGraphOpDesc>& operations);

  bool clusterPEs(bool cluster);
  void createSingleOpClusters();
  bool checkIfOutputPortsNeeded(OpGraphValDesc val, std::vector<OpGraphOpDesc> &ops);
  bool checkIfInputPortsNeeded(OpGraphOpDesc source_op, std::vector<OpGraphOpDesc> &ops);
  bool checkPortConn(std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>>& global_port_connection, int i,
        std::set<MRRGNodeDesc> ports_used, 
        std::set<OpGraphOpDesc> out_port_found,
        std::map<OpGraphOpDesc, std::set<MRRGNodeDesc>>& op_to_port_nodes, 
        std::string submodule, bool isInput);
  bool checkPortsOfOperation(std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>> global_port_connection, std::map<OpGraphOpDesc, std::vector<MRRGNodeDesc>>& ops_mrrg_nodes,
        std::set<OpGraphOpDesc> exlusive_port_operation, std::string submodule, bool isInput);
  bool checkConnectedOperations(std::pair<OpGraphOpDesc, OpGraphOpDesc>, std::map<OpGraphOpDesc, std::vector<MRRGNodeDesc>>& ops_mrrg_nodes,
        std::string submodule);
  /*bool checkInputsOfOperation(OpGraphOpDesc val, std::vector<OpGraphOpDesc> &ops, std::vector<MRRGNodeDesc>& source_op_nodes,
        std::string submodule);
  bool checkOutputsOfOperation(OpGraphOpDesc val, std::vector<OpGraphOpDesc> &ops, std::vector<MRRGNodeDesc>& source_op_nodes,
  std::string submodule);
  */
  void setSourceOpNodes(std::vector<MRRGNodeDesc>& source_op_nodes, OpGraphOpDesc source_op, std::string submodule);
  void checkIfAllOpsAreClustered();
  void getModulesToMapOp(OpGraphOpDesc op, std::map<std::string, MRRGNodeDesc>& modules_could_map_op);
  void setReachesOutputPorts(std::map<MRRGNodeDesc, std::set<MRRGNodeDesc>>& connected_nodes,
                std::vector<MRRGNodeDesc>& op_mrrg_nodes, std::string submodule);
  void setReachesInputPorts(std::map<MRRGNodeDesc, std::set<MRRGNodeDesc>>& connected_nodes,
                std::vector<MRRGNodeDesc>& op_mrrg_nodes, std::string submodule);
  void addNextOps(std::vector<OpGraphOpDesc>* next_ops, OpGraphOpDesc op);
  bool checkReachbitlity(std::vector<OpGraphOpDesc>&, std::string);
  void setPortsForSubmodule(std::string);
  bool isReachable(MRRGNodeDesc source, MRRGNodeDesc sink, std::string, Latency cycles = -1, OpGraphValDesc val = nullptr);
  void resolveReachability(OpGraphOpDesc source, OpGraphOpDesc sink, OpGraphVal* val);
  void opsCouldBeClustered(
        std::vector<OpGraphOpDesc> nextOps, std::vector<MRRGNodeDesc> &used_nodes, //NOLINT
        std::vector<OpGraphOpDesc> &used_ops, std::set<OpGraphOpDesc> &visitedOps, std::string subModule); //NOLINT

  // Getters and setters for different variables of the class
  std::unordered_map<OpGraphOpDesc, std::vector<std::string>> getOpsNodeNames() {return l_ops_to_node_names;}
  std::vector<std::vector<OpGraphOpDesc>> getClusters() {return l_clusters;}
  std::map<OpGraphOpDesc, int> getOpClusterIndex() {return l_op_cluster_index;}
  void printOps (std::vector<OpGraphOpDesc>& ops);
  void printNodes (std::vector<MRRGNodeDesc>& mrrg_nodes);
  void setSchedule(std::unordered_map<OpGraphOpDesc, int>  s) {l_schedule = s;}
  void unsetReschedule() {l_reschedule = false;}
  using opPair = std::pair<OpGraphOp*, OpGraphOp*>;
  std::unordered_map<opPair , int, pair_hash> extended_sched_const;

 private:
  bool l_reschedule = false;
  MRRG* localMRRG;
  int  l_II;
  int  l_rows;
  int  l_cols;
  bool l_isElastic;
  std::shared_ptr<OpGraph>                                       l_opgraph;
  const MRRG&     l_mrrg;
  std::set<OpsNotToCluster>                                      l_ops_not_clustered;
  std::map<std::string, MRRG*>                                   l_subModule_MRRGs;
  std::unordered_set<OpGraphOpDesc>                              l_clustered_ops;
  std::map<std::string, std::vector<MRRGNodeDesc>>               l_function_nodes;
  std::map<std::string, std::vector<MRRGNodeDesc>>               l_routing_nodes;
  std::map<std::string, std::set<MRRGNodeDesc>>                  l_submodule_input_ports;
  std::map<std::string, std::set<MRRGNodeDesc>>                  l_submodule_output_ports;
  std::map<std::string, std::vector<std::vector<OpGraphOpDesc>>> l_mem_name_ops;
};

/**
 * @brief Implementation of the Anneal Placer
 *
 * Instatiated for every mapping call.
 * Inherits from the driver to get easy access to configuration members.
 */
class AnnealPlacer : ClusteredMapperDriver {
 public:
    AnnealPlacer(ClusteredMapperDriver driver,
      const OpGraph& opgraph,
      const MRRG& mrrg,
      std::unordered_map<std::string, std::string> fix_ports,
      int rows,
      int cols,
      bool isElastic = false);
    // ~AnnealPlacer();
    Mapping placeOpGraph(float temp);

    struct UnmappableException : std::runtime_error { using runtime_error::runtime_error; };
    void clearPlacement();
    void setInitialPlacement();
    float determineTemperature(float scale);

 private:
    /** @brief The main body of one iteration */
    std::pair<bool, float> inner_place_and_route_loop(float temp);

    /** @brief gets the functional units to map the op */
    MRRGNodeDesc getRandomUnoccupiedFU(OpGraphOpDesc op);
    MRRGNodeDesc getRandomFU(OpGraphOpDesc op);
    std::vector<MRRGNodeDesc> getRandomFUs(std::vector<OpGraphOpDesc> ops, bool occupied);
    MRRGNodeDesc getFUForOpAtLoc(OpGraphOpDesc op, Location loc);

    /** @brief places and removes an op from an mrrgnode*/
    void ripUpOp(OpGraphOpDesc op);
    bool placeOp(OpGraphOpDesc op, MRRGNodeDesc n);

    /** @brief gets the mrrgnode associated with an op or the op associated with an mrrgnode */
    MRRGNodeDesc getMappedMRRGNode(OpGraphOpDesc op);
    OpGraphOpDesc getOpMapedAt(MRRGNodeDesc n);

    /** @brief @param cluster checks if the cluster has a fixed port or no
     * @return true if it does contain a fixed port
     * @return false if it doesn't contain a fixed port
     */
    bool isClusterFixed(std::vector<OpGraphOpDesc> cluster);

    /** @brief Should a move with this @p delta_cost be accepted at this @p temperature? */
    bool accept(float delta_cost, float temperature) const;
    float nextTemperature(float t, float accept_rate) const;

    /** @brief Checks if the opgraph mapping is valid */
    bool isOpgraphCovered() const;
    bool checkOveruse();
    void assertOccupancyLimit(MRRGNodeDesc fu);

    /** @brief does the costing of the fopgraph   */
    BoundingBox getBB(OpGraphValDesc);
    double getTotalCost();
    void swap(OpGraphOpDesc op, MRRGNodeDesc node, std::map<OpGraphOpDesc, MRRGNodeDesc>& old_placement);


    void setMappingInto(Mapping& m) const { // NOLINT
        for (const auto& op_and_mapping : l_mapping) {
            std::vector<MRRGNodeDesc> as_mrrg;
            for (const auto& n : op_and_mapping.second) as_mrrg.push_back(n);
            m.setNodeMapping(op_and_mapping.first, std::move(as_mrrg));
        }
    }
    void printMapping() const { printMapping(std::cout); } // makes it easier to call from a debugger NOLINT
    void printMapping(std::ostream& os) const {
        auto cpy = l_empty_mapping_result;
        setMappingInto(cpy);
        cpy.outputMapping(os);
    }

    int             l_rows = 0;
    int             l_cols = 0;
    int             l_II;
    bool            l_isElastic;
    const MRRG&     l_mrrg;
    const OpGraph&  l_opgraph;

    using OpCodeByCycle = std::pair<OpGraphOpCode, int>;

    std::set<MRRGNodeDesc>                                                  l_fixed_mrrg_nodes;
    mutable std::minstd_rand                                                l_randomInt;
    std::unordered_map<std::string, std::string>                            l_fix_ports;
    std::map<const MRRGNode*, NodeAttributes>                               l_function_nodes;
    std::map<OpGraph::NodeDescriptor, std::vector<MRRGNodeDesc>>            l_mapping = {};
    std::map<OpGraphOpCode, std::vector<std::vector<MRRGNodeDesc>>>         l_grid;
    std::unordered_map<OpCodeByCycle, std::vector<MRRGNodeDesc>, pair_hash> l_op_to_nodes;
    
    std::size_t randomUintBelow(std::size_t past_end) const { return l_randomInt() % past_end; }  // want speed
    double randomRealBetween0And1() const { return l_randomInt() / static_cast<double>(l_randomInt.max()); }  // want speed
};
