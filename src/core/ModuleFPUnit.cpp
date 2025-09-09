#include <CGRA/Module.h>
#include <CGRA/ModuleFPUnit.h>
#include <regex>

//Module Definition for Adder
FPAdd::FPAdd(std::string name, Location loc, unsigned size) 
  : Module(name, loc, size)
{
  adds_synchronous_circuitry = true; //Adder has sequential elements

  //Create ports

  addPort("X", PORT_INPUT, "size", size + 2);
  addPort("Y", PORT_INPUT, "size", size + 2);
  addPort("R", PORT_OUTPUT, "size", size + 2);
  addPort("FP_clk_en", PORT_INPUT, 1);

}

//Virtual function that override Module::GenericName. Returns generic name for the adder
std::string FPAdd::GenericName()
{
  return "FPAdder";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the adder
nlohmann::json FPAdd::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";

  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "FPAdd FPAdder (\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".ce(~FP_clk_en),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(X),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".Y(Y),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(R)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
FPAdd::~FPAdd()
{
  //Empty Destructor
}




//Module Definition for Multiplier
FPMult::FPMult(std::string name, Location loc, unsigned size) 
  : Module(name, loc, size)
{
  adds_synchronous_circuitry = true; //Multiplier has sequential elements

  //Create ports

  addPort("X", PORT_INPUT, "size", size + 2);
  addPort("Y", PORT_INPUT, "size", size + 2);
  addPort("R", PORT_OUTPUT, "size", size + 2);
  addPort("FP_clk_en", PORT_INPUT, 1);
}

//Virtual function that override Module::GenericName. Returns generic name for the Multiplier
std::string FPMult::GenericName()
{
  return "FPMultiplier";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the Multiplier
nlohmann::json FPMult::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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
  
  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";

  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "FPMult FPMultiplier (\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".ce(~FP_clk_en),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(X),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".Y(Y),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(R)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
FPMult::~FPMult()
{
  //Empty Destructor
}




//Module Definition for Divider
FPDiv::FPDiv(std::string name, Location loc, unsigned size) 
  : Module(name, loc, size)
{
  adds_synchronous_circuitry = true; //Divider has sequential elements

  //Create ports

  addPort("X", PORT_INPUT, "size", size + 2);
  addPort("Y", PORT_INPUT, "size", size + 2);
  addPort("R", PORT_OUTPUT, "size", size + 2);
  addPort("FP_clk_en", PORT_INPUT, 1);
}

//Virtual function that override Module::GenericName. Returns generic name for the Divider
std::string FPDiv::GenericName()
{
  return "FPDivider";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the Divider
nlohmann::json FPDiv::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";

  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "FPDiv FPDivider (\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".ce(~FP_clk_en),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(X),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".Y(Y),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(R)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
FPDiv::~FPDiv()
{
  //Empty Destructor
}




//Module Definition for Sqrt
FPSqrt::FPSqrt(std::string name, Location loc, unsigned size) 
  : Module(name, loc, size)
{
  adds_synchronous_circuitry = true; //Sqrt has sequential elements

  //Create ports

  addPort("X", PORT_INPUT, "size", size + 2);
  addPort("R", PORT_OUTPUT, "size", size + 2);
  addPort("FP_clk_en", PORT_INPUT, 1);
}

//Virtual function that override Module::GenericName. Returns generic name for the Sqrt
std::string FPSqrt::GenericName()
{
  return "FPSqrt";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the Sqrt
nlohmann::json FPSqrt::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";

  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "FPSqrt FPSqroot (\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".ce(~FP_clk_en),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(X),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(R)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
FPSqrt::~FPSqrt()
{
  //Empty Destructor
}

//Module Definition for Input
InputIEEE2FloPoCo::InputIEEE2FloPoCo(std::string name, Location loc, unsigned size) 
  : Module(name, loc, size)
{
  adds_synchronous_circuitry = true; //Input has sequential elements

  //Create ports

  addPort("X", PORT_INPUT, "size_in", size);
  addPort("R", PORT_OUTPUT, "size_out", size + 2);
}

//Virtual function that override Module::GenericName. Returns generic name for the Input
std::string InputIEEE2FloPoCo::GenericName()
{
  return "FPInput";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the Input
nlohmann::json InputIEEE2FloPoCo::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "InputIEEE2FloPoCo FPInput (\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(X),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(R)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
InputIEEE2FloPoCo::~InputIEEE2FloPoCo()
{
  //Empty Destructor
}

//Module Definition for Output
OutputFloPoCo2IEEE::OutputFloPoCo2IEEE(std::string name, Location loc, unsigned size) 
  : Module(name, loc, size)
{
  adds_synchronous_circuitry = true; //Output has sequential elements

  //Create ports
  addPort("X", PORT_INPUT, "size_in", size + 2);
  addPort("R", PORT_OUTPUT, "size_out", size);
}

//Virtual function that override Module::GenericName. Returns generic name for the Output
std::string OutputFloPoCo2IEEE::GenericName()
{
  return "FPOutput";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the Output
nlohmann::json OutputFloPoCo2IEEE::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "OutputFloPoCo2IEEE FPOutput (\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(X),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(R)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
OutputFloPoCo2IEEE::~OutputFloPoCo2IEEE()
{
  //Empty Destructor
}


const std::map<OpGraphOpCode, LLVMMode> OutputConvert2FP::all_modes =
{
    {OpCode::INT2FP,    {"op_fp2int", "int2fp",    {"5"},      "int2fp_sel"}},
};


//Module Definition for FP2Fix
OutputConvert2FP::OutputConvert2FP(std::string name, Location loc, unsigned size, std::vector<OpGraphOpCode> supported_modes_) 
  : Module(name, loc, size), supported_modes(std::move(supported_modes_))
{
    adds_synchronous_circuitry = true; //FP2Fix has sequential elements

    //Create ports
    addPort("data_in", PORT_INPUT, "size", size);
    addPort("data_out", PORT_OUTPUT, "size", size);
    addPort("valid_upstream", PORT_INPUT, 1);
    addPort("stop_downstream", PORT_INPUT, 1);
    addPort("stop_upstream", PORT_OUTPUT_REG, 1);
    addPort("valid_downstream", PORT_OUTPUT_REG, 1);

    node_relative_position.insert({"data_in", {0.5, 0}});
    node_relative_position.insert({"int2fp", {0.5, 0.33}});
    node_relative_position.insert({"data_out", {0.5, 0.67}});
}

//Virtual function that override Module::GenericName. Returns generic name for the FP2Fix
std::string OutputConvert2FP::GenericName()
{
  return "OutputConvert2FP";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the FP2Fix
nlohmann::json OutputConvert2FP::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  {
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "wire [size+1:0] out_int2fp;\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [1:0] register;\n";
    moduleDefinition += "\n";

    moduleDefinition += std::string(SET_INDENT) + "Fix2FP int2fp(\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".ce(~stop_downstream),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".I(data_in),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".O(out_int2fp)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    moduleDefinition += std::string(SET_INDENT) + "OutputFloPoCo2IEEE IEEE2FP(\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(out_int2fp),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(data_out)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (~stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= {valid_upstream, register[1:1]};\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 1;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= register;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";
    moduleDefinition += "\n";
    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (~register[0]) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (register[0]) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 1;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
OutputConvert2FP::~OutputConvert2FP()
{
  //Empty Destructor
}

MRRG* OutputConvert2FP::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i+= II)
    {
        // create nodes
        MRRG::NodeDescriptor datain_result = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "data_in", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;

        MRRG::NodeDescriptor int2fp  = result.insert(MRRGNode::make_function(this, data_size, i, "int2fp", 0, supported_modes)).first;

        MRRG::NodeDescriptor dataout = result.insert(MRRGNode::make_routing(this, data_size, i, "data_out")).first;

        result.link(datain_result, int2fp);
        result.link(int2fp, dataout);

    }

    return result_ptr;
}




const std::map<OpGraphOpCode, LLVMMode> OutputConvert2Int::all_modes = 
{  
    {OpCode::FP2INT,    {"op_fp2int", "fp2int",    {"1"},      "fp2int_sel"}},
};


//Module Definition for FP2Fix
OutputConvert2Int::OutputConvert2Int(std::string name, Location loc, unsigned size, std::vector<OpGraphOpCode> supported_modes_) 
  : Module(name, loc, size), supported_modes(std::move(supported_modes_))
{
    adds_synchronous_circuitry = true; //FP2Fix has sequential elements

    //Create ports
    addPort("data_in", PORT_INPUT, "size", size);
    addPort("data_out", PORT_OUTPUT, "size", size);
    addPort("valid_upstream", PORT_INPUT, 1);
    addPort("stop_downstream", PORT_INPUT, 1);
    addPort("stop_upstream", PORT_OUTPUT_REG, 1);
    addPort("valid_downstream", PORT_OUTPUT_REG, 1);

    node_relative_position.insert({"data_in", {0.5, 0}});
    node_relative_position.insert({"fp2int", {0.5, 0.33}});
    node_relative_position.insert({"data_out", {0.5, 0.67}});
}

//Virtual function that override Module::GenericName. Returns generic name for the FP2Fix
std::string OutputConvert2Int::GenericName()
{
  return "FPOutputConvert2Int";
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the FP2Fix
nlohmann::json OutputConvert2Int::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
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

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
  for (auto& port : ports)
  {
    port_type portType = port.second->pt;
    std::string portTypeString = {};
    if(portType == port_type::PORT_INPUT)
    {
      portTypeString = "input";
    }
    else if (portType == port_type::PORT_OUTPUT)
    {
      portTypeString = "output";
    }
    else if (portType == port_type::PORT_OUTPUT_REG)
    {
      portTypeString = "output reg";
    }
    else
    {
      portTypeString = "inout";
    }
    std::string portSizeString;
    if (!(port.second->parameter).empty()) // Check if size is parameterized
    {
      std::string portParameterName = port.second->parameter;
      portSizeString = "[" + portParameterName + "-1:0]";
    }
    else
    {
      portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
    }
    std::string portName = port.second->getName();
    std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
    moduleDefinition += std::string(SET_INDENT) + portDeclaration;
  }

  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "wire [size+1:0] out_fp2int;\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [1:0] register;\n";
    moduleDefinition += "\n";
    moduleDefinition += std::string(SET_INDENT) + "InputIEEE2FloPoCo ieee2fp(\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".X(data_in),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".R(out_fp2int)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";
    moduleDefinition += "\n";
    moduleDefinition += std::string(SET_INDENT) + "FP2Fix fp2int(\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".rst(CGRA_Reset),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".ce(~stop_downstream),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".I(out_fp2int),\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + ".O(data_out)\n";
    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (~stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= {valid_upstream, register[1:1]};\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= register;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 1;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";
    moduleDefinition += "\n";
    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (~register[0]) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (register[0]) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 1;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
OutputConvert2Int::~OutputConvert2Int()
{
  //Empty Destructor
}

MRRG* OutputConvert2Int::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i+= II)
    {
	// create nodes
	MRRG::NodeDescriptor datain_result = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "data_in", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;

	MRRG::NodeDescriptor fp2int  = result.insert(MRRGNode::make_function(this, data_size, i, "fp2int", 0, supported_modes)).first;

	MRRG::NodeDescriptor dataout = result.insert(MRRGNode::make_routing(this, data_size, i, "data_out")).first; 

	result.link(datain_result, fp2int);
	result.link(fp2int, dataout);

    }

    return result_ptr;
}

const std::map<OpGraphOpCode, LLVMMode> FPUnit::all_modes =
{ 
 {OpCode::FADD,    {"op_add",      "add",         {"8"},      "add_sel"}}, // janders was 7 from Adham originally
  {OpCode::FMUL,    {"op_multiply", "multiply",    {"0"},      "mul_sel"}},
  {OpCode::FDIV,    {"op_divide",   "divide",      {"20"},     "div_sel"}},
  {OpCode::SQRT,   {"op_sqrt",    "sqrt",         {"17"},     "sqrt_sel"}},
};

std::string FPUnit::GenericName()
{
  std::string NameToReturn = "fpunit_" + std::to_string(getSize()) + "b"; // Base name is "func"
  for (unsigned i = 0; i < supported_modes.size(); i++)
  {
    // Add a "_(module_name_inside)" to the name
    NameToReturn.append("_");
    NameToReturn.append(all_modes.at(supported_modes.at(i)).OpName);
  }
  return NameToReturn;
}

FPUnit::FPUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency) 
  : Module(name, loc, size), pipeline_mode(II, latency), supported_modes(std::move(supported_modes_))
{
  if (II != 1) {
    std::cout << II << '\n';
    make_and_throw<cgrame_error>([&](auto&& s) { s << "dont support an II other than 1 (given II=" << II << ')'; });
  }

  parameterlist["latency"] = latency;

  // Create the ports
  addPort("in_a", PORT_INPUT, "size", size);
  addPort("in_b", PORT_INPUT, "size", size);
  addPort("out", PORT_OUTPUT, "size", size);

  if (supported_modes.size() > 1) // If there is more than one mode, we need to add a select line, we will also have a mux, so make port "out" a reg
  {
    addPort("opcode", PORT_INPUT, ceil(log2(supported_modes.size())));
  }
  addPort("FP_clk_en", PORT_INPUT, 1);
  
  addSubModule(new InputIEEE2FloPoCo("fpinput1", loc, size), 0.5, 0, 0.1, 0.5);
  addSubModule(new InputIEEE2FloPoCo("fpinput2", loc, size), 1.0, 0, 0.1, 0.5);

  for (unsigned i = 0; i < supported_modes.size(); i++)
  {
    if (all_modes.at(supported_modes.at(i)).ModuleName == "op_add"){
      addSubModule(new FPAdd("fpadd", loc, size), 0, 0.5, 0.1, 0.5);
    }
    else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_multiply"){
      addSubModule(new FPMult("fpmult", loc, size), 0.5, 0.5, 0.1, 0.5);
    }
    else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_divide"){
      addSubModule(new FPDiv("fpdiv", loc, size), 1.0, 0.5, 0.1, 0.5);
    }
    else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_sqrt"){
      addSubModule(new FPSqrt("fpsqroot", loc, size), 1.5, 0.5, 0.1, 0.5);
    }
  }
  
  if (supported_modes.size() > 1) 
  {
	  addSubModule(new Multiplexer("result_mux", loc, supported_modes.size(), size + 2), 0.75, 1.0, 0.1, 0.5);
  }
  addSubModule(new OutputFloPoCo2IEEE("fpoutput", loc, size), 0.75, 1.5, 0.1, 0.5);

  addConnection("this.in_a", "fpinput1.X");
  addConnection("this.in_b", "fpinput2.X");

  for (unsigned i = 0; i < supported_modes.size(); i++)
  {
    if (all_modes.at(supported_modes.at(i)).ModuleName == "op_add"){
      addConnection("fpinput1.R", "fpadd.X");
      addConnection("fpinput2.R", "fpadd.Y");
      addConnection("this.FP_clk_en", "fpadd.FP_clk_en");
    }
    else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_multiply"){
      addConnection("fpinput1.R", "fpmult.X");
      addConnection("fpinput2.R", "fpmult.Y");
      addConnection("this.FP_clk_en", "fpmult.FP_clk_en");
    }
    else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_divide"){
      addConnection("fpinput1.R", "fpdiv.X");
      addConnection("fpinput2.R", "fpdiv.Y");
      addConnection("this.FP_clk_en", "fpdiv.FP_clk_en");
    }
    else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_sqrt"){
      addConnection("fpinput1.R", "fpsqroot.X");
      addConnection("this.FP_clk_en", "fpsqroot.FP_clk_en");
    }
  }
  if (supported_modes.size() > 1) {
    for (unsigned i = 0; i < supported_modes.size(); i++) {
      if (all_modes.at(supported_modes.at(i)).ModuleName == "op_add"){
        addConnection("fpadd.R", "result_mux.in" + std::to_string(i));
      }
      else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_multiply"){
        addConnection("fpmult.R", "result_mux.in" + std::to_string(i));
      }
      else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_divide"){
        addConnection("fpdiv.R", "result_mux.in" + std::to_string(i));
      }
      else if (all_modes.at(supported_modes.at(i)).ModuleName == "op_sqrt"){
        addConnection("fpsqroot.R", "result_mux.in" + std::to_string(i));
      }
    }
  }
  else if (supported_modes.size() == 1) 
  {
    if (all_modes.at(supported_modes.at(0)).ModuleName == "op_add"){
      addConnection("fpadd.R", "fpoutput.X");
    }
    else if (all_modes.at(supported_modes.at(0)).ModuleName == "op_multiply"){
      addConnection("fpmult.R", "fpoutput.X");
    }
    else if (all_modes.at(supported_modes.at(0)).ModuleName == "op_divide"){
      addConnection("fpdiv.R", "fpoutput.X");
    }
    else if (all_modes.at(supported_modes.at(0)).ModuleName == "op_sqrt"){
      addConnection("fpsqroot.R", "fpoutput.X");
    }
  }
  if (supported_modes.size() > 1) 
  {
	  addConnection("result_mux.select", "this.opcode");
	  addConnection("result_mux.out", "fpoutput.X");
  }
  addConnection("fpoutput.R", "this.out");

  node_relative_position.insert({"in_a", {0.33, 0}});
  node_relative_position.insert({"in_b", {0.67, 0}});
  node_relative_position.insert({"fpunit", {0.5, 0.33}});
  node_relative_position.insert({"out", {0.5, 0.67}});
}

BitConfig FPUnit::getBitConfig(
  const MRRG& mrrg, const OpGraph & og,
  const Mapping& map,
  const ConfigCell& ccell,
  const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
  const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
  (void)ccell;
  (void)mrrg_nodes_from_val_node;
  const auto bits_needed = std::lround(ceil(log2(supported_modes.size())));

  BitConfig bitConfig(mrrg.initiationInterval());

  // Organize nodes mapping by cycle
  std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
  for (const auto& op_and_mrrg_nodes : mrrg_nodes_from_op_node) {
    for (const auto& mrrg_node : op_and_mrrg_nodes.second) {
      opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
    }
  }

  int cycle = 0;
  for (auto & op_and_mrrg_nodes : opNodesByCycle) {
    if (op_and_mrrg_nodes.empty()) {
      bitConfig.add({ (size_t)bits_needed, BitSetting::DONT_CARE_PREFER_LOW }, cycle);
    } else if (op_and_mrrg_nodes.size() == 1) {
        const auto find_result = std::find(begin(supported_modes), end(supported_modes), begin(op_and_mrrg_nodes)->first->opcode);
        if (find_result == end(supported_modes)) {
          throw cgrame_error("couldn't find op in supported modes list");
        } else {
            bitConfig.add( bitsettings_from_int(std::distance(begin(supported_modes), find_result), bits_needed), cycle);
        }

    } else {
        throw cgrame_error("expect either 0 or 1 op nodes");
    }
    ++cycle;
  }

  return bitConfig;
}

FPUnit::~FPUnit()
{
}

MRRG* FPUnit::createMRRG(unsigned II = 1)
{
  MRRG* result_ptr = new MRRG(II);
  auto& result = *result_ptr;

  for(unsigned i = 0; i < II; i+= getII())
  {
    // create nodes
    MRRG::NodeDescriptor in_a = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_a", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;
    MRRG::NodeDescriptor in_b = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_b", {Operands::BINARY_RHS, Operands::BINARY_ANY})).first;
    MRRG::NodeDescriptor fpunit  = result.insert(MRRGNode::make_function(this, data_size, i, "fpunit", 0, supported_modes)).first;
    MRRG::NodeDescriptor out  = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;

    result.link(in_a, fpunit);
    result.link(in_b, fpunit);
    (void)out;
  }

  for(unsigned i = 0; i < II; i+= getII())
  {
    MRRG::NodeDescriptor fpunit = result.getNode(i, "fpunit");
    MRRG::NodeDescriptor out_next = result.getNode(MOD_II(i + getLatency()), "out");

    result.link(fpunit, out_next);
  }

  return result_ptr;
}

