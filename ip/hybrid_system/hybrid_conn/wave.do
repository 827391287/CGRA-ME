onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/counter
add wave -noupdate /tb_top/riscv_wrapper_i/CGRA_Enable
add wave -noupdate /tb_top/riscv_wrapper_i/CGRA_Reset_Control
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_0_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_0_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_1_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_1_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_2_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_2_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_3_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_3_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_4_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_4_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_5_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_5_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_6_IOPin_bidir_in
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/io_top_6_IOPin_bidir_out
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_0_mem_unit_addr_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_0_mem_unit_data_in_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_0_mem_unit_data_out_from_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_0_mem_unit_w_rq_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_1_mem_unit_addr_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_1_mem_unit_data_in_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_1_mem_unit_data_out_from_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_1_mem_unit_w_rq_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_2_mem_unit_addr_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_2_mem_unit_data_in_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_2_mem_unit_data_out_from_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_2_mem_unit_w_rq_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_3_mem_unit_addr_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_3_mem_unit_data_in_to_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_3_mem_unit_data_out_from_ram
add wave -noupdate -radix unsigned /tb_top/riscv_wrapper_i/mem_3_mem_unit_w_rq_to_ram
add wave -noupdate /tb_top/riscv_wrapper_i/cgra_data_addr
add wave -noupdate /tb_top/riscv_wrapper_i/cgra_data_rdata
add wave -noupdate /tb_top/riscv_wrapper_i/cgra_data_wdata
add wave -noupdate /tb_top/riscv_wrapper_i/cgra_we_i
add wave -noupdate -group tb_top /tb_top/CLK_PHASE_HI
add wave -noupdate -group tb_top /tb_top/CLK_PHASE_LO
add wave -noupdate -group tb_top /tb_top/CLK_PERIOD
add wave -noupdate -group tb_top /tb_top/STIM_APPLICATION_DEL
add wave -noupdate -group tb_top /tb_top/RESP_ACQUISITION_DEL
add wave -noupdate -group tb_top /tb_top/RESET_DEL
add wave -noupdate -group tb_top /tb_top/RESET_WAIT_CYCLES
add wave -noupdate -group tb_top /tb_top/clk_i
add wave -noupdate -group tb_top /tb_top/rst_ni
add wave -noupdate -group tb_top /tb_top/cycle_cnt_q
add wave -noupdate -group tb_top /tb_top/fetch_enable
add wave -noupdate -group tb_top /tb_top/Config_Clock_en
add wave -noupdate -group tb_top /tb_top/Config_Reset
add wave -noupdate -group tb_top /tb_top/CGRA_Clock_en
add wave -noupdate -group tb_top /tb_top/CGRA_Reset
add wave -noupdate -group tb_top /tb_top/riscv_enable
add wave -noupdate -group tb_top /tb_top/configurator_enable
add wave -noupdate -group tb_top /tb_top/configurator_reset
add wave -noupdate -group tb_top /tb_top/configurator_done
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/clk_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/rst_ni
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/CGRA_Reset
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/instr_req_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/instr_addr_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/instr_rdata_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/instr_rvalid_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/instr_gnt_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_req_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_addr_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_we_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_be_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_wdata_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_rdata_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_rvalid_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/riscv_gnt_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/cgra_enable
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/cgra_data_rdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/cgra_we_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_id_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_ack_i
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_software_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_timer_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_external_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_fast_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_nmi_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_fastx_o
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/select_rdata_d
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/select_rdata_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/transaction
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/state_valid_n
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/state_valid_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_addr_aligned
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_rvalid_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/instr_rvalid_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/core_instr_rdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/core_data_rdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_bank_enable
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_addr
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_wdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_rdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_we
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_be
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_gnt
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_data_valid
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_req_dec
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_wdata_dec
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_addr_dec
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_we_dec
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_be_dec
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/data_addr_dec_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_instr_rdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_instr_req
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_instr_addr
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_instr_gnt
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/ram_instr_valid
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/timer_irq_mask_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/timer_cnt_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_timer_q
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/timer_reg_valid
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/timer_val_valid
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/timer_wdata
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/cgra_bank_enable
add wave -noupdate -group ram_i /tb_top/riscv_wrapper_i/ram_i/irq_rnd_lines
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/clk_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/rst_ni
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/rst_CGRA
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/en_a_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/addr_a_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/wdata_a_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/rdata_a_o
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/we_a_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/be_a_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/en_b_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/addr_b_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/wdata_b_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/rdata_b_o
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/we_b_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/be_b_i
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/addr_a_int
add wave -noupdate -group dp_ram_0 /tb_top/riscv_wrapper_i/ram_i/dp_ram_0/addr_b_int
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/clk_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/rst_ni
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/rst_CGRA
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/en_a_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/addr_a_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/wdata_a_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/rdata_a_o
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/we_a_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/be_a_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/en_b_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/addr_b_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/wdata_b_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/rdata_b_o
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/we_b_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/be_b_i
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/addr_a_int
add wave -noupdate -group dp_ram_1 /tb_top/riscv_wrapper_i/ram_i/dp_ram_1/addr_b_int
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/clk_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/rst_ni
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/rst_CGRA
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/en_a_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/addr_a_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/wdata_a_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/rdata_a_o
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/we_a_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/be_a_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/en_b_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/addr_b_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/wdata_b_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/rdata_b_o
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/we_b_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/be_b_i
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/addr_a_int
add wave -noupdate -group dp_ram_2 /tb_top/riscv_wrapper_i/ram_i/dp_ram_2/addr_b_int
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/clk_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/rst_ni
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/rst_CGRA
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/en_a_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/addr_a_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/wdata_a_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/rdata_a_o
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/we_a_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/be_a_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/en_b_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/addr_b_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/wdata_b_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/rdata_b_o
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/we_b_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/be_b_i
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/addr_a_int
add wave -noupdate -group dp_ram_3 /tb_top/riscv_wrapper_i/ram_i/dp_ram_3/addr_b_int
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/clk_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/rst_ni
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/clock_en_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/test_en_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/fregfile_disable_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/boot_addr_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/core_id_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/cluster_id_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_req_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_gnt_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_rvalid_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_addr_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_rdata_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_req_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_gnt_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_rvalid_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_we_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_be_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_addr_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_wdata_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_rdata_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_atop_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_req_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_ready_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_gnt_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_operands_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_op_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_type_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_flags_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_valid_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_result_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_master_flags_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_ack_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_id_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_sec_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_software_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_timer_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_external_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_fast_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_nmi_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_fastx_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/sec_lvl_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_req_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/fetch_enable_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/core_busy_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/ext_perf_counters_i
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/is_hwlp_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/hwlp_dec_cnt_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_valid_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_rdata_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/is_compressed_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/is_fetch_failed_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/illegal_c_insn_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pc_if
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pc_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/clear_instr_valid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pc_set
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pc_mux_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/exc_pc_mux_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/exc_cause
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/trap_addr_mux
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/lsu_load_err
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/lsu_store_err
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/is_decoding
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/useincr_addr_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_misaligned
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_multicycle
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/jump_target_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/jump_target_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/branch_in_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/branch_decision
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/ctrl_busy
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/if_busy
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/lsu_busy
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_busy
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pc_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_en_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_operator_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_operand_a_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_operand_b_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_operand_c_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/bmask_a_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/bmask_b_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/imm_vec_ext_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_vec_mode_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_is_clpx_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_is_subrot_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/alu_clpx_shift_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_operator_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_operand_a_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_operand_b_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_operand_c_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_en_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_sel_subword_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_signed_mode_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_imm_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_dot_op_a_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_dot_op_b_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_dot_op_c_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_dot_signed_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_is_clpx_ex_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_clpx_shift_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_clpx_img_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/fprec_csr
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/frm_csr
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/fflags
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/fflags_csr
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/fflags_we
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_en_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_type_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_flags_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_op_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_lat_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_operands_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_waddr_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_read_regs
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_read_regs_valid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_read_dep
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_write_regs
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_write_regs_valid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_write_dep
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_apu_type
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_apu_cont
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_apu_dep
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_apu_wb
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_waddr_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_we_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_waddr_fw_wb_o
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_we_wb
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_wdata
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_alu_waddr_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_alu_we_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_alu_waddr_fw
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_alu_we_fw
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/regfile_alu_wdata_fw
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_access_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_op_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mtvec
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mtvecx
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/utvec
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_access
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_op
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_addr
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_addr_int
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_rdata
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_wdata
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/current_priv_lvl
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_we_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_atop_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_type_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_sign_ext_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_reg_offset_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_req_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_load_event_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_misaligned_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/lsu_rdata
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/halt_if
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/id_ready
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/ex_ready
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/id_valid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/ex_valid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/wb_valid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/lsu_ready_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/lsu_ready_wb
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/apu_ready_wb
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_req_int
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/m_irq_enable
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/u_irq_enable
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_irq_sec
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mepc
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/uepc
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/depc
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_software
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_timer
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_external
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_fast
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_nmi
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_save_cause
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_save_if
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_save_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_save_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_cause
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_restore_mret_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_restore_uret_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_restore_dret_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_mode
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_cause
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_csr_save
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_single_step
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_ebreakm
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/debug_ebreaku
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/hwlp_start
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/hwlp_end
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/hwlp_cnt
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_hwlp_regid
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_hwlp_we
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/csr_hwlp_data
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_imiss
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_jump
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_jr_stall
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_ld_stall
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/perf_pipeline_stall
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/core_ctrl_firstfetch
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/core_busy_int
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/core_busy_q
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pmp_addr
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/pmp_cfg
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_req_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_addr_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_we_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_gnt_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_err_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/data_err_ack
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_req_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_gnt_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_addr_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/instr_err_pmp
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_pending
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/irq_id
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/is_interrupt
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/clk
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/clock_en
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/sleeping
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/mult_is_clpx_ex
add wave -noupdate -group Core /tb_top/riscv_wrapper_i/riscv_core_i/tracer_clk
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/current_pc_i
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_start_addr_i
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_end_addr_i
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_counter_i
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_dec_cnt_o
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_dec_cnt_id_i
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_jump_o
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/hwlp_targ_addr_o
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/pc_is_end_addr
add wave -noupdate -group {IF Stage} -group {Hwlp Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwloop_controller_i/j
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/clk
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/rst_n
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/prefetch_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/prefetch_addr_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/branch_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/branch_addr_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/hwlp_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/hwlp_addr_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/fetch_gnt_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/fetch_valid_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/valid_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/rdata_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/addr_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_req_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_addr_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_gnt_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_rvalid_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_rdata_i
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/busy_o
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/CS
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/NS
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/L0_buffer
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/addr_q
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/instr_addr_int
add wave -noupdate -group {IF Stage} -group Prefetch -group L0 /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/L0_buffer_i/valid
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/clk
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/rst_n
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/req_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/branch_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/hwloop_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/hwloop_target_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/ready_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/valid_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/is_hwlp_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/instr_req_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/instr_addr_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/instr_gnt_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/instr_rvalid_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/instr_rdata_i
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/busy_o
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/busy_L0
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/CS
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/NS
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/do_fetch
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/do_hwlp
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/do_hwlp_int
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/use_last
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/save_rdata_last
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/use_hwlp
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/save_rdata_hwlp
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/valid
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/hwlp_is_crossword
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/is_crossword
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/next_is_crossword
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/next_valid
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/next_upper_compressed
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/fetch_possible
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/upper_is_compressed
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_q
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_n
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_int
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_aligned_next
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_real_next
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/is_hwlp_q
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/is_hwlp_n
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_last_q
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/valid_L0
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_L0
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/addr_L0
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/fetch_valid
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/fetch_gnt
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/rdata
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/rdata_unaligned
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/aligned_is_compressed
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/unaligned_is_compressed
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/hwlp_aligned_is_compressed
add wave -noupdate -group {IF Stage} -group Prefetch /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_128/prefetch_buffer_i/hwlp_unaligned_is_compressed
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/clk
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/rst_n
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/m_trap_base_addr_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/m_trap_base_addrx_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/u_trap_base_addr_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/trap_addr_mux_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/boot_addr_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/req_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_req_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_addr_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_gnt_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_rvalid_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_rdata_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_err_pmp_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_dec_cnt_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/is_hwlp_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_valid_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_rdata_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/is_compressed_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/illegal_c_insn_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/pc_if_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/pc_id_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/is_fetch_failed_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/clear_instr_valid_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/pc_set_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/mepc_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/uepc_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/depc_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/pc_mux_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/exc_pc_mux_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/exc_vec_pc_mux_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/jump_target_id_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/jump_target_ex_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_start_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_end_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_cnt_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/halt_if_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/id_ready_i
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/if_busy_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/perf_imiss_o
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/offset_fsm_cs
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/offset_fsm_ns
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/if_valid
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/if_ready
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/valid
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/prefetch_busy
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/branch_req
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_addr_n
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_valid
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_ready
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_rdata
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_addr
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/is_hwlp_id_q
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_is_hwlp
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/exc_pc
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_jump
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_branch
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_target
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_dec_cnt
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/hwlp_dec_cnt_if
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/trap_base_addr
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/fetch_failed
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_decompressed
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/illegal_c_insn
add wave -noupdate -group {IF Stage} /tb_top/riscv_wrapper_i/riscv_core_i/if_stage_i/instr_compressed_int
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/clk
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/rst_n
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/test_en_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fregfile_disable_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fetch_enable_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/ctrl_busy_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/core_ctrl_firstfetch_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/is_decoding_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwlp_dec_cnt_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/is_hwlp_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/instr_valid_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/instr_rdata_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/instr_req_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/branch_in_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/branch_decision_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/jump_target_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/clear_instr_valid_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/pc_set_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/pc_mux_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/exc_pc_mux_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/trap_addr_mux_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/illegal_c_insn_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/is_compressed_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/is_fetch_failed_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/pc_if_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/pc_id_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/halt_if_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/id_ready_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/ex_ready_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/wb_ready_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/id_valid_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/ex_valid_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/pc_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operand_a_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operand_b_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operand_c_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_a_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_b_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_vec_ext_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_vec_mode_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_waddr_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_we_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_waddr_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_we_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_en_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operator_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_is_clpx_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_is_subrot_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_clpx_shift_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_operator_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_operand_a_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_operand_b_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_operand_c_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_en_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_sel_subword_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_signed_mode_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_imm_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_dot_op_a_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_dot_op_b_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_dot_op_c_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_dot_signed_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_is_clpx_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_clpx_shift_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_clpx_img_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_en_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_type_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_op_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_lat_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_operands_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_flags_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_waddr_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_read_regs_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_read_regs_valid_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_read_dep_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_write_regs_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_write_regs_valid_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_write_dep_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_perf_dep_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_busy_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/frm_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_access_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_op_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/current_priv_lvl_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_irq_sec_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_cause_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_save_if_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_save_id_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_save_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_restore_mret_id_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_restore_uret_id_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_restore_dret_id_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_save_cause_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwlp_start_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwlp_end_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwlp_cnt_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_hwlp_regid_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_hwlp_we_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_hwlp_data_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_req_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_we_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_type_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_sign_ext_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_reg_offset_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_load_event_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_misaligned_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/prepost_useincr_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_misaligned_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_err_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_err_ack_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/atop_ex_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_pending_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_sec_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_id_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/m_irq_enable_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/u_irq_enable_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_ack_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_id_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/exc_cause_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_mode_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_cause_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_csr_save_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_req_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_single_step_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_ebreakm_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/debug_ebreaku_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_waddr_wb_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_we_wb_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_wdata_wb_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_waddr_fw_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_we_fw_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_wdata_fw_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_multicycle_i
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/perf_jump_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/perf_jr_stall_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/perf_ld_stall_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/perf_pipeline_stall_o
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/instr
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/deassert_we
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/illegal_insn_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/ebrk_insn
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mret_insn_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/uret_insn_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/dret_insn_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/ecall_insn_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/pipe_flush_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fencei_insn_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/rega_used_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regb_used_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regc_used_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/branch_taken_ex
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/jump_in_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/jump_in_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/misaligned_stall
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/jr_stall
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/load_stall
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_apu_stall
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/instr_multicycle
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_mask
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/halt_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_i_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_iz_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_s_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_sb_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_u_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_uj_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_z_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_s2_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_bi_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_s3_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_vs_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_vu_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_shuffleb_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_shuffleh_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_shuffle_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_clip_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_a
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_b
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/jump_target
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_req_ctrl
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_sec_ctrl
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/irq_id_ctrl
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/exc_ack
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/exc_kill
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_addr_ra_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_addr_rb_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_addr_rc_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_fp_a
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_fp_b
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_fp_c
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_fp_d
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fregfile_ena
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_waddr_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_waddr_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_we_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_we_dec_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_data_ra_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_data_rb_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_data_rc_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_en
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operator
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_op_a_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_op_b_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_op_c_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regc_mux
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_a_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_b_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/jump_target_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_operator
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_en
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_int_en
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_sel_subword
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_signed_mode
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_dot_en
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_dot_signed
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fpu_src_fmt
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fpu_dst_fmt
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fpu_int_fmt
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_en
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_type
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_op
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_lat
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_operands
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_flags
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_waddr
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_read_regs
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_read_regs_valid
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_write_regs
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_write_regs_valid
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_flags_src
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/apu_stall
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/fp_rnd_mode
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_we_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/regfile_alu_waddr_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_we_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_type_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_sign_ext_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_reg_offset_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_req_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/data_load_event_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/atop_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regid
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regid_int
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_we
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_we_int
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_we_masked
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_target_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_start_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_cnt_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_target
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_start
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_start_int
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_end
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_cnt
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_cnt_int
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_valid
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_access
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_op
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/csr_status
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/prepost_useincr
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_a_fw_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_b_fw_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_c_fw_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_a_fw_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_b_fw_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_c_fw_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_b
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_b_vec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_c
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/operand_c_vec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operand_a
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operand_b
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_operand_c
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_a_mux
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_b_mux
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_bmask_a_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_bmask_b_mux_sel
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_imm_mux
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_a_id_imm
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_b_id_imm
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_a_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/bmask_b_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/imm_vec_ext_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mult_imm_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/alu_vec_mode
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/scalar_replication
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/scalar_replication_c
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_ex_is_reg_a_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_ex_is_reg_b_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_ex_is_reg_c_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_wb_is_reg_a_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_wb_is_reg_b_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_wb_is_reg_c_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_alu_is_reg_a_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_alu_is_reg_b_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/reg_d_alu_is_reg_c_id
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/is_clpx
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/is_subrot
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/mret_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/uret_dec
add wave -noupdate -group {ID Stage} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/dret_dec
add wave -noupdate -group RF /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/registers_i/riscv_register_file_i/mem
add wave -noupdate -group RF_FP /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/registers_i/riscv_register_file_i/mem_fp
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/deassert_we_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_misaligned_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_multicycle_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/instr_multicycle_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/illegal_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/ebrk_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mret_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/uret_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/dret_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mret_dec_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/uret_dec_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/dret_dec_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/ecall_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/pipe_flush_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fencei_insn_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/rega_used_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regb_used_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regc_used_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/reg_fp_a_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/reg_fp_b_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/reg_fp_c_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/reg_fp_d_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/bmask_a_mux_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/bmask_b_mux_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_bmask_a_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_bmask_b_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/instr_rdata_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/illegal_c_insn_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_en_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_operator_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_op_a_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_op_b_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_op_c_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/alu_vec_mode_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/scalar_replication_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/scalar_replication_c_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/imm_a_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/imm_b_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regc_mux_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/is_clpx_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/is_subrot_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_operator_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_int_en_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_dot_en_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_imm_mux_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_sel_subword_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_signed_mode_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_dot_signed_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/frm_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fpu_dst_fmt_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fpu_src_fmt_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fpu_int_fmt_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/apu_en_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/apu_type_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/apu_op_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/apu_lat_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/apu_flags_src_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fp_rnd_mode_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regfile_mem_we_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regfile_alu_we_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regfile_alu_we_dec_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regfile_alu_waddr_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/csr_access_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/csr_status_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/csr_op_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/current_priv_lvl_i
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_req_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_we_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/prepost_useincr_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_type_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_sign_extension_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_reg_offset_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_load_event_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/atop_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/hwloop_we_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/hwloop_target_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/hwloop_start_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/hwloop_cnt_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/jump_in_dec_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/jump_in_id_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/jump_target_mux_sel_o
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regfile_mem_we
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/regfile_alu_we
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/data_req
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/hwloop_we
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/csr_illegal
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/jump_in_id
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/csr_op
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_int_en
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/mult_dot_en
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/apu_en
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/check_fprm
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fpu_op
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fpu_op_mod
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fpu_vec_op
add wave -noupdate -group Decoder /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/decoder_i/fp_op_group
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/clk
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/rst_n
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/fetch_enable_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ctrl_busy_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/first_fetch_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/is_decoding_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/is_fetch_failed_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/deassert_we_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/illegal_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ecall_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/mret_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/uret_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/dret_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/mret_dec_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/uret_dec_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/dret_dec_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/pipe_flush_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ebrk_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/fencei_insn_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_status_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/instr_multicycle_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/hwloop_mask_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/instr_valid_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/instr_req_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/pc_set_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/pc_mux_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/exc_pc_mux_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/trap_addr_mux_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_req_ex_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_we_ex_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_misaligned_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_load_event_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_err_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_err_ack_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/mult_multicycle_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/apu_en_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/apu_read_dep_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/apu_write_dep_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/apu_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/branch_taken_ex_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/jump_in_id_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/jump_in_dec_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_pending_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_req_ctrl_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_sec_ctrl_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_id_ctrl_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/m_IE_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/u_IE_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/current_priv_lvl_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_ack_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_id_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/exc_cause_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/exc_ack_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/exc_kill_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_mode_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_cause_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_csr_save_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_req_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_single_step_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_ebreakm_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_ebreaku_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_save_if_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_save_id_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_save_ex_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_cause_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_irq_sec_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_restore_mret_id_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_restore_uret_id_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_restore_dret_id_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/csr_save_cause_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/regfile_we_id_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/regfile_alu_waddr_id_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/regfile_we_ex_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/regfile_waddr_ex_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/regfile_we_wb_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/regfile_alu_we_fw_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/operand_a_fw_mux_sel_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/operand_b_fw_mux_sel_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/operand_c_fw_mux_sel_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_ex_is_reg_a_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_ex_is_reg_b_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_ex_is_reg_c_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_wb_is_reg_a_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_wb_is_reg_b_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_wb_is_reg_c_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_alu_is_reg_a_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_alu_is_reg_b_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/reg_d_alu_is_reg_c_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/halt_if_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/halt_id_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/misaligned_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/jr_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/load_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/id_ready_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ex_valid_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/wb_ready_i
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/perf_jump_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/perf_jr_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/perf_ld_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/perf_pipeline_stall_o
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ctrl_fsm_cs
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ctrl_fsm_ns
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/jump_done
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/jump_done_q
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/jump_in_dec
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/branch_in_id
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/boot_done
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/boot_done_q
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/irq_enable_int
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/data_err_q
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_mode_q
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/debug_mode_n
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/ebrk_force_debug_mode
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/illegal_insn_q
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/illegal_insn_n
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/instr_valid_irq_flush_n
add wave -noupdate -group Controller /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/controller_i/instr_valid_irq_flush_q
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/clk
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/rst_n
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_req_ctrl_o
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_sec_ctrl_o
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_id_ctrl_o
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/ctrl_ack_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/ctrl_kill_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_pending_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_sec_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_id_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/m_IE_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/u_IE_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/current_priv_lvl_i
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/exc_ctrl_cs
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_enable_ext
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_id_q
add wave -noupdate -group {Int Ctrl} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/int_controller_i/irq_sec_q
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/clk
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/rst_n
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_start_data_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_end_data_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_cnt_data_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_we_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_regid_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/valid_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_dec_cnt_i
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_start_addr_o
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_end_addr_o
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_counter_o
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_start_q
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_end_q
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_counter_q
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/hwlp_counter_n
add wave -noupdate -group {Hwloop Regs} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/hwloop_regs_i/i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/clk
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/rst_n
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/enable_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operator_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_a_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_b_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_c_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/vector_mode_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bmask_a_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bmask_b_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/imm_vec_ext_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_clpx_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_subrot_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/clpx_shift_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/result_o
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/comparison_result_o
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/ready_o
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/ex_ready_i
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_a_rev
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_a_neg
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_a_neg_rev
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_b_neg
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/div_shift
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/div_valid
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bmask
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_op_b_negate
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_op_a
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_op_b
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_in_a
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_in_b
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_result_expanded
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_round_value
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/adder_round_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_left
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_use_round
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_arithmetic
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_amt_left
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_amt
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_amt_int
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_amt_norm
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_op_a
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_right_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_left_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/clpx_shift_ex
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shift_op_a_32
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_equal
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_greater
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/f_is_greater
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/cmp_signed
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_equal_vec
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_greater_vec
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/operand_b_eq
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/is_equal_clip
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/cmp_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/f_is_qnan
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/f_is_snan
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/f_is_nan
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/result_minmax
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/fp_canonical_nan
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/sel_minmax
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/do_min
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/minmax_is_fp_special
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/minmax_b
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/fclass_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/f_sign_inject_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/clip_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_byte_sel
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_reg_sel
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_reg1_sel
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_reg0_sel
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_through
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_r1
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_r0
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_r1_in
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_r0_in
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/shuffle_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/pack_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/ff_input
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/cnt_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/clb_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/ff1_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/ff_no_one
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/fl1_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bitop_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/extract_is_signed
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/extract_sign
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bmask_first
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bmask_inv
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bextins_and
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bextins_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bclr_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/bset_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/radix_2_rev
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/radix_4_rev
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/radix_8_rev
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/reverse_result
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/radix_mux_sel
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/result_div
add wave -noupdate -group {EX Stage} -group ALU /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/div_ready
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/Clk_CI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/Rst_RBI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OpA_DI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OpB_DI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OpBShift_DI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OpBIsZero_SI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OpBSign_SI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OpCode_SI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/InVld_SI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OutRdy_SI
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OutVld_SO
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/Res_DO
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ResReg_DP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ResReg_DN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ResReg_DP_rev
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/AReg_DP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/AReg_DN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/BReg_DP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/BReg_DN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/RemSel_SN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/RemSel_SP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/CompInv_SN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/CompInv_SP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ResInv_SN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ResInv_SP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/AddMux_D
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/AddOut_D
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/AddTmp_D
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/BMux_D
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/OutMux_D
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/Cnt_DP
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/Cnt_DN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/CntZero_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ARegEn_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/BRegEn_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ResRegEn_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/ABComp_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/PmSel_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/LoadEn_S
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/State_SN
add wave -noupdate -group {EX Stage} -group ALU_DIV /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_i/int_div/div_i/State_SP
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/clk
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/rst_n
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/enable_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/operator_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_subword_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_signed_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/op_a_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/op_b_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/op_c_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/imm_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/dot_signed_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/dot_op_a_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/dot_op_b_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/dot_op_c_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/is_clpx_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/clpx_shift_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/clpx_img_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/result_o
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/multicycle_o
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/ready_o
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/ex_ready_i
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_op_a
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_op_b
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_op_c
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_mul
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_mac
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_round
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_round_tmp
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_result
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_mac_msb1
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_mac_msb0
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_imm
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_subword
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_signed
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/short_shift_arith
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_imm
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_subword
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_signed
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_shift_arith
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_carry_q
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_active
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_save
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_clearcarry
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_ready
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_CS
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/mulh_NS
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/int_op_a_msu
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/int_op_b_msu
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/int_result
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/int_is_msu
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/dot_char_result
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/dot_short_result
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/accumulator
add wave -noupdate -group {EX Stage} -group MUL /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_i/clpx_shift_result
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/clk
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/rst_n
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_operator_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_operand_a_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_operand_b_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_operand_c_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_en_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/bmask_a_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/bmask_b_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/imm_vec_ext_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_vec_mode_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_is_clpx_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_is_subrot_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_clpx_shift_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_operator_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_operand_a_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_operand_b_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_operand_c_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_en_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_sel_subword_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_signed_mode_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_imm_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_dot_op_a_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_dot_op_b_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_dot_op_c_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_dot_signed_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_is_clpx_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_clpx_shift_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_clpx_img_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_multicycle_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/fpu_prec_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/fpu_fflags_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/fpu_fflags_we_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_en_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_op_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_lat_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_operands_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_waddr_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_flags_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_read_regs_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_read_regs_valid_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_read_dep_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_write_regs_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_write_regs_valid_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_write_dep_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_perf_type_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_perf_cont_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_perf_wb_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_busy_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_ready_wb_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_req_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_ready_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_gnt_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_operands_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_op_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_valid_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_master_result_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/lsu_en_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/lsu_rdata_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/branch_in_ex_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_alu_waddr_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_alu_we_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_we_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_waddr_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/csr_access_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/csr_rdata_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_waddr_wb_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_we_wb_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_wdata_wb_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_alu_waddr_fw_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_alu_we_fw_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_alu_wdata_fw_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/jump_target_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/branch_decision_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/lsu_ready_ex_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/lsu_err_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/ex_ready_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/ex_valid_o
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/wb_ready_i
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_result
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_result
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_cmp_result
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_we_lsu
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/regfile_waddr_lsu
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/wb_contention
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/wb_contention_lsu
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/alu_ready
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/mult_ready
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/fpu_ready
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/fpu_valid
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_valid
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_waddr
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_result
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_stall
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_active
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_singlecycle
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_multicycle
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_req
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_ready
add wave -noupdate -group {EX Stage} /tb_top/riscv_wrapper_i/riscv_core_i/ex_stage_i/apu_gnt
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/clk
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/rst_n
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_req_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_gnt_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_rvalid_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_err_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_addr_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_we_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_be_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_wdata_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_rdata_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_we_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_type_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_wdata_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_reg_offset_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_sign_ext_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_rdata_ex_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_req_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/operand_a_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/operand_b_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/addr_useincr_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_misaligned_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_misaligned_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_atop_ex_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_atop_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/lsu_ready_ex_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/lsu_ready_wb_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/ex_valid_i
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/busy_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_addr_int
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_type_q
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/rdata_offset_q
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_sign_ext_q
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_we_q
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/wdata_offset
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_be
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_wdata
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/misaligned_st
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/CS
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/NS
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/rdata_q
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/data_rdata_ext
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/rdata_w_ext
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/rdata_h_ext
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/rdata_b_ext
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/load_err_o
add wave -noupdate -group LSU /tb_top/riscv_wrapper_i/riscv_core_i/load_store_unit_i/store_err_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/clk
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/rst_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/core_id_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/cluster_id_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mtvec_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mtvecx_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/utvec_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/boot_addr_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_access_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_addr_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_wdata_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_op_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_rdata_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/frm_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fprec_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fflags_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fflags_we_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_software_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_timer_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_external_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_fast_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_nmi_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_fastx_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/m_irq_enable_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/u_irq_enable_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_pending_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_id_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_irq_sec_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/sec_lvl_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mepc_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/uepc_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/debug_mode_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/debug_cause_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/debug_csr_save_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/depc_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/debug_single_step_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/debug_ebreakm_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/debug_ebreaku_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pmp_addr_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pmp_cfg_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/priv_lvl_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pc_if_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pc_id_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pc_ex_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_save_if_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_save_id_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_save_ex_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_restore_mret_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_restore_uret_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_restore_dret_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_cause_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_save_cause_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/hwlp_start_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/hwlp_end_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/hwlp_cnt_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/hwlp_data_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/hwlp_regid_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/hwlp_we_o
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/id_valid_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/is_compressed_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/is_decoding_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/imiss_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pc_set_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/jump_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/branch_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/branch_taken_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/ld_stall_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/jr_stall_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pipeline_stall_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/apu_typeconflict_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/apu_contention_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/apu_dep_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/apu_wb_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mem_load_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mem_store_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/ext_counters_i
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_wdata_int
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_rdata_int
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/csr_we_int
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/frm_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/frm_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fflags_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fflags_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fprec_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/fprec_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mepc_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mepc_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/uepc_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/uepc_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/dcsr_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/dcsr_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/depc_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/depc_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/dscratch0_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/dscratch0_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/dscratch1_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/dscratch1_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mscratch_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mscratch_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/exception_pc
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mstatus_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mstatus_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mcause_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mcause_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/ucause_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/ucause_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mtvec_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mtvec_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mtvecx_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mtvecx_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/utvec_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/utvec_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mip
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mipx
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mie_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/mie_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/miex_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/miex_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/menip
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/menipx
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/is_irq
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/priv_lvl_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/priv_lvl_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/priv_lvl_reg_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pmp_reg_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pmp_reg_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pmpaddr_we
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pmpcfg_we
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/id_valid_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCCR_in
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCCR_inc
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCCR_inc_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCCR_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCCR_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCMR_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCMR_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCER_n
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/PCER_q
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/perf_rdata
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pccr_index
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/pccr_all_sel
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/is_pccr
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/is_pcer
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/is_pcmr
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_req
add wave -noupdate -group CSR /tb_top/riscv_wrapper_i/riscv_core_i/cs_registers_i/irq_reqx
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/clk_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/rst_ni
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/CGRA_Reset
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/instr_req_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/instr_addr_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/instr_rdata_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/instr_rvalid_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/instr_gnt_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_req_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_addr_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_we_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_be_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_wdata_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_rdata_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_rvalid_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/riscv_gnt_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/cgra_enable
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/cgra_data_rdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/cgra_we_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_id_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_ack_i
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_software_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_timer_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_external_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_fast_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_nmi_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_fastx_o
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/select_rdata_d
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/select_rdata_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/transaction
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/state_valid_n
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/state_valid_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_addr_aligned
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_rvalid_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/instr_rvalid_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/core_instr_rdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/core_data_rdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_bank_enable
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_addr
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_wdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_rdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_we
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_be
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_gnt
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_data_valid
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_req_dec
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_wdata_dec
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_addr_dec
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_we_dec
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_be_dec
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/data_addr_dec_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_instr_rdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_instr_req
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_instr_addr
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_instr_gnt
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/ram_instr_valid
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/timer_irq_mask_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/timer_cnt_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_timer_q
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/timer_reg_valid
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/timer_val_valid
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/timer_wdata
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/cgra_bank_enable
add wave -noupdate -group {MM RAM} /tb_top/riscv_wrapper_i/ram_i/irq_rnd_lines
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/clk_i
add wave -noupdate -expand -group {Software Debugging} -divider {Instructions at ID stage, sampled half a cycle later}
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/riscv_tracer_i/insn_disas
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/riscv_tracer_i/insn_pc
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/riscv_tracer_i/insn_val
add wave -noupdate -expand -group {Software Debugging} -divider {Program counter at ID and IF stage}
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/pc_id
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/pc_if
add wave -noupdate -expand -group {Software Debugging} -divider {Register File contents}
add wave -noupdate -expand -group {Software Debugging} /tb_top/riscv_wrapper_i/riscv_core_i/id_stage_i/registers_i/riscv_register_file_i/mem
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {2022 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 250
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {19424 ps} {19454 ps}
