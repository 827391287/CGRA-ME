/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/Exception.h>
#include <CGRA/ModuleComposites.h>
#include <ios>

MemPort::MemPort(std::string name, Location loc, int num_connections_, int size, int num_const_addresses_, bool pred, int II)
    : Module(name, loc, size)
    , num_connections(num_connections_)
    , num_const_addresses(num_const_addresses_)
{
    using std::to_string;

    addParameter("size", size);
    if (II > 1) 
        addPort("Context", PORT_INPUT, ceil(log2(II)));  

    if (num_connections < 2) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "MemPort doesn't support num_connections < 2. num_connections = " << num_connections;
    });}

    // add constant ports (always will have these ports)
    addPort("out", PORT_OUTPUT, size);
    node_relative_position.insert({"out", {0.5, 0.7}});     // Add position for visualization

    // make all input ports
    for(int i = 0; i < num_connections; i++)
    {
        addPort("in" + std::to_string(i), PORT_INPUT, size);
        // Add position for visualization
        node_relative_position.insert({"in" + std::to_string(i), {0, static_cast<double>(i+0.5)/num_connections/2.0}});
    };

    const auto constAddrName = [](int index) { return "const_addr_" + to_string(index); };

    // add FU for loads/stores
    addSubModule(new MemoryUnit("mem_unit", loc, size, isElastic, pred), 0.2, 0, 0.5, 0.2);

    if (pred) {
        for (int i = 0; i < num_connections; i++)
            addPort("pred" + std::to_string(i), PORT_INPUT, 1);
        addSubModule(new Multiplexer("mux_pred", loc, num_connections, 1), 0.6, 0.4, 0.5, 0.2);
        for(int i = 0; i < num_connections; i++)
            addConnection("this.pred" + std::to_string(i), "mux_pred.in" + std::to_string(i));
        addConnection("mux_pred.out", "mem_unit.pred");
        addConfig(new ConfigCell("MuxPred", II), {"mux_pred.select"});
    }

    for (int i = 0; i < num_const_addresses; ++i) {
        addSubModule(new ConstUnit(constAddrName(i), loc, size, II), static_cast<double>(i)/num_const_addresses, 0.75, 1.0/num_const_addresses, 0.25);
    }

    // add input muxs
    addSubModule(new Multiplexer("mux_addr", loc, num_connections + num_const_addresses, size), 0.6, 0.2, 0.5, 0.2);
    addSubModule(new Multiplexer("mux_data", loc, num_connections, size), 0.6, 0.4, 0.5, 0.2);
    addConfig(new ConfigCell("MuxAddr", II), {"mux_addr.select"});
    addConfig(new ConfigCell("MuxData", II), {"mux_data.select"});

    // make all mux input connections
    for(int i = 0; i < num_connections; i++)
    {
        addConnection("this.in" + std::to_string(i), "mux_data.in" + std::to_string(i));
        addConnection("this.in" + std::to_string(i), "mux_addr.in" + std::to_string(i));
    }
    for (int i = 0; i < num_const_addresses; ++i) {
        addConnection(constAddrName(i) + ".out", "mux_addr.in" + to_string(i + num_connections));
    }

    // connections to the mem unit
    addConnection("mux_addr.out", "mem_unit.addr");
    addConnection("mux_data.out", "mem_unit.data_in");
    addConnection("mem_unit.data_out", "this.out");
    addConfig(new ConfigCell("WriteRq", II), {"mem_unit.w_rq"});

    if (II > 1) {
        addConnection("this.Context", "WriteRq.Context", false);
        addConnection("this.Context", "MuxData.Context", false);
        addConnection("this.Context", "MuxAddr.Context", false);
        if (pred)
            addConnection("this.Context", "MuxPred.Context", false);
        for (int i = 0; i < num_const_addresses; ++i) {
            addConnection("this.Context", constAddrName(i) + ".Context", false);
        }
    }
}

std::string MemPort::GenericName()
{
    return string_from_stream([&](auto&& s) {
        s << "memoryPort_" << num_connections << "connect_" ;
        if (num_const_addresses != 0) {
            s << num_const_addresses << "constaddrs_";
        }
        s << this->getSize() << "b";
    });
}

MemPort::~MemPort()
{
}

IOPort::IOPort(std::string name, Location loc, int num_inputs_, int size)
    : Module(name, loc, size)
    , num_inputs(num_inputs_)
{
    if (num_inputs < 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "IOPort doesn't support num_inputs < 1. num_inputs = " << num_inputs;
    });}

    // add constant ports (always will have these ports)
    addPort("out", PORT_OUTPUT, size);
    for(int i = 0; i < num_inputs; i++)
    {
        addPort("in" + std::to_string(i), PORT_INPUT, size);
    };

    addSubModule(new IO("io_unit", loc, size), 0, 0.5, 1, 0.5);

    // add connection(s) + mux to io unit
    if(num_inputs == 1)
    {
        addConnection("this.in0", "io_unit.in");
        addConnection("io_unit.out", "this.out");
    }
    else
    {
        addSubModule(new Multiplexer("mux_in", loc, num_inputs, size), 0, 0, 1, 0.5);

        for(int i = 0; i < num_inputs; i++)
        {
            addConnection("this.in" + std::to_string(i), "mux_in.in" + std::to_string(i));
        }

        addConnection("mux_in.out", "io_unit.in");
        addConnection("io_unit.out", "this.out");

        addConfig(new ConfigCell("MuxConfig"), {"mux_in.select"});
    }
}

std::string IOPort::GenericName()
{
    return "ioPort_" + std::to_string(num_inputs) + "in";
}

IOPort::~IOPort()
{
}


static const std::map<std::string, CGRABlockType> block_type_map =
{
    {"no-bypass", STANDARD_NOBYPASS},
    {"bypass", STANDARD_BYPASS},
    {"diagonal", STANDARD_DIAGONAL},
};

std::istream& operator >>(std::istream &is, CGRABlockType &type)
{
    std::string str;
    is >> str;
    if (block_type_map.count(str) > 0)
        type = block_type_map.at(str);
    else
        is.setstate(std::ios_base::failbit);

    return is;
}

SimpleFU::SimpleFU(std::string name, Location loc, CGRABlockType blockType_)
    : Module(name, loc)
    , blockType(blockType_)
{
    if (blockType == STANDARD_NOBYPASS)
    {
        // add input muxs
        addSubModule(new Multiplexer("mux_a", loc, 5));
        addSubModule(new Multiplexer("mux_b", loc, 5));
        // add FU
        addSubModule(new FuncUnit("func", loc, {OpCode::ADD, OpCode::MUL}, 32, 1, 0));
        // add reg
        addSubModule(new Register("register", loc, 32));
        // add output mux
        addSubModule(new Multiplexer("mux_out", loc, 2));
        // config cells
        addConfig(new ConfigCell("MuxAConfig"), {"mux_a.select"});
        addConfig(new ConfigCell("MuxBConfig"), {"mux_b.select"});
        addConfig(new ConfigCell("MuxOutConfig"), {"mux_out.select"});
        addConfig(new ConfigCell("FuncConfig"), {"func.select"});
        addConfig(new ConfigCell("RegisterConfig"), {"register.enable"});
        // add all ports
        addPort("in0", PORT_INPUT, DEFAULT_SIZE);
        addPort("in1", PORT_INPUT, DEFAULT_SIZE);
        addPort("in2", PORT_INPUT, DEFAULT_SIZE);
        addPort("in3", PORT_INPUT, DEFAULT_SIZE);
        addPort("out", PORT_OUTPUT, DEFAULT_SIZE);
        // to mux_a
        addConnection("this.in0", "mux_a.in0");
        addConnection("this.in1", "mux_a.in1");
        addConnection("this.in2", "mux_a.in2");
        addConnection("this.in3", "mux_a.in3");
        addConnection("mux_out.out", "mux_a.in4");
        // to mux_b
        addConnection("this.in0", "mux_b.in0");
        addConnection("this.in1", "mux_b.in1");
        addConnection("this.in2", "mux_b.in2");
        addConnection("this.in3", "mux_b.in3");
        addConnection("mux_out.out", "mux_b.in4");
        // to funcunit
        addConnection("mux_a.out", "func.in_a");
        addConnection("mux_b.out", "func.in_b");
        // to reg
        addConnection("func.out", "register.in");
        // to output mux
        addConnection("func.out", "mux_out.in0");
        addConnection("register.out", "mux_out.in1");
        // to output
        addConnection("mux_out.out", "this.out");
    }
    else if (blockType == STANDARD_BYPASS)
    {
        // add input muxs
        addSubModule(new Multiplexer("mux_a", loc, 5));
        addSubModule(new Multiplexer("mux_b", loc, 5));
        // add FU
        addSubModule(new FuncUnit("func", loc, {OpCode::ADD, OpCode::MUL}, 32, 1, 0));
        // add bypass mux
        addSubModule(new Multiplexer("mux_bypass", loc, 3));
        // add reg
        addSubModule(new Register("register", loc, 32));
        // add output mux
        addSubModule(new Multiplexer("mux_out", loc, 2));
        // config cells
        addConfig(new ConfigCell("MuxAConfig"), {"mux_a.select"});
        addConfig(new ConfigCell("MuxBConfig"), {"mux_b.select"});
        addConfig(new ConfigCell("MuxBypassConfig"), {"mux_bypass.select"});
        addConfig(new ConfigCell("MuxOutConfig"), {"mux_out.select"});
        addConfig(new ConfigCell("FuncConfig"), {"func.select"});
        addConfig(new ConfigCell("RegisterConfig"), {"register.enable"});

        // add all ports
        addPort("in0", PORT_INPUT, DEFAULT_SIZE);
        addPort("in1", PORT_INPUT, DEFAULT_SIZE);
        addPort("in2", PORT_INPUT, DEFAULT_SIZE);
        addPort("in3", PORT_INPUT, DEFAULT_SIZE);
        addPort("out", PORT_OUTPUT, DEFAULT_SIZE);
        // to mux_a
        addConnection("this.in0", "mux_a.in0");
        addConnection("this.in1", "mux_a.in1");
        addConnection("this.in2", "mux_a.in2");
        addConnection("this.in3", "mux_a.in3");
        addConnection("register.out", "mux_a.in4");
        // to mux_b
        addConnection("this.in0", "mux_b.in0");
        addConnection("this.in1", "mux_b.in1");
        addConnection("this.in2", "mux_b.in2");
        addConnection("this.in3", "mux_b.in3");
        addConnection("register.out", "mux_b.in4");
        // to funcunit
        addConnection("mux_a.out", "func.in_a");
        addConnection("mux_b.out", "func.in_b");
        // to bypass
        addConnection("mux_a.out", "mux_bypass.in0");
        addConnection("mux_b.out", "mux_bypass.in1");
        addConnection("func.out", "mux_bypass.in2");
        // to reg
        addConnection("mux_bypass.out", "register.in");
        // to output mux
        addConnection("mux_bypass.out", "mux_out.in0");
        addConnection("register.out", "mux_out.in1");
        // to output
        addConnection("mux_out.out", "this.out");
    }
    else if (blockType == STANDARD_DIAGONAL)
    {
        // add input muxs
        addSubModule(new Multiplexer("mux_a", loc, 9));
        addSubModule(new Multiplexer("mux_b", loc, 9));
        // add FU
        addSubModule(new FuncUnit("func", loc, {OpCode::ADD, OpCode::MUL}, 32, 1, 0));
        // add bypass mux
        addSubModule(new Multiplexer("mux_bypass", loc, 3));
        // add reg
        addSubModule(new Register("register", loc, 32));
        // add output mux
        addSubModule(new Multiplexer("mux_out", loc, 2));
        // config cells
        addConfig(new ConfigCell("MuxAConfig"), {"mux_a.select"});
        addConfig(new ConfigCell("MuxBConfig"), {"mux_b.select"});
        addConfig(new ConfigCell("MuxBypassConfig"), {"mux_bypass.select"});
        addConfig(new ConfigCell("MuxOutConfig"), {"mux_out.select"});
        addConfig(new ConfigCell("FuncConfig"), {"func.select"});
        addConfig(new ConfigCell("RegisterConfig"), {"register.enable"});

        // add all ports
        addPort("in0", PORT_INPUT, DEFAULT_SIZE);
        addPort("in1", PORT_INPUT, DEFAULT_SIZE);
        addPort("in2", PORT_INPUT, DEFAULT_SIZE);
        addPort("in3", PORT_INPUT, DEFAULT_SIZE);
        addPort("in4", PORT_INPUT, DEFAULT_SIZE);
        addPort("in5", PORT_INPUT, DEFAULT_SIZE);
        addPort("in6", PORT_INPUT, DEFAULT_SIZE);
        addPort("in7", PORT_INPUT, DEFAULT_SIZE);
        addPort("out", PORT_OUTPUT, DEFAULT_SIZE);
        // to mux_a
        addConnection("this.in0", "mux_a.in0");
        addConnection("this.in1", "mux_a.in1");
        addConnection("this.in2", "mux_a.in2");
        addConnection("this.in3", "mux_a.in3");
        addConnection("this.in4", "mux_a.in4");
        addConnection("this.in5", "mux_a.in5");
        addConnection("this.in6", "mux_a.in6");
        addConnection("this.in7", "mux_a.in7");
        addConnection("register.out", "mux_a.in8");
        // to mux_b
        addConnection("this.in0", "mux_b.in0");
        addConnection("this.in1", "mux_b.in1");
        addConnection("this.in2", "mux_b.in2");
        addConnection("this.in3", "mux_b.in3");
        addConnection("this.in4", "mux_b.in4");
        addConnection("this.in5", "mux_b.in5");
        addConnection("this.in6", "mux_b.in6");
        addConnection("this.in7", "mux_b.in7");
        addConnection("register.out", "mux_b.in8");
        // to funcunit
        addConnection("mux_a.out", "func.in_a");
        addConnection("mux_b.out", "func.in_b");
        // to bypass
        addConnection("mux_a.out", "mux_bypass.in0");
        addConnection("mux_b.out", "mux_bypass.in1");
        addConnection("func.out", "mux_bypass.in2");
        // to reg
        addConnection("mux_bypass.out", "register.in");
        // to output mux
        addConnection("mux_bypass.out", "mux_out.in0");
        addConnection("register.out", "mux_out.in1");
        // to output
        addConnection("mux_out.out", "this.out");
    }
    else
    {
        std::cout << "ERROR, unintended mode\n";
    }
}

std::string SimpleFU::GenericName()
{
    return "cgrablock_bt" + std::to_string(blockType);
}

SimpleFU::~SimpleFU()
{
}

