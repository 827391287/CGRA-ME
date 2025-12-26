/*
 * 适配 RAM 从 0 地址开始的哈佛架构 / 双存储器模型
 */

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(__mem_start)

MEMORY
{
    /* 
     * INSTR_RAM (ROM行为): 存放代码、只读数据、数据段的【加载值 LMA】
     * 地址从 0x00000000 开始
     */
    INSTR_RAM (rx) : ORIGIN = 0x00000000, LENGTH = 512K

    /* 
     * DATA_RAM (RAM行为): 存放数据段的【运行时值 VMA】、BSS、堆栈
     * 地址也从 0x00000000 开始 (因为是独立的数据总线/存储器)
     */
    DATA_RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 512K
}

SECTIONS
{
    /* =================================================================
       1. 指令区 (映射到 INSTR_RAM)
       ================================================================= */
    
    /* 中断向量表 */
    .vectors :
    {
        __irq_base_vector = .;
        KEEP(*(.vectors));
    } > INSTR_RAM

    /* 启动代码 (通常在 0x80 或 0x180) */
    .start :
    {
        . = ALIGN(0x180); 
        __boot_addr = .;
        KEEP(*(.start));
    } > INSTR_RAM

    /* 代码段 */
    .text :
    {
        __mem_start = .;
        *(.text.init)
        *(.text)
        *(.text.*)
        _etext = .;
    } > INSTR_RAM

    /* 只读数据 (字符串等) */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
        
        /* [关键修复] 强制 .rodata 结束时对齐到 4 字节 */
        /* 这解决了 0x8cd3 这种奇数地址导致 lw 失败的问题 */
        . = ALIGN(4); 
    } > INSTR_RAM

    /* =================================================================
       2. 数据区 (VMA 映射到 DATA_RAM, LMA 存放在 INSTR_RAM)
       ================================================================= */

    /* 
     * .data 段定义：
     * > DATA_RAM      : 表示运行时，变量地址在 Data RAM (从 0 开始)
     * AT > INSTR_RAM  : 表示烧录时，初始值紧挨着代码段存在 Instr RAM 中
     */
    .data : 
    {
        . = ALIGN(4);
        _data_start = .;        /* 目的地址 (a1): RAM 中的起始 */
        __data_begin = .;
        
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        
        . = ALIGN(4);
        _data_end = .;          /* 目的地址 (a2): RAM 中的结束 */
    } > DATA_RAM AT > INSTR_RAM

    /* 
     * 计算源地址 (a0): 获取 .data 段在 INSTR_RAM 中的加载位置 
     * 这通常是 .rodata 结束后的下一个位置
     */
    _data_lma_start = LOADADDR(.data);

    /* =================================================================
       3. BSS 区 (只在 DATA_RAM 占位，不需要搬运)
       ================================================================= */
    .bss :
    {
        . = ALIGN(4);
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > DATA_RAM

    /* 全局指针 GP 优化 */
    __global_pointer$ = MIN(__data_begin + 0x800, MAX(__data_begin + 0x800, __bss_end - 0x800));

    /* 堆栈指针: 设置在 DATA_RAM 的末尾 (0 + 512KB) */
    _stack_top = ORIGIN(DATA_RAM) + LENGTH(DATA_RAM);
    _end = .;
}