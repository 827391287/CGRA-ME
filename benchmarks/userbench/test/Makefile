PROJECT = cgrame
DATA_DOT_FILE = test_kernal/final_graph_loop.dot
FIX_PORT_DATA = test_kernal/test_ports.txt
FFT_C_SOURCE = firmware/test_main.c
COL = 4
ROW = 4
II = 1
MEMORY = 22
MAPPER = -m ILPHeuristicMapper
ARCH = hycube
PE_CONN = 255
MAPPER_OPT = --mapper-opts "ILPHeuristicMapper.allow_recomputation=true"
VERILOG_PATH = rtl
HYBRID=on
VERILOG_OPT = --verilog-opts "hybrid=$(HYBRID) memsize=$(MEMORY)"

COMPILER = riscv32-unknown-elf

#ASSEM_DATA = firmware/fft_kernal/fir4.S
#ASSEM_COUNTER = firmware/counter/cgra_fir_4_1.S
#VSIM = vsim
#VLOG = vlog

#MODELSIM_DIR ?= /opt/intelFPGA_pro/20.1/modelsim_ase/
#VLOG ?= $(MODELSIM_DIR)/bin/vlog
#VSIM ?= $(MODELSIM_DIR)/bin/vsim
#VLIB ?= $(MODELSIM_DIR)/bin/vlib

#all_counter: riscv bitstream_counter hybrid hex_counter simulation
all_data: bitstream_data hybrid hex_test package
#compile the verilog files for riscv processor
#riscv:
#	echo "Compile hardware of RISC-V processor"
#	${VLIB} work
#	./riscv_compile.sh

#example of how to generate bitstream
#bitstream_counter: 
#	echo "Generate bitstream for $(COL)x$(ROW) $(ARCH) CGRA with application $(COUNTER_DOT_FILE)"
#	$(PROJECT) -c $(ARCH) -g $(COUNTER_DOT_FILE) $(MAPPER) $(MAPPER_OPT) --arch-opts "pe_conn=$(PE_CONN) cols=$(COL) rows=$(ROW)" --gen-testbench --fixed-ports $(FIX_PORT_COUNTER)

bitstream_data:
	echo "Generate bitstream for $(COL)x$(ROW) $(ARCH) CGRA with application $(DATA_DOT_FILE)"
	$(PROJECT) -c $(ARCH) -g $(DATA_DOT_FILE) $(MAPPER) $(MAPPER_OPT) --II $(II) --arch-opts "pe_conn=$(PE_CONN) cols=$(COL) rows=$(ROW)" --gen-testbench --fixed-ports $(FIX_PORT_DATA) > log.out
#generate control file and hybrid header file for the system
hybrid:
	echo "Generating Hardware RTL & Header..."
	$(PROJECT) -c $(ARCH) $(MAPPER) --II $(II) --arch-opts "pe_conn=$(PE_CONN) cols=$(COL) rows=$(ROW)" --gen-verilog $(VERILOG_PATH) $(VERILOG_OPT) 

#generate hex file for risc-v processor
hex_test:
	echo "Compiling RISC-V Firmware..."
# 	复制生成的头文件到 firmware 目录，供 C 代码引用
	cp hybrid.h firmware/
# 	编译 fft_main.c
# 	-DTEST_FUNC_NAME=main 告诉启动代码跳转到 main 函数
	$(COMPILER)-gcc -c -march=rv32g -g -o firmware/app.o \
		-DTEST_FUNC_NAME=main -DTEST_FUNC_TXT='"main"' -DTEST_FUNC_RET=main_ret \
		$(FFT_C_SOURCE)
# 	编译其他辅助库
	$(COMPILER)-gcc -c -march=rv32g -g -o firmware/print.o firmware/print.c
	$(COMPILER)-gcc -c -march=rv32g -g -o firmware/stats.o firmware/stats.c
	$(COMPILER)-gcc -c -march=rv32g -g -o firmware/start.o firmware/start.S	
# 	链接生成 ELF
	$(COMPILER)-gcc -g -Os -march=rv32g -ffreestanding -nostdlib -o firmware/firmware.elf \
		-Wl,-Bstatic,-T,firmware/link.ld \
		firmware/start.o firmware/print.o firmware/stats.o firmware/app.o -lgcc
	
# 	转换成 Verilog 格式的 Hex 文件
	$(COMPILER)-objcopy -O verilog firmware/firmware.elf firmware/firmware.hex
	$(COMPILER)-objdump -M numeric,no-aliases -S -D firmware/firmware.elf > firmware/firmware.dump

#start simulation for the hybrid system	
#simulation:
#	echo "Start simlation"
#	if [ ! -d "work" ]; then vlib work; fi
#	vlog $(VERILOG_PATH)/control.sv $(VERILOG_PATH)/cgrame.v testbench.v instruc_ram.sv dp_ram.sv riscv_tracer_defines.sv riscv_tracer.sv tb_top.sv
#	vsim work.tb_top +firmware=firmware/firmware.hex
#	do wave.do
#	run -all	 
package:
	echo "Packaging for Windows Simulation..."
	rm -rf test_sim_pkg
	mkdir -p test_sim_pkg/firmware
	
# A. 复制生成的硬件
	cp rtl/cgrame.v test_sim_pkg/
	cp rtl/control.sv test_sim_pkg/
	cp hybrid.h test_sim_pkg/
	cp filelist.f test_sim_pkg/
	
# B. 复制 Testbench 和模型文件
# 注意：如果你有 riscv_core.sv 等源码，确保它们也在当前目录或指定位置
	cp rtl/tb_top.sv testbench.v rtl/instruc_ram.sv rtl/dp_ram.sv rtl/riscv_tracer*.sv test_sim_pkg/ 

# C. 复制固件
	cp firmware/firmware.hex test_sim_pkg/firmware/
	cp firmware/firmware.dump test_sim_pkg/firmware/
# D. 生成 Windows ModelSim 脚本 (run.do)
	echo "vlib work" >> test_sim_pkg/run.do
	echo "vlog -f filelist.f" >> test_sim_pkg/run.do
	echo "vsim -voptargs=+acc work.tb_top +firmware=firmware/firmware.hex" >> test_sim_pkg/run.do
	echo "-----------------------------------------------------"
	echo "SUCCESS! Copy the 'test_sim_pkg' folder to Windows."
	echo "-----------------------------------------------------"


#clean: 
#	rm -rf work hybrid.h firmware/*.o firmware/*.hex firmware/*.map $(VERILOG_PATH)/control.sv $(VERILOG_PATH)/cgrame.v transcript vsim.wlf trace_core_00* 
clean: 
	rm -rf  firmware/*.o firmware/*.hex firmware/*.elf  firmware/firmware.dump 
#log.out testbench.v rtl/cgrame.v rtl/control.sv firmware/hybrid.h   hybrid.h 
