#ifndef __FPUNITMODULES__H__
#define __FPUNITMODULES__H__

#include <CGRA/Module.h>

class FPAdd: public Module
{
  public:
    FPAdd(std::string name, Location, unsigned size); //module definition
    virtual ~FPAdd(); //class destructor
    virtual std::string GenericName() override; //generates generic name for adder
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for adder
};

class FPMult: public Module
{
  public:
    FPMult(std::string name, Location, unsigned size); //module definition
    virtual ~FPMult(); //class destructor
    virtual std::string GenericName() override; //generates generic name for multiplier
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for multiplier
};

class FPDiv: public Module
{
  public:
    FPDiv(std::string name, Location, unsigned size); //module definition
    virtual ~FPDiv(); //class destructor
    virtual std::string GenericName() override; //generates generic name for divider
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for divider
};

class FPSqrt: public Module
{
  public:
    FPSqrt(std::string name, Location, unsigned size); //module definition
    virtual ~FPSqrt(); //class destructor
    virtual std::string GenericName() override; //generates generic name for sqrt
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for sqrt
};

class InputIEEE2FloPoCo: public Module
{
  public:
    InputIEEE2FloPoCo(std::string name, Location, unsigned size); //module definition
    virtual ~InputIEEE2FloPoCo(); //class destructor
    virtual std::string GenericName() override; //generates generic name for input
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for input
};

class OutputFloPoCo2IEEE: public Module
{
  public:
    OutputFloPoCo2IEEE(std::string name, Location, unsigned size); //module definition
    virtual ~OutputFloPoCo2IEEE(); //class destructor
    virtual std::string GenericName() override; //generates generic name for output
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for output
};

class OutputConvert2Int: public Module
{
  public:
    OutputConvert2Int(std::string name, Location, unsigned size, std::vector<OpGraphOpCode> supported_modes); //module definition
    virtual ~OutputConvert2Int(); //class destructor
    virtual std::string GenericName() override; //generates generic name for OutputSelect
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for OutputSelect
    MRRG* createMRRG(unsigned II) override; //creates Modulo Routing Resource Graph
    static const std::map<OpGraphOpCode, LLVMMode> all_modes;
    auto& getSupportedModes() const { return supported_modes; }
  protected:
    unsigned size;
    std::vector<OpGraphOpCode> supported_modes;
};

class OutputConvert2FP: public Module
{
  public:
    OutputConvert2FP(std::string name, Location, unsigned size, std::vector<OpGraphOpCode> supported_modes); //module definition
    virtual ~OutputConvert2FP(); //class destructor
    virtual std::string GenericName() override; //generates generic name for OutputSelect
    virtual nlohmann::json CoreIRGenFunctionality() override; //generates functionality for OutputSelect
    MRRG* createMRRG(unsigned II) override; //creates Modulo Routing Resource Graph
    static const std::map<OpGraphOpCode, LLVMMode> all_modes;
    auto& getSupportedModes() const { return supported_modes; }
  protected:
    unsigned size;
    std::vector<OpGraphOpCode> supported_modes;
};


class FPUnit: public Module
{
  public:
    FPUnit(std::string name, Location, std::vector<OpGraphOpCode> supported_modes, unsigned size, int II, int latency); //module definition
    virtual ~FPUnit(); //class destructor
    virtual MRRG* createMRRG(unsigned II) override; //creates Modulo Routing Resource Graph
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
#endif
