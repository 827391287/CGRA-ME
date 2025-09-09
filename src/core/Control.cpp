#include <CGRA/Control.h>
#include <math.h>
#include <cmath>

using std::vector;
using std::ofstream;
using std::string;
using std::to_string;

void buildComp(ofstream &myfile, std::vector<std::string> ports, int mem_size) {
    string parameter[4] = {"INSTR_RDATA_WIDTH", "BOOT_ADDR", "PULP_SECURE", "A_EXTENSION"};
    string input[10] = {"clk_i", "rst_ni","fetch_enable_i", "Config_Clock_en", "Config_Reset", "CGRA_Clock_en",
            "CGRA_Reset", "riscv_enable", "configurator_enable", "configurator_reset"};
    string logic[44] = {"riscv_rst_n", "counter", "instr_req", "instr_gnt", "instr_rvalid", "instr_addr", "instr_rdata",
            "data_req", "data_gnt", "data_rvalid", "data_addr", "data_we", "data_be", "data_rdata", "data_wdata",
            "data_rdata_port", "data_rdata_ram", "data_atop", "debug_req_i", "sec_lvl_o", "irq_ack", "irq_id",
            "irq_sec", "irq_software","irq_timer", "irq_external", "irq_fast", "irq_nmi", "irq_fastx",
            "ConfigIn", "ConfigOut", "CGRA_Enable", "cgra_data_addr", "cgra_data_rdata", "cgra_data_wdata", 
            "cgra_we_i", "core_busy_o", "Config_Clock", "CGRA_Reset_Control", "cgra_io", "select_port", "end_port", 
            "use_counter", "end_cgra"};
    int mem_index;
    std::vector<std::string> io_in_s;
    std::vector<std::string> io_out_s;
    std::vector<std::string> data_in_s;
    std::vector<std::string> data_out_s;
    std::vector<std::string> w_rq_s;
    std::vector<std::string> addr_s;
    for (auto port : ports) {
        if (port.find("addr_to_ram") != string::npos) {
            addr_s.push_back(port);
        } else if (port.find("data_in_to_ram") != string::npos) {
            data_in_s.push_back(port);
        } else if (port.find("data_out_from_ram") != string::npos) {
            data_out_s.push_back(port);
        } else if (port.find("w_rq_to_ram") != string::npos) { 
            w_rq_s.push_back(port);
        } else if (port.find("bidir_in") != string::npos) { 
            io_in_s.push_back(port);
        } else if (port.find("bidir_out") != string::npos) { 
            io_out_s.push_back(port);
        } /*else {
            std::cout << "ports not for memory or I/O detected" << port << std::endl;
        }*/
    }
    if (addr_s.size() != w_rq_s.size() || addr_s.size() != data_in_s.size() || 
    addr_s.size() != data_out_s.size() || io_in_s.size() != io_out_s.size()) {
        std::cout << "Port numbers are not consistent" << std::endl;
        std::cout << addr_s.size() << " " << w_rq_s.size() << " " << data_in_s.size() << " " << data_in_s.size() << " " << io_in_s.size() << " " << io_out_s.size() << "\n";
        exit(1);
    } 
    if(!IsPowerOfTwo(addr_s.size())) {
        std::cout << "Memory port number is not power of 2" << std::endl;
        exit(1);
    }

    std::vector<std::string> head_mem;
    std::vector<std::string> head_io;
    string::size_type pos;
    for (auto port : addr_s) {
        pos = port.find("mem_unit_addr_to_ram");
        if (pos != string::npos) {
            head_mem.push_back(port.substr(0, pos-1));
        }
    }
    for (auto port : io_in_s) {
        pos = port.find("IOPin_bidir_in");
        if (pos != string::npos) {
            head_io.push_back(port.substr(0, pos-1));
        }
    }

    vector<vector<string>> io_port;
    vector<vector<string>> mem_port;
    for (int i = 0; i < io_in_s.size(); i++) {
        io_port.push_back(vector<string>());
        io_port[i].push_back(io_in_s[i]);
        io_port[i].push_back(io_out_s[i]);
    }
    for (int i = 0; i < addr_s.size(); i++) {
        mem_port.push_back(vector<string>());
        mem_port[i].push_back(addr_s[i]);
        mem_port[i].push_back(data_in_s[i]);
        mem_port[i].push_back(data_out_s[i]);
        mem_port[i].push_back(w_rq_s[i]);
    }
    mem_index = mem_size - 1;
    buildControl(myfile, io_port, mem_port, parameter, input, logic, mem_index);
    buildRam(myfile, mem_port.size(), parameter, input, logic, mem_index);
    ofstream header;
    header.open("hybrid.h");
    buildHybridHeader(header, head_mem, head_io);
    header.close();
    myfile.close();
}

void buildControl(ofstream &myfile, vector<vector<string>> io_port, vector<vector<string>> mem_port, 
string *parameter, string *input, string *logic, int mem_index) {
    myfile << "module hybrid_wrapper\n";
    myfile <<"\t#(parameter " << parameter[0] << " = 128,\n";
    myfile <<"\tparameter " << parameter[1] << " = 'h180,\n";
    myfile <<"\tparameter " << parameter[2] << " = 1,\n";
    myfile <<"\tparameter " << parameter[3] << " = 1)\n";
    myfile <<"\t(input logic " << input[0] << ",\n";
    myfile <<"\tinput logic " << input[1] << ",\n";
    myfile <<"\tinput logic " << input[2] << ",\n";
    myfile <<"\tinput logic " << input[3] << ",\n";
    myfile <<"\tinput logic " << input[4] << ",\n";
    myfile <<"\tinput logic " << input[5] << ",\n";
    myfile <<"\tinput logic " << input[6] << ",\n";
    myfile <<"\tinput logic " << input[7] << ",\n";
    myfile <<"\tinput logic " << input[8] << ",\n";
    myfile <<"\tinput logic " << input[9] << ",\n";
    myfile <<"\toutput logic configurator_done\n";
    myfile <<"\t);\n\n";
    myfile <<"\tlogic " << logic[0] << ";\n";
	myfile <<"\tlogic [31:0] " << logic[1] << ";\n";
    myfile <<"\tlogic " << logic[2] << ";\n";
    myfile <<"\tlogic " << logic[3] << ";\n";
    myfile <<"\tlogic " << logic[4] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[5] << ";\n";
    myfile <<"\tlogic [" << parameter[0] << "-1:0] " << logic[6] << ";\n";
    myfile <<"\tlogic " << logic[7] << ";\n";
    myfile <<"\tlogic " << logic[8] << ";\n";
    myfile <<"\tlogic " << logic[9] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[10] << ";\n";
    myfile <<"\tlogic " << logic[11] << ";\n";
    myfile <<"\tlogic [3:0] " << logic[12] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[13] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[14] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[15] << ";\n";
	myfile <<"\tlogic [31:0] " << logic[16] << ";\n";
	myfile <<"\tlogic [5:0] " << logic[17] << ";\n";
    myfile <<"\tlogic " << logic[18] << ";\n";
    myfile <<"\tlogic " << logic[19] << ";\n";
    myfile <<"\tlogic " << logic[20] << ";\n";
    myfile <<"\tlogic [0:4] " << logic[21] << ";\n";
    myfile <<"\tlogic " << logic[22] << ";\n";
    myfile <<"\tlogic " << logic[23] << ";\n";
    myfile <<"\tlogic " << logic[24] << ";\n";
    myfile <<"\tlogic " << logic[25] << ";\n";
    myfile <<"\tlogic [14:0] " << logic[26] << ";\n";
    myfile <<"\tlogic " << logic[27] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[28] << ";\n";
    myfile <<"\tlogic " << logic[29] << ";\n";
    myfile <<"\tlogic " << logic[30] << ";\n";
	myfile <<"\tlogic " << logic[31] << ";\n";
    myfile <<"\tlogic " << logic[36] << ";\n";
    myfile <<"\tlogic " << logic[37] << ";\n";
    myfile <<"\tlogic " << logic[38] << ";\n";
    for(int i = 0; i < io_port.size(); i++) {
        myfile <<"\tlogic [31:0] " << io_port[i][0] << ";\n";
        myfile <<"\tlogic [31:0] " << io_port[i][1] << ";\n";
    }
    for(int i = 0; i < mem_port.size(); i++) {
        myfile <<"\tlogic [31:0] " << mem_port[i][0] << ";\n";
        myfile <<"\tlogic [31:0] " << mem_port[i][1] << ";\n";
        myfile <<"\tlogic [31:0] " << mem_port[i][2] << ";\n";
        myfile <<"\tlogic " << mem_port[i][3] << ";\n";
    }
    int bit = 32 * mem_port.size() - 1;
    myfile <<"\tlogic [31:0] " << logic[32] << "[" << mem_port.size() - 1 << ":0];\n";
    myfile <<"\tlogic [" << bit << ":0] " << logic[33] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[34] << "[" << mem_port.size() - 1 << ":0];\n";
    myfile <<"\tlogic [" << mem_port.size() - 1 << ":0] " << logic[35] << ";\n";
    myfile <<"\tlogic [31:0] " << logic[39] << "[" << io_port.size() - 1 << ":0];\n";
	myfile <<"\tlogic [" << (int)ceil(log2(io_port.size())) << ":0] " << logic[40] << ";\n";
	myfile <<"\tlogic [31:0] " << logic[41] << ";\n";
	myfile <<"\tlogic " << logic[42] << ";\n";
	myfile <<"\tlogic " << logic[43] << ";\n";
    myfile <<"\tassign " << logic[37] << " = " << input[0] << " & " << input[3] << ";\n";
    myfile <<"\tassign " << logic[0] << " = " <<input[1] << " & " << input[7] << ";\n";
    myfile << "\tassign " << logic[35] << " = {\n";
    for (int i = mem_port.size() - 1; i > 0; i--) {
        myfile <<"\t\t" << mem_port[i][3] << ",\n";
    }
    myfile <<"\t\t" << mem_port[0][3] << "};\n\n";

    myfile <<"\tassign " << logic[41] << " = " << logic[39] << "[" << logic[40] << "];\n";
    myfile << "\tassign " << logic[39] << " = {\n";
    for (int i = io_port.size() - 1; i > 0; i--) {
        myfile <<"\t\t" << io_port[i][1] << ",\n";
    }
    myfile <<"\t\t" << io_port[0][1] << "};\n\n";

    myfile << "\talways_comb begin\n\t\t" << logic[32] << " = {\n";
    for (int i = mem_port.size() - 1; i > 0; i--) {
        myfile <<"\t\t" << mem_port[i][0] << ",\n";
    }
    myfile <<"\t\t" << mem_port[0][0] << "};\n";
    myfile << "\t\t{\n";
    for (int i = mem_port.size() - 1; i > 0; i--) {
        myfile <<"\t\t" << mem_port[i][2] << ",\n";
    }
    myfile <<"\t\t" << mem_port[0][2] << "} = " << logic[33] << ";\n";
    myfile << "\t\t" << logic[34] << " = {\n";
    for (int i = mem_port.size() - 1; i > 0; i--) {
        myfile <<"\t\t" << mem_port[i][1] << ",\n";
    }
    myfile <<"\t\t" << mem_port[0][1] << "};\n";
    myfile << "\tend\n\tassign " << logic[18] << " = 1'b0;\n\tassign " << logic[22] << " = 1'b0;\n\n";

    myfile << "\triscv_core\n";
    myfile << "\t\t#(." << parameter[0] << " (" << parameter[0] << "),\n";
    myfile << "\t\t." << parameter[2] << "(" << parameter[2] << "),\n";
    myfile << "\t\t." << parameter[3] << "(" << parameter[3] << "),\n";
    myfile << "\t\t.FPU(0))\n";
    myfile << "\triscv_core_i (\n";
    myfile << "\t\t." << input[0] << "(" << input[0] << "),\n";
    myfile << "\t\t." << input[1] << "(" << logic[0] << "),\n";
    myfile << "\t\t.clock_en_i('1),\n";
    myfile << "\t\t.test_en_i('0),\n";
    myfile << "\t\t.boot_addr_i(" << parameter[1] << "),\n";
    myfile << "\t\t.core_id_i(4'h0),\n";
    myfile << "\t\t.cluster_id_i(6'h0),\n";
    myfile << "\t\t." << logic[5] << "_o(" << logic[5] << "),\n";
    myfile << "\t\t." << logic[2] << "_o(" << logic[2] << "),\n";
    myfile << "\t\t." << logic[6] << "_i(" << logic[6] << "),\n";
    myfile << "\t\t." << logic[3] << "_i(" << logic[3] << "),\n";
    myfile << "\t\t." << logic[4] << "_i(" << logic[4] << "),\n";
    myfile << "\t\t." << logic[10] << "_o(" << logic[10] << "),\n";
    myfile << "\t\t." << logic[14] << "_o( " << logic[14] << "),\n";
    myfile << "\t\t." << logic[11] << "_o( " << logic[11] << "),\n";
    myfile << "\t\t." << logic[7] << "_o(" << logic[7] << "),\n";
    myfile << "\t\t." << logic[12] << "_o(" << logic[12] << "),\n";
    myfile << "\t\t." << logic[13] << "_i(" << logic[13] << "),\n";
    myfile << "\t\t." << logic[8] << "_i( " << logic[8] << "),\n";
    myfile << "\t\t." << logic[9] << "_i( " << logic[9] << "),\n";
    myfile << "\t\t." << logic[17] << "_o( " << logic[17] << "),\n";
    myfile << "\t\t.apu_master_req_o(),\n";
    myfile << "\t\t.apu_master_ready_o(),\n";
    myfile << "\t\t.apu_master_gnt_i(),\n";
    myfile << "\t\t.apu_master_operands_o(),\n";
    myfile << "\t\t.apu_master_op_o(),\n";
    myfile << "\t\t.apu_master_type_o(),\n";
    myfile << "\t\t.apu_master_flags_o(),\n";
    myfile << "\t\t.apu_master_valid_i(),\n";
    myfile << "\t\t.apu_master_result_i (),\n";
    myfile << "\t\t.apu_master_flags_i(),\n";
    myfile << "\t\t." << logic[23] << "_i(" << logic[23] << "),\n";
    myfile << "\t\t." << logic[24] << "_i(" << logic[24] << "),\n";
    myfile << "\t\t." << logic[25] << "_i(" << logic[25] << "),\n";
    myfile << "\t\t." << logic[26] << "_i(" << logic[26] << "),\n";
    myfile << "\t\t." << logic[27] << "_i(" << logic[27] << "),\n";
    myfile << "\t\t." << logic[28] << "_i(" << logic[28] << "),\n";
    myfile << "\t\t." << logic[20] << "_o(" << logic[20] << "),\n";
    myfile << "\t\t." << logic[21] << "_o(" << logic[21] << "),\n";
    myfile << "\t\t." << logic[22] << "_i(" << logic[22] << "),\n";
    myfile << "\t\t." << logic[19] << "(" << logic[19] << "),\n";
    myfile << "\t\t." << logic[18] << "(" << logic[18] << "),\n";
    myfile << "\t\t." <<input[2] << "(" << input[2] << "),\n";
    myfile << "\t\t." << logic[36] << "(" << logic[36] << "),\n";
    myfile << "\t\t.ext_perf_counters_i(),\n";
    myfile << "\t\t.fregfile_disable_i(1'b0));\n\n";

    myfile << "\talways @(posedge " << input[0] << " or negedge " << input[1] << ") begin\n";
    myfile << "\t\tif (~" << input[1] << ") begin\n";
        for (int i = 0; i < io_port.size(); i++) {
            myfile <<"\t\t\t" << io_port[i][0] << " <= '0;\n";
        }
    myfile << "\t\t\t" << logic[1] << " <= '0;\n";
    myfile << "\t\t\t" << logic[31] << " <= 1'b0;\n";
    myfile << "\t\t\t" << logic[15] << " <= '0;\n";
    myfile << "\t\t\t" << logic[38] << " <= '0;\n";
    myfile << "\t\t\t" << logic[40] << " <= '0;\n";
    myfile << "\t\t\t" << logic[42] << " <= '1;\n";
    myfile << "\t\t\t" << logic[43] << " <= '0;\n";
	myfile << "\t\tend else begin\n";
	myfile << "\t\t\tif (data_req) begin\n";
	myfile << "\t\t\t\tif (data_addr[" << mem_index+1 << "]) begin\n";
	myfile << "\t\t\t\t\tif (data_we) begin\n";
    for (int i = 0; i < io_port.size(); i++) {
        myfile << "\t\t\t\t\t\t";
        if(i == 0) {
            myfile << "if ";
        } else {
            myfile << "end else if ";
        }
        myfile << "(data_addr[9:2] == " << i << ") begin\n";
        myfile << "\t\t\t\t\t\t\t" << io_port[i][0] << " <= data_wdata;\n";
    }
	myfile << "\t\t\t\t\t\tend else if(data_addr[9:2] == " << io_port.size() << ") begin\n";
    myfile << "\t\t\t\t\t\t\tcounter <= data_wdata;\n";
    myfile << "\t\t\t\t\t\t\tCGRA_Enable <= 1'b1;\n";
    myfile << "\t\t\t\t\t\t\tuse_counter <= 1'b1;\n";
    myfile << "\t\t\t\t\t\tend else if(data_addr[9:2] == " << io_port.size() + 1 << ") begin\n";
    myfile << "\t\t\t\t\t\t\tCGRA_Enable <= 1'b1;\n";
    myfile << "\t\t\t\t\t\t\tuse_counter <= 1'b0;\n";
    myfile << "\t\t\t\t\t\tend else if(data_addr[9:2] == " << io_port.size() + 2 << ") begin\n";
    myfile << "\t\t\t\t\t\t\tCGRA_Reset_Control <= data_wdata;\n";
    myfile << "\t\t\t\t\t\tend else if(data_addr[9:2] == " << io_port.size() + 3 << ") begin\n";
    myfile << "\t\t\t\t\t\t\tselect_port <= data_wdata;\n";
    myfile << "\t\t\t\t\t\tend\n";
	myfile << "\t\t\t\t\tend else begin\n";
    for (int i = 0; i < io_port.size(); i++) {
        myfile << "\t\t\t\t\t\t";
        if(i == 0) {
            myfile << "if ";
        } else {
            myfile << "end else if ";
        }
        myfile << "(data_addr[9:2] == " << i << ") begin\n";
        myfile << "\t\t\t\t\t\t\tdata_rdata_port <= " << io_port[i][1] << ";\n";
    }
    myfile << "\t\t\t\t\t\tend else if (data_addr[9:2] == " << io_port.size() << ") begin\n";
	myfile << "\t\t\t\t\t\t\tif (counter <= 3) begin\n";
	myfile << "\t\t\t\t\t\t\t\tdata_rdata_port <= 1;\n";
	myfile << "\t\t\t\t\t\t\tend else begin\n";
	myfile << "\t\t\t\t\t\t\t\tdata_rdata_port <= 0;\n";
	myfile << "\t\t\t\t\t\t\tend\n";
    myfile << "\t\t\t\t\t\tend else if (data_addr[9:2] == " << io_port.size() + 1 << ") begin\n";
	myfile << "\t\t\t\t\t\t\tif (end_cgra == 1) begin\n";
	myfile << "\t\t\t\t\t\t\t\tdata_rdata_port <= 1;\n";
	myfile << "\t\t\t\t\t\t\tend else begin\n";
	myfile << "\t\t\t\t\t\t\t\tdata_rdata_port <= 0;\n";
	myfile << "\t\t\t\t\t\t\tend\n";
	myfile << "\t\t\t\t\t\tend\n";
	myfile << "\t\t\t\t\tend\n";
	myfile << "\t\t\t\tend\n";
	myfile << "\t\t\tend\n";
    myfile << "\t\t\tif (use_counter) begin\n";
	myfile << "\t\t\t\tif (CGRA_Enable) begin\n";
	myfile << "\t\t\t\t\tif (counter == 0) begin\n";
	myfile << "\t\t\t\t\t\tCGRA_Enable <= 1'b0;\n";
	myfile << "\t\t\t\t\tend else begin\n";
	myfile << "\t\t\t\t\t\tcounter <= counter - 1;\n";
	myfile << "\t\t\t\t\tend\n";
	myfile << "\t\t\t\tend\n";		
	myfile << "\t\t\tend else begin\n";
    myfile << "\t\t\t\tif (CGRA_Enable) begin\n";
    myfile << "\t\t\t\t\tif (end_port == 1) begin\n";
    myfile << "\t\t\t\t\t\tCGRA_Enable <= 1'b0;\n";
    myfile << "\t\t\t\t\t\tend_cgra <= 1'b1;\n";
    myfile << "\t\t\t\t\tend\n";
	myfile << "\t\t\t\tend\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\tend\n";	
	myfile << "\tend\n\n";
	myfile << "\talways_comb begin\n";
	myfile << "\t\tif(" << logic[10] << " < 2 ** " << to_string(mem_index+1) << ") begin\n";
	myfile << "\t\t\t" << logic[13] << " = " << logic[16] << ";\n";
	myfile << "\t\tend else begin\n";
	myfile << "\t\t\t" << logic[13] << " = " << logic[15] << ";\n\t\tend\n\tend\n\n";

	myfile << "\tcgra_U0 cgra_i(\n";
    myfile << "\t\t." << logic[37] << "(" << logic[37] << "),\n";
    myfile << "\t\t." << input[4] << "(" << input[4] << "),\n";
    myfile << "\t\t." << logic[29] << "(" << logic[29] << "),\n";
    myfile << "\t\t." << logic[30] << "(" << logic[30] << "),\n";
    myfile << "\t\t.CGRA_Clock(" << input[0] << " & " << input[5] << "),\n";
    myfile << "\t\t." << logic[31] << "(" << logic[31] << "),\n";
    myfile << "\t\t." << input[6] << "(" << input[6] << " | " << logic[38] << "),\n";
    for (int i = 0; i < io_port.size(); i++) {
        myfile << "\t\t." << io_port[i][0] << "(" << io_port[i][0] << "),\n";
        myfile << "\t\t." << io_port[i][1] << "(" << io_port[i][1] << "),\n";
    }
    for (int i = 0; i < mem_port.size(); i++) {
        if (i == mem_port.size() - 1) {
            myfile << "\t\t." << mem_port[i][0] << "(" << mem_port[i][0] << "),\n";
            myfile << "\t\t." << mem_port[i][1] << "(" << mem_port[i][1] << "),\n";
            myfile << "\t\t." << mem_port[i][2] << "(" << mem_port[i][2] << "),\n";
            myfile << "\t\t." << mem_port[i][3] << "(" << mem_port[i][3] << ")\n";
        } else {
            myfile << "\t\t." << mem_port[i][0] << "(" << mem_port[i][0] << "),\n";
            myfile << "\t\t." << mem_port[i][1] << "(" << mem_port[i][1] << "),\n";
            myfile << "\t\t." << mem_port[i][2] << "(" << mem_port[i][2] << "),\n";
            myfile << "\t\t." << mem_port[i][3] << "(" << mem_port[i][3] << "),\n";
        }
    }
    myfile << "\t\t);\n\n";
   
	myfile << "\tCGRA_configurator configurator(\n";
    myfile << "\t\t.clock(" << logic[37] << "),\n";
    myfile << "\t\t.enable(" << input[8] << "),\n";
    myfile << "\t\t.sync_reset(" << input[9] << "),\n";
    myfile << "\t\t.bitstream(" << logic[29] << "),\n";
    myfile << "\t\t.done(configurator_done));\n\n";
    myfile << "\tmm_ram\n";
    myfile << "\t\t#(.RAM_ADDR_WIDTH (" << mem_index+1 << "),\n";
    myfile << "\t\t." << parameter[0] << " (" << parameter[0] << "))\n";
    myfile << "\t\tram_i(\n";
    myfile << "\t\t." << input[0] << "(" << input[0] << "),\n";
    myfile << "\t\t." << input[1] << "(" << logic[0] << "),\n";
    myfile << "\t\t." << input[6] << "(" << input[6] << " | " << logic[38] << "),\n";
    myfile << "\t\t." << logic[2] << "_i(" << logic[2] << "),\n";
    myfile << "\t\t." << logic[5] << "_i   (" << logic[5] << "),\n";
    myfile << "\t\t." << logic[6] << "_o(" << logic[6] << "),\n";
    myfile << "\t\t." << logic[4] << "_o(" << logic[4] << "),\n";
    myfile << "\t\t." << logic[3] << "_o(" << logic[3] << "),\n";
    myfile << "\t\t.riscv_req_i(" << logic[7] << "),\n";
    myfile << "\t\t.riscv_addr_i(" << logic[10] << "),\n";
    myfile << "\t\t.riscv_we_i(" << logic[11] << "),\n";
    myfile << "\t\t.riscv_be_i(" << logic[12] << " ),\n";
    myfile << "\t\t.riscv_wdata_i(" << logic[14] << "),\n";
    myfile << "\t\t.riscv_rdata_o( " << logic[13] << "_ram),\n";
    myfile << "\t\t.riscv_rvalid_o  ( " << logic[9] << "),\n";
    myfile << "\t\t.riscv_gnt_o( " << logic[8] << "),\n";
    myfile << "\t\t.cgra_enable( " << logic[31] << "),\n";
    myfile << "\t\t." << logic[32] << " ( " << logic[32] << "),\n";
    myfile << "\t\t." << logic[34] << "( " << logic[34] << "),\n";
    myfile << "\t\t." << logic[33] << "( " << logic[33] << "),\n";
    myfile << "\t\t." << logic[35] << "(" << logic[35] << "),\n";
    myfile << "\t\t." << logic[21] << "_i( " << logic[21] << "),\n";
    myfile << "\t\t." << logic[20] << "_i( " << logic[20] << "  ),\n";
    myfile << "\t\t." << logic[23] << "_o ( " << logic[23] << "),\n";
    myfile << "\t\t." << logic[24] << "_o ( " << logic[24] << "),\n";
    myfile << "\t\t." << logic[25] << "_o ( " << logic[25] << "),\n";
    myfile << "\t\t." << logic[26] << "_o( " << logic[26] << " ),\n";
    myfile << "\t\t." << logic[27] << "_o( " << logic[27] << "  ),\n";
    myfile << "\t\t." << logic[28] << "_o( " << logic[28] << "));\n";
    myfile << "endmodule\n";
}

void buildRam(ofstream &myfile, int mem_num, string *parameter, string *input, string *logic, int mem_size) {
    myfile << "module mm_ram\n";
    myfile << "\t#(parameter RAM_ADDR_WIDTH = 16,\n";
    myfile << "\tparameter INSTR_RDATA_WIDTH = 128)\n";
    myfile << "\t(input logic clk_i,\n";
    myfile << "\tinput logic rst_ni,\n";
    myfile << "\tinput logic	CGRA_Reset,\n";
    myfile << "\tinput logic instr_req_i,\n";
    myfile << "\tinput logic [31:0] instr_addr_i,\n";
    myfile << "\toutput logic [INSTR_RDATA_WIDTH-1:0] instr_rdata_o,\n";
    myfile << "\toutput logic instr_rvalid_o,\n";
    myfile << "\toutput logic instr_gnt_o,\n";
    myfile << "\tinput logic riscv_req_i,\n";
    myfile << "\tinput logic [31:0] riscv_addr_i,\n";
    myfile << "\tinput logic riscv_we_i,\n";
    myfile << "\tinput logic [3:0]  riscv_be_i,\n";
    myfile << "\tinput logic [31:0] riscv_wdata_i,\n";
    myfile << "\toutput logic [31:0] riscv_rdata_o,\n";
    myfile << "\toutput logic riscv_rvalid_o,\n";
    myfile << "\toutput logic riscv_gnt_o,\n";
    myfile << "\tinput logic cgra_enable,\n"; 
    myfile << "\tinput logic [31:0] cgra_data_addr[" << mem_num - 1 << ":0],\n";
    myfile << "\tinput logic [31:0] cgra_data_wdata[" << mem_num - 1 << ":0],\n";
    myfile << "\toutput logic [" << 32 * mem_num - 1 << ":0] cgra_data_rdata,\n";
    myfile << "\tinput logic [" << mem_num - 1 << ":0] cgra_we_i,\n";
    myfile << "\tinput logic [4:0] irq_id_i,\n";
    myfile << "\tinput logic irq_ack_i,\n";
    myfile << "\toutput logic irq_software_o,\n";
    myfile << "\toutput logic irq_timer_o,\n";
    myfile << "\toutput logic irq_external_o,\n";
    myfile << "\toutput logic [14:0] irq_fast_o,\n";
    myfile << "\toutput logic irq_nmi_o,\n";
    myfile << "\toutput logic [31:0] irq_fastx_o);\n";
    myfile << "\tlocalparam int TIMER_IRQ_ID = 7;\n";
    myfile << "\tlocalparam int INSTRUC_ADDR_WIDTH = 22;\n";
    myfile << "\tlocalparam int BANK_ADDR_WIDTH = RAM_ADDR_WIDTH - " << (int)ceil(log2(mem_num)) << ";\n";
    myfile << "\tenum logic [1:0]{RAM, MM, RND_STALL, ERR} select_rdata_d, select_rdata_q;\n";
    myfile << "\tenum logic {T_RAM, T_PER} transaction;\n";
    myfile << "\tenum logic [1:0] {IDLE, PERIPHEARL_VALID, WAIT_RAM_GNT, WAIT_RAM_VALID} state_valid_n, state_valid_q;\n";
    myfile << "\tlogic [31:0] data_addr_aligned;\n";
    myfile << "\tlogic data_rvalid_q;\n";
    myfile << "\tlogic instr_rvalid_q;\n";
    myfile << "\tlogic [INSTR_RDATA_WIDTH-1:0] core_instr_rdata;\n";
    myfile << "\tlogic [31:0] core_data_rdata;\n";
    myfile << "\tlogic [" << mem_num - 1 << ":0] ram_bank_enable;\n";
    myfile << "\tlogic [RAM_ADDR_WIDTH-1:0] ram_data_addr;\n";
    myfile << "\tlogic [31:0] ram_data_wdata;\n";
    myfile << "\tlogic [31:0] ram_data_rdata;\n";
    myfile << "\tlogic ram_data_we;\n";
    myfile << "\tlogic [3:0]  ram_data_be;\n";
    myfile << "\tlogic ram_data_gnt;\n";
    myfile << "\tlogic ram_data_valid;\n";
    myfile << "\tlogic data_req_dec;\n";
    myfile << "\tlogic [31:0] data_wdata_dec;\n";
    myfile << "\tlogic [RAM_ADDR_WIDTH-1:0] data_addr_dec;\n";
    myfile << "\tlogic data_we_dec;\n";
    myfile << "\tlogic [3:0]  data_be_dec;\n";
    myfile << "\tlogic [RAM_ADDR_WIDTH-1:0] data_addr_dec_q;\n";
    myfile << "\tlogic [INSTR_RDATA_WIDTH-1:0] ram_instr_rdata;\n";
    myfile << "\tlogic ram_instr_req;\n";
    myfile << "\tlogic [INSTRUC_ADDR_WIDTH-1:0] ram_instr_addr;\n";
    myfile << "\tlogic ram_instr_gnt;\n";
    myfile << "\tlogic ram_instr_valid;\n";
    myfile << "\tlogic [31:0] timer_irq_mask_q;\n";
    myfile << "\tlogic [31:0] timer_cnt_q;\n";
    myfile << "\tlogic irq_timer_q;\n";
    myfile << "\tlogic timer_reg_valid;\n";
    myfile << "\tlogic timer_val_valid;\n";
    myfile << "\tlogic [31:0] timer_wdata;\n";
    myfile << "\tlogic [31:0] ram_data_rdata_tmp[0:3];\n";
    myfile << "\tlogic [" << mem_num - 1 << ":0] cgra_bank_enable;\n";
    myfile << "\ttypedef struct packed {\n";
    myfile << "\t\tlogic irq_software;\n";
    myfile << "\t\tlogic irq_timer;\n";
    myfile << "\t\tlogic irq_external;\n";
    myfile << "\t\tlogic [14:0] irq_fast;\n";
    myfile << "\t\tlogic irq_nmi;\n";
    myfile << "\t\tlogic [31:0] irq_fastx;\n";
    myfile << "\t} Interrupts_tb_t;\n";
    myfile << "\tInterrupts_tb_t irq_rnd_lines;\n";
    myfile << "\talways_comb data_addr_aligned = {riscv_addr_i[31:2], 2'b0} ;\n";
    myfile << "\talways_comb begin\n";
    myfile << "\t\tdata_req_dec = '0;\n";
    myfile << "\t\tdata_addr_dec = '0;\n";
    myfile << "\t\tdata_wdata_dec = '0;\n";
    myfile << "\t\tdata_we_dec = '0;\n";
    myfile << "\t\tdata_be_dec = '0;\n";
    myfile << "\t\ttimer_wdata = '0;\n";
    myfile << "\t\ttimer_reg_valid = '0;\n";
    myfile << "\t\ttimer_val_valid = '0;\n";
    myfile << "\t\tselect_rdata_d = RAM;\n";
    myfile << "\t\ttransaction  = T_PER;\n";
    myfile << "\t\tif (riscv_req_i)  begin\n";
    myfile << "\t\t\tif (riscv_we_i)  begin\n";
    myfile << "\t\t\t\tif (riscv_addr_i < 2 ** RAM_ADDR_WIDTH)  begin\n";
    myfile << "\t\t\t\t\tdata_req_dec = riscv_req_i;\n";
    myfile << "\t\t\t\t\tdata_addr_dec = riscv_addr_i[RAM_ADDR_WIDTH-1:0];\n";
    myfile << "\t\t\t\t\tdata_wdata_dec = riscv_wdata_i;\n";
    myfile << "\t\t\t\t\tdata_we_dec = riscv_we_i;\n";
    myfile << "\t\t\t\t\tdata_be_dec = riscv_be_i;\n";
    myfile << "\t\t\t\t\ttransaction = T_RAM;\n";
    myfile << "\t\t\t\tend else  begin\n";
	myfile << "\t\t\t\t\tdata_req_dec = 0;\n";
	myfile << "\t\t\t\t\tif (riscv_addr_i == 32'h1500_0000)  begin\n";
    myfile << "\t\t\t\t\t\ttimer_wdata = riscv_wdata_i;\n";
    myfile << "\t\t\t\t\t\ttimer_reg_valid = '1;\n";
    myfile << "\t\t\t\t\tend else if (riscv_addr_i == 32'h1500_0004)  begin\n";
    myfile << "\t\t\t\t\t\ttimer_wdata = riscv_wdata_i;\n";
    myfile << "\t\t\t\t\t\ttimer_val_valid = '1;\n";
    myfile << "\t\t\t\t\tend\n";
	myfile << "\t\t\t\tend\n";
    myfile << "\t\t\tend else  begin\n";
    myfile << "\t\t\t\tif (riscv_addr_i < 2 ** RAM_ADDR_WIDTH)  begin\n";
    myfile << "\t\t\t\t\tselect_rdata_d = RAM;\n";
    myfile << "\t\t\t\t\tdata_req_dec = riscv_req_i;\n";
    myfile << "\t\t\t\t\tdata_addr_dec = riscv_addr_i[RAM_ADDR_WIDTH-1:0];\n";
    myfile << "\t\t\t\t\tdata_wdata_dec = riscv_wdata_i;\n";
    myfile << "\t\t\t\t\tdata_we_dec = riscv_we_i;\n";
    myfile << "\t\t\t\t\tdata_be_dec = riscv_be_i;\n";
    myfile << "\t\t\t\t\ttransaction = T_RAM;\n";
    myfile << "\t\t\t\tend else\n";
    myfile << "\t\t\t\t\tselect_rdata_d = ERR;\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\tend\n";
    myfile << "\tend\n";
    myfile << "\talways_comb begin: read_mux\n";
    myfile << "\t\triscv_rdata_o = '0;\n";
    myfile << "\t\tif(select_rdata_q == RAM) begin\n";
    myfile << "\t\t\triscv_rdata_o = core_data_rdata;\n";
    myfile << "\t\tend\n";
    myfile << "\tend\n";
    myfile << "\talways_ff @(posedge clk_i, negedge rst_ni) begin: tb_timer\n";
    myfile << "\t\tif(~rst_ni) begin\n";
    myfile << "\t\t\ttimer_irq_mask_q <= '0;\n";
    myfile << "\t\t\ttimer_cnt_q <= '0;\n";
    myfile << "\t\t\tirq_timer_q <= '0;\n";            
    myfile << "\t\tend else begin\n";
    myfile << "\t\t\tif(timer_reg_valid) begin\n";
    myfile << "\t\t\t\ttimer_irq_mask_q <= timer_wdata;\n";
    myfile << "\t\t\tend else if(timer_val_valid) begin\n";
    myfile << "\t\t\t\ttimer_cnt_q <= timer_wdata;\n";
    myfile << "\t\t\tend else begin\n";
    myfile << "\t\t\t\tif(timer_cnt_q > 0)\n";
    myfile << "\t\t\t\t\ttimer_cnt_q <= timer_cnt_q - 1;\n";
    myfile << "\t\t\t\tif(timer_cnt_q == 1)\n";
    myfile << "\t\t\t\t\tirq_timer_q <= 1'b1 && timer_irq_mask_q[TIMER_IRQ_ID];\n";
    myfile << "\t\t\t\tif(irq_ack_i == 1'b1 && irq_id_i == TIMER_IRQ_ID)\n";
    myfile << "\t\t\t\t\tirq_timer_q <= '0;\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\tend\n";
    myfile << "\tend\n";
    for (int i = 0; i < mem_num; i++) {
        myfile << "\tdp_ram\n";
        myfile << "\t\t#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))\n";
        myfile << "\tdp_ram_" << i << "\n";
        myfile << "\t\t(\n";
        myfile << "\t\t.clk_i ( clk_i ),\n";
	 	myfile << "\t\t.rst_ni ( rst_ni ),\n";
		myfile << "\t\t.rst_CGRA  ( CGRA_Reset ),\n";
        myfile << "\t\t.en_a_i ( cgra_bank_enable[" << i << "] ),\n";
        myfile << "\t\t.addr_a_i  ( cgra_data_addr[" << i << "][RAM_ADDR_WIDTH-1:0] ),\n";
        myfile << "\t\t.wdata_a_i ( cgra_data_wdata[" << i << "] ),\n";       
        myfile << "\t\t.rdata_a_o ( cgra_data_rdata[" << i*32+31 << ":" << i*32 << "] ),\n";
        myfile << "\t\t.we_a_i ( cgra_we_i[" << i << "] ),\n";
        myfile << "\t\t.be_a_i ( 4'b1111 ),\n";       
        myfile << "\t\t.en_b_i ( ram_bank_enable[" << i << "] ),\n";
        myfile << "\t\t.addr_b_i  ( ram_data_addr ),\n";
        myfile << "\t\t.wdata_b_i ( ram_data_wdata ),\n";
        myfile << "\t\t.rdata_b_o ( ram_data_rdata_tmp[" << i << "] ),\n";
        myfile << "\t\t.we_b_i ( ram_data_we ),\n";
        myfile << "\t\t.be_b_i ( ram_data_be ));\n";
    }

    myfile << "\tinstruc_ram\n";
    myfile << "\t\t#(.ADDR_WIDTH (INSTRUC_ADDR_WIDTH),\n";
    myfile << "\t\t.INSTR_RDATA_WIDTH(INSTR_RDATA_WIDTH))\n";
    myfile << "\tinstruc_ram_i\n";
    myfile << "\t\t(.clk_i( clk_i ),\n";
    myfile << "\t\t.en_a_i( ram_instr_req ),\n";
    myfile << "\t\t.addr_a_i( ram_instr_addr ),\n";
    myfile << "\t\t.rdata_a_o( ram_instr_rdata )\n";
    myfile << "\t\t);\n";

    myfile << "\talways_ff @(posedge clk_i, negedge rst_ni) begin\n";
    myfile << "\t\tif (~rst_ni) begin\n";
    myfile << "\t\t\tselect_rdata_q <= RAM;\n";
    myfile << "\t\t\tdata_rvalid_q  <= '0;\n";
    myfile << "\t\t\tinstr_rvalid_q <= '0;\n";
    myfile << "\t\t\tstate_valid_q  <= IDLE;\n";
	myfile << "\t\t\tdata_addr_dec_q <= '0;\n";
    myfile << "\t\tend else begin\n";
    myfile << "\t\t\tselect_rdata_q <= select_rdata_d;\n";
    myfile << "\t\t\tdata_rvalid_q  <= data_req_dec;\n";
    myfile << "\t\t\tinstr_rvalid_q <= ram_instr_req;\n";
    myfile << "\t\t\tstate_valid_q  <= state_valid_n;\n";
	myfile << "\t\t\tdata_addr_dec_q <= data_addr_dec;\n";
    myfile << "\t\tend\n";
    myfile << "\tend\n";
    myfile << "\talways_comb begin\n";
    myfile << "\t\triscv_gnt_o    = 1'b0;\n";
    myfile << "\t\triscv_rvalid_o = 1'b0;\n";
    myfile << "\t\tstate_valid_n = state_valid_q;\n";
    myfile << "\t\tunique case(state_valid_q)\n";
    myfile << "\t\t\tIDLE:\n";
    myfile << "\t\t\tbegin\n";
    myfile << "\t\t\t\tif(riscv_req_i) begin\n";
    myfile << "\t\t\t\t\tif(transaction == T_RAM) begin\n";
    myfile << "\t\t\t\t\t\triscv_gnt_o = ram_data_gnt;\n";
    myfile << "\t\t\t\t\t\tif(ram_data_gnt) begin\n";
    myfile << "\t\t\t\t\t\t\tstate_valid_n = WAIT_RAM_VALID;\n";
    myfile << "\t\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\t\tstate_valid_n = WAIT_RAM_GNT;\n";
    myfile << "\t\t\t\t\t\tend\n";
    myfile << "\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\tstate_valid_n = PERIPHEARL_VALID;\n";
    myfile << "\t\t\t\t\t\triscv_gnt_o    = 1'b1;\n";
    myfile << "\t\t\t\t\tend\n";
    myfile << "\t\t\t\tend\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\t\tPERIPHEARL_VALID:\n";
    myfile << "\t\t\tbegin\n";
    myfile << "\t\t\t\triscv_rvalid_o  = 1'b1;\n";
    myfile << "\t\t\t\tif(riscv_req_i) begin\n";
    myfile << "\t\t\t\t\tif(transaction == T_RAM) begin\n";
    myfile << "\t\t\t\t\t\triscv_gnt_o = ram_data_gnt;\n";
    myfile << "\t\t\t\t\t\tif(ram_data_gnt) begin\n";
    myfile << "\t\t\t\t\t\t\tstate_valid_n = WAIT_RAM_VALID;\n";
    myfile << "\t\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\t\tstate_valid_n = WAIT_RAM_GNT;\n";
    myfile << "\t\t\t\t\t\tend\n";
    myfile << "\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\tstate_valid_n = PERIPHEARL_VALID;\n";
    myfile << "\t\t\t\t\t\triscv_gnt_o    = 1'b1;\n";
    myfile << "\t\t\t\t\tend\n";
    myfile << "\t\t\t\tend else state_valid_n = IDLE;\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\t\tWAIT_RAM_GNT:\n";
    myfile << "\t\t\tbegin\n";
    myfile << "\t\t\t\triscv_rvalid_o  = 1'b0;\n";
    myfile << "\t\t\t\tif(riscv_req_i) begin\n";
    myfile << "\t\t\t\t\triscv_gnt_o = ram_data_gnt;\n";
    myfile << "\t\t\t\t\tif(ram_data_gnt) begin\n";
    myfile << "\t\t\t\t\t\tstate_valid_n = WAIT_RAM_VALID;\n";
    myfile << "\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\tstate_valid_n = WAIT_RAM_GNT;\n";
    myfile << "\t\t\t\t\tend\n";
    myfile << "\t\t\t\tend else state_valid_n = IDLE;\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\t\tWAIT_RAM_VALID:\n";
    myfile << "\t\t\tbegin\n";
    myfile << "\t\t\t\triscv_rvalid_o  = ram_data_valid;\n";
    myfile << "\t\t\t\tif(ram_data_valid) begin\n";
    myfile << "\t\t\t\t\tif(riscv_req_i) begin\n";
    myfile << "\t\t\t\t\t\tif(transaction == RAM) begin\n";
    myfile << "\t\t\t\t\t\t\triscv_gnt_o    = ram_data_gnt;\n";
    myfile << "\t\t\t\t\t\t\tif(ram_data_gnt) begin\n";
    myfile << "\t\t\t\t\t\t\t\tstate_valid_n = WAIT_RAM_VALID;\n";
    myfile << "\t\t\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\t\t\tstate_valid_n = WAIT_RAM_GNT;\n";
    myfile << "\t\t\t\t\t\t\tend\n";
    myfile << "\t\t\t\t\t\tend else begin\n";
    myfile << "\t\t\t\t\t\t\tstate_valid_n = PERIPHEARL_VALID;\n";
    myfile << "\t\t\t\t\t\t\triscv_gnt_o    = 1'b1;\n";
    myfile << "\t\t\t\t\t\tend\n";
    myfile << "\t\t\t\t\tend else state_valid_n = IDLE;\n";
    myfile << "\t\t\t\tend\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\t\tdefault: begin\n";
    myfile << "\t\t\tend\n";
    myfile << "\t\tendcase\n";
    myfile << "\tend\n";

    myfile << "\tassign instr_gnt_o    = ram_instr_gnt;\n";
    myfile << "\tassign instr_rvalid_o = ram_instr_valid;\n";
    myfile << "\tassign instr_rdata_o  = core_instr_rdata;\n";
    myfile << "\talways_comb begin\n";
    myfile << "\t\tram_instr_req    = instr_req_i;\n";
    myfile << "\t\tram_instr_addr   = instr_addr_i;\n";
    myfile << "\t\tram_instr_gnt    = instr_req_i;\n";
    myfile << "\t\tram_instr_valid  = instr_rvalid_q;\n";
    myfile << "\t\tcore_instr_rdata = ram_instr_rdata;\n";
    myfile << "\t\tram_data_valid   = data_rvalid_q;\n";
    myfile << "\t\tram_data_gnt     = data_req_dec;\n";
    myfile << "\t\tcore_data_rdata  = ram_data_rdata;\n";
    myfile << "\t\tram_data_wdata   = data_wdata_dec;\n";
    myfile << "\t\tram_data_we      = data_we_dec;\n";
    myfile << "\t\tram_data_be      = data_be_dec;\n";
    myfile << "\t\tram_data_addr	 = data_addr_dec[BANK_ADDR_WIDTH - 1:0];\n";
    myfile << "\t\tcgra_bank_enable = {" << mem_num << "{cgra_enable}};\n";
    myfile << "\tend\n";
    myfile << "\talways_comb begin\n";
    myfile << "\t\tram_bank_enable = '0;\n";
    for (int i = 0 ; i < mem_num; i++) {
        myfile << "\t\t";
        if(i == 0) {
            myfile << "if (data_addr_dec < 2**BANK_ADDR_WIDTH) begin";
        } else {
            myfile << "end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * " << 
            i << " - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * " << i+1 << ") begin;\n";
        }
        myfile << "\t\t\tram_bank_enable[" << i << "]  = data_req_dec;\n";
    }
    myfile << "\t\tend\n";
    myfile << "\tend\n";
    myfile << "\talways_comb begin\n";
    myfile << "\t\tram_data_rdata = '0;\n";
    for (int i = 0 ; i < mem_num; i++) {
        myfile << "\t\t";
        if(i == 0) {
            myfile << "if (data_addr_dec_q < 2**BANK_ADDR_WIDTH) begin";
        } else {
            myfile << "end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * " << 
            i << " - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * " << i+1 << ") begin;\n";
        }
        myfile << "\t\t\tram_data_rdata = ram_data_rdata_tmp[" << i << "];\n";
    }
    myfile << "\t\tend\n";
    myfile << "\tend\n";
    myfile << "\tassign irq_software_o = irq_rnd_lines.irq_software;\n";
    myfile << "\tassign irq_timer_o    = irq_rnd_lines.irq_timer | irq_timer_q;\n";
    myfile << "\tassign irq_external_o = irq_rnd_lines.irq_external;\n";
    myfile << "\tassign irq_fast_o     = irq_rnd_lines.irq_fast;\n";
    myfile << "\tassign irq_nmi_o      = irq_rnd_lines.irq_nmi;\n";
    myfile << "\tassign irq_fastx_o    = irq_rnd_lines.irq_fastx;\n";
    myfile << "endmodule";
}

bool IsPowerOfTwo(int x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

void buildHybridHeader(ofstream &header, vector<string> memPort, vector<string> ioPort) {
    int io = 4194304;
    for (int i = 0; i < ioPort.size(); i++) {
        header << "\t.set " << ioPort[i] << ", " << io + 4*i << std::endl;
    }
    header << "\t.set counter " << ", " << io + 4*ioPort.size() << std::endl;
    header << "\t.set endport" << ", " << io + 4*(ioPort.size()+1) << std::endl;
    header << "\t.set reset" << ", " << io + 4*(ioPort.size()+2) << std::endl;
    header << "\t.set selport" << ", " << io + 4*(ioPort.size()+3) << std::endl;
    for (int i = 0; i < memPort.size(); i++) {
        header << "\t.set " << memPort[i] << ", " << io/memPort.size()*i << std::endl;
    }
}
