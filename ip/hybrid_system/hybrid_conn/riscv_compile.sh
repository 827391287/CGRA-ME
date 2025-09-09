#!/bin/bash
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/fpnew_pkg.sv
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/apu_core_package.sv
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/apu_macros.sv
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_config.sv
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_defines.sv
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/cv32e40p_sim_clock_gating.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_prefetch_buffer.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_prefetch_L0_buffer.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_L0_buffer.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_fetch_fifo.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_if_stage.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/register_file_test_wrap.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_register_file.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_decoder.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_compressed_decoder.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_id_stage.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/amo_shim.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_hwloop_regs.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_cs_registers.sv  -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_pmp.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_controller.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_int_controller.sv  -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_hwloop_controller.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_alu_basic.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_mult.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_alu_div.sv  -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_alu.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_ex_stage.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_apu_disp.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_load_store_unit.sv -suppress 2583
/opt/intelFPGA_pro/20.1/modelsim_ase/bin/vlog ../riscv/riscv_core.sv -suppress 2583
