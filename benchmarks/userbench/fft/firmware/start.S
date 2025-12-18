// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

//#define ENABLE_STATS
#define ENABLE_RVTST

.set timer_irq_mask, 0x15000000
.set timer_irq_val, 0x15000004
.set print_port, 0x10000000
.set test_ret_val, 0x20000000

.section .vectors, "ax"
.option norvc
vector_table:
	j sw_irq_handler /* ecall, illegal insn and data prohibited */
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
vector_table_timer:
	j timer_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler
	j __no_irq_handler

/* this is fixed to 0x8000, used for PULP_SECURE=0. We redirect this entry to the
new vector table (which is at mtvec) */
.section .legacy_irq, "ax"
	j vector_table
	j __no_irq_handler
	j __no_irq_handler
	j vector_table_timer


.section .start, "ax"
	j start

.section .text
.global irq
.global stats
.global init_stats
.global print_dec
.global print_str


/* exception handling */
__no_irq_handler:
	la a0, no_exception_handler_msg
	jal ra, print_str
	j __no_irq_handler


sw_irq_handler:
	csrr t0, mcause
	slli t0, t0, 1  /* shift off the high bit */
	srli t0, t0, 1
	li t1, 2
	beq t0, t1, handle_illegal_insn
	li t1, 11
	beq t0, t1, handle_ecall
	li t1, 3
	beq t0, t1, handle_ebreak
	j handle_unknown

handle_ecall:
	/* la a0, ecall_msg */
	/* jal ra, print_str */ /* this is too verbose for now */
	j fail_test

handle_ebreak:
	la a0, ebreak_msg
	jal ra, print_str
	j fail_test

handle_illegal_insn:
	la a0, illegal_insn_msg
	jal ra, print_str
	j fail_test

handle_unknown:
	la a0, unknown_msg
	jal ra, print_str
	mv a0, t0
	jal ra, print_chr
	j fail_test

fail_test:
	li a0, 1
	sw a0, test_results, t1 /* signal failure */
	csrr a0, mepc
	addi a0, a0, 4
	csrw mepc, a0
	mret
	wfi

/* when we get a timeout we abort the whole testing procedure */
timer_irq_handler:
	la a0, timeout_msg
	jal ra, print_str
	li a0, 1
	sw a0, test_results, t1 /* signal failure */
	j fast_exit


/* Main program
 **********************************/

start:
	/* 1. 初始化全局指针 (Global Pointer) */
.option push
.option norelax
1:	auipc gp, %pcrel_hi(__global_pointer$)
	addi  gp, gp, %pcrel_lo(1b)
.option pop

	/* 2. [关键修改] 尽早初始化栈指针 (Stack Pointer) */
	/* 将栈顶设置在内存高位 (1MB处)，必须在调用任何 C 函数之前完成 */
	lui sp,(1024*1024)>>12 

    /* 3. [关键修改] 数据搬运循环 (Copy .data from ROM to RAM) */
    /* 依赖修改后的 link.ld 提供的符号 */
    la a0, _data_lma_start  /* 源地址：Firmware 中的数据位置 */
    la a1, _data_start      /* 目的地址：RAM 中的起始位置 */
    la a2, _data_end        /* 目的地址：RAM 中的结束位置 */
    
    /* 如果源地址和目的地址相同（极少见情况），则跳过 */
    beq a0, a1, copy_data_done

copy_data_loop:
    bge a1, a2, copy_data_done /* 如果当前地址 >= 结束地址，完成 */
    lw t0, 0(a0)               /* 从 ROM 读取一个字 */
    sw t0, 0(a1)               /* 写入 RAM 一个字 */
    addi a0, a0, 4             /* 源指针 +4 */
    addi a1, a1, 4             /* 目的指针 +4 */
    j copy_data_loop

copy_data_done:

	/* 4. 初始化性能计数器 (调用 C 函数) */
	jal ra, init_stats

	/* 5. 清零 BSS 段 (Init bss to zero) */
	li t0, 0
	la t1, __bss_start
	la t2, __bss_end

loop_bss:
	bge t1, t2, loop_bss_done
	sw t0, 0(t1)
	addi t1, t1, 4
	j loop_bss
loop_bss_done:

	/* 6. 清零所有通用寄存器 (避免 X 态传播) */
	addi x1, zero, 0
	addi x2, zero, 0
	/* addi x3, zero, 0 */ /* gp 已经初始化，不要清零 */
	addi x4, zero, 0
	addi x5, zero, 0
	addi x6, zero, 0
	addi x7, zero, 0
	addi x8, zero, 0
	addi x9, zero, 0
	addi x10, zero, 0
	addi x11, zero, 0
	addi x12, zero, 0
	addi x13, zero, 0
	addi x14, zero, 0
	addi x15, zero, 0
	addi x16, zero, 0
	addi x17, zero, 0
	addi x18, zero, 0
	addi x19, zero, 0
	addi x20, zero, 0
	addi x21, zero, 0
	addi x22, zero, 0
	addi x23, zero, 0
	addi x24, zero, 0
	addi x25, zero, 0
	addi x26, zero, 0
	addi x27, zero, 0
	addi x28, zero, 0
	addi x29, zero, 0
	addi x30, zero, 0
	addi x31, zero, 0

	/* 7. 开启机器级中断 */
	li      t0, 8
	csrrs   zero, mstatus, t0

	/* 8. 设置定时器中断 Mask */
	li a0, timer_irq_mask
	li a1, 0x8
	sw a1,0(a0)


#ifdef ENABLE_RVTST
#  define TEST(n) \
	.global n; \
	/* set timer value */;\
	li a0, timer_irq_val; \
	li a1, 100000; \
	sw a1, 0(a0); \
	/* return from test */;\
	jal zero,n; \
	.global n ## _ret; \
	n ## _ret:
#else
#  define TEST(n) \
	.global n ## _ret; \
	n ## _ret:
#endif

	/* 9. 跳转到主程序 app() */
	TEST(app)

	/* 栈指针初始化已移至开头，此处删除旧代码 */
	
	/* GP 初始化已移至开头，保留 TP 初始化 */
	lui gp, %hi(0xdeadbeef)
	addi gp, gp, %lo(0xdeadbeef)
	addi tp, gp, 0

#ifdef ENABLE_STATS
	/* call stats C code */
	jal ra,stats
#endif

#ifdef ENABLE_SIEVE
	/* call sieve C code */
	jal ra,sieve
#endif

#ifdef ENABLE_MULTST
	/* call multest C code */
	jal ra,multest
#endif
fast_exit:
	/* print "DONE\n" */
	lui a0,print_port>>12
	addi a1,zero,'D'
	addi a2,zero,'O'
	addi a3,zero,'N'
	addi a4,zero,'E'
	addi a5,zero,'\n'
	sw a1,0(a0)
	sw a2,0(a0)
	sw a3,0(a0)
	sw a4,0(a0)
	sw a5,0(a0)


	li a0, test_ret_val
	lw a1, test_results
	sw a1,0(a0)

	wfi  /* we are done */

.section .rodata
illegal_insn_msg:
	.string "illegal instruction exception handler entered\n"
ecall_msg:
	.string "ecall exception handler entered\n"
ebreak_msg:
	.string "ebreak exception handler entered\n"
unknown_msg:
	.string "unknown exception handler entered\n"
no_exception_handler_msg:
	.string "no exception handler installed\n"
riscv_tests_msg:
	.string "running riscv-tests\n"
riscv_compliance_tests_msg:
	.string "running riscv-compliance-tests\n"
timeout_msg:
	.string "\n\nTEST TIMEOUT, aborting...\n\n"

.section .data
.global test_results
test_results:
	.word 123456789