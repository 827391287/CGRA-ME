#ifndef __ELASTICMODULES__H__
#define __ELASTICMODULES__H__

#include <CGRA/Module.h>

#define DEFAULT_EB_DEPTH 2
#define DEFAULT_FANIN_FANOUT 2
#define DEFAULT_LFORK_IMPL 1	// LKFork design

class ElasticBufferFifo: public Module
{
    public:
	ElasticBufferFifo(std::string name, Location, int depth_ = DEFAULT_EB_DEPTH, int size = DEFAULT_SIZE, bool hasEnable_ = true, bool isElastic = true);
	virtual ~ElasticBufferFifo();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;
	int getDepth() const {return eb_depth;};
	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;



    private:
	int eb_depth;
	bool hasEnable;
};

class ElasticEagerFork: public Module
{
    public: 
	ElasticEagerFork(std::string name, Location, int size, int fanout_ = DEFAULT_FANIN_FANOUT, int unit_fork_fanout_ = DEFAULT_FANIN_FANOUT, bool isElastic = true);
	virtual ~ElasticEagerFork();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;

	int getUnitFanout() const {return unit_fork_fanout;};
	int getFanout() const {return fanout;};
    private:
	int unit_fork_fanout;
	int fanout;
};

class ElasticForkBranch: public Module
{
    public: 
    static const std::map<OpGraphOpCode,LLVMMode> all_modes;
	ElasticForkBranch(std::string name, Location, int size, int fanout_ = DEFAULT_FANIN_FANOUT, bool isElastic = true);
	virtual ~ElasticForkBranch();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;

	int getFanout() const {return fanout;};
    private:
	int fanout;
};


class ElasticSelMultiplexer : public Module {
    public:
        static const std::map<OpGraphOpCode,LLVMMode> all_modes;
        ElasticSelMultiplexer(std::string, Location, unsigned mux_size, unsigned size = DEFAULT_SIZE, bool isElastic = false); // ("name", mux size, size)
        virtual ~ElasticSelMultiplexer();
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


class ElasticLazyFork: public Module
{
    public: 
	ElasticLazyFork(std::string name, Location, int size = DEFAULT_SIZE, int fanout_ = DEFAULT_FANIN_FANOUT, int unit_fork_fanout_ = DEFAULT_FANIN_FANOUT, int impl_type_ = DEFAULT_LFORK_IMPL);
	virtual ~ElasticLazyFork();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;
    private:
	int unit_fork_fanout;
	int fanout;
	int impl_type;
};


class ElasticJoin: public Module
{
    public:
	ElasticJoin(std::string name, Location, int size = DEFAULT_SIZE, int fanin_ = DEFAULT_FANIN_FANOUT, bool isElastic = true);
	virtual ~ElasticJoin();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;
    private:
	int fanin;
};


class ElasticDiverge: public Module
{
    public:
	ElasticDiverge(std::string name, Location, int size = DEFAULT_SIZE, int fanout_ = DEFAULT_FANIN_FANOUT);
	virtual ~ElasticDiverge();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;
    private:
	int fanout;
};


class ElasticMerge: public Module
{
    public:
	ElasticMerge(std::string name, Location, int size = DEFAULT_SIZE, int fanin_ = DEFAULT_FANIN_FANOUT);
	virtual ~ElasticMerge();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;
    private:
	int fanin;
};

class ElasticVLUWrapper: public Module
{
	public:
		ElasticVLUWrapper(std::string name, Location, int size = DEFAULT_SIZE, int input_count_ = 1);
		virtual ~ElasticVLUWrapper();
		virtual std::string GenericName() override;
		virtual nlohmann::json CoreIRGenFunctionality() override;
        MRRG* createMRRG(unsigned II) override;
	
	private:
		int input_count;
};

class FPUnit_wrapper: public Module
{
    public:
	FPUnit_wrapper(std::string name, Location, int max_latency, std::vector<OpGraphOpCode> supported_modes); //module definition
	virtual ~FPUnit_wrapper(); //class destructor
	virtual std::string GenericName() override; //generates generic name for wrapper
	virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for wrapper
	MRRG* createMRRG(unsigned II) override; //creates Modulo Routing Resource Graph
	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;
	auto& getSupportedModes() const { return supported_modes; }
	static const std::map<OpGraphOpCode, LLVMMode> all_modes;
    protected:
	std::vector<OpGraphOpCode> supported_modes;
	int max_latency;
};

class ElasticTokenInducer: public Module
{
    public:
	ElasticTokenInducer(std::string name, Location, int size = DEFAULT_SIZE, int iteration_interval = 2, bool isEalstic = true);
	virtual ~ElasticTokenInducer();
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;
	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;
	bool isNodeInCycle( const MRRGNode* originNode, const MRRGNode* thisNode, std::set<const MRRGNode*>& visited,std::set<const MRRGNode*>& mrrgnodes_mapped_to ) const;
	private: 
		int iteration_interval = 0;

};

class ElasticRegisterFile: public Module
{
	public:
		ElasticRegisterFile(std::string name, Location, int numInputPorts, int numOutputPorts, int log2Registers, int size = DEFAULT_SIZE, int contexts = 1, int eb_depth = DEFAULT_EB_DEPTH);
		virtual ~ElasticRegisterFile();
		virtual std::string GenericName() override;
		virtual nlohmann::json CoreIRGenFunctionality() override;
	
	private:
		int numInputPorts;
		int numOutputPorts;
		int log2Registers;
		int eb_depth;
};

class ElasticFuncUnit: public Module
{
    public:
        ElasticFuncUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency, bool predExist, bool isElastic, int contexts = 1);
        virtual std::string GenericName() override;
};

class ElasticFPUnit: public Module
{
    public:
        ElasticFPUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency, bool predExist, bool isElastic, int contexts = 1);
        virtual std::string GenericName() override;
};


class ElasticCrossbar: public Module
{
    public:
        ElasticCrossbar(std::string name, Location, int num_inputs, int num_outputs, int data_size, int type, bool predExist=false, std::string pred_type = "full", int contexts = 1);
        virtual std::string GenericName() override;
    private:
        int num_inputs;
        int num_outputs;
};

class ElasticMemPort: public Module
{
	public:
		ElasticMemPort(std::string name, Location, int size = DEFAULT_SIZE, int eb_depth = DEFAULT_EB_DEPTH, int num_connections = 2, bool pred = false, int contexts = 1);
		virtual std::string GenericName() override;
		virtual ~ElasticMemPort();

	private:
		int eb_depth;
		int num_connections;
};

class ElasticMemoryUnit: public Module {
    public:
	ElasticMemoryUnit(std::string name, Location, unsigned size = DEFAULT_SIZE, unsigned addr_size = DEFAULT_SIZE);
	virtual std::string GenericName() override;
	virtual nlohmann::json CoreIRGenFunctionality() override;
	MRRG* createMRRG(unsigned II) override;
	virtual ~ElasticMemoryUnit();

	virtual BitConfig getBitConfig(
		const MRRG& mrrg, const OpGraph & og,
		const Mapping& map,
		const ConfigCell& ccell,
		const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
		const MRRGNodesFromValNode& mrrg_nodes_from_val_node
		) const override;

    private:
	unsigned data_size;
	unsigned addr_size;
};

class ElasticConfigCell : public ConfigCell {
    public:
        ElasticConfigCell(std::string name, int iteration_interval = 1, Location loc = {0,0}); // Constructor of the configuration cell; initializes the name
       	virtual std::string GenericName() override; // returns the module name
        virtual nlohmann::json CoreIRGenFunctionality() override;
	private:
	int configCell_II = 0;
};

#endif
