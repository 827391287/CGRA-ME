#!/bin/bash
riscv32-unknown-elf-gcc -c -march=rv32im -g -o firmware/app.o -DTEST_FUNC_NAME=app -DTEST_FUNC_TXT='"app"' -DTEST_FUNC_RET=app_ret firmware/app.S
riscv32-unknown-elf-gcc -c -march=rv32imc -g -o firmware/print.o firmware/print.c
riscv32-unknown-elf-gcc -c -march=rv32imc -g -o firmware/stats.o firmware/stats.c
riscv32-unknown-elf-gcc -c -march=rv32imc -g -o firmware/start.o firmware/start.S
riscv32-unknown-elf-gcc -g -Os -march=rv32imc -ffreestanding -nostdlib -o firmware/firmware.elf -Wl,-Bstatic,-T,firmware/link.ld,-Map,firmware/firmware.map,--strip-debug firmware/start.o firmware/print.o firmware/stats.o firmware/app.o -lgcc
riscv32-unknown-elf-objcopy -O verilog firmware/firmware.elf firmware/firmware.hex
#vsim -c -dpicpppath /usr/bin/gcc work.tb_top +firmware=firmware/firmware.hex
