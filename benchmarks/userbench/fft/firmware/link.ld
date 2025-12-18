/*
This is free and unencumbered software released into the public domain.
*/

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(__mem_start)
MEMORY
{
	/* the memory in the testbench is 1024k in size;
	* set LENGTH=1008k and leave at least 16k for stack */
	 mem : ORIGIN = 0x00000000, LENGTH = 0x000fc000
}

SECTIONS
{
	.vectors (ORIGIN(mem)):
	{
		/* . = ALIGN(4); */
		__irq_base_vector = .;
		KEEP(*(.vectors));
	} > mem

	.start (ORIGIN(mem) + 0x180):
	{
		__boot_addr = .;
		KEEP(*(.start));
	} > mem

	/* temporary hack to redirect control flow when PULP_SECURE=0 */
	.legacy_irq (ORIGIN(mem) + 0x8000):
	{
		KEEP(*(.legacy_irq));
	} > mem


	.text.init :
	{
		*(.text.init);
	} > mem

	.text :
	{
		__mem_start = .;
		start*(.text);
		*(.text);
		__mem_end = .;
	} > mem
	_etext = .;

	/* [修改 1] 定义 .data 段 */
	.data :
	{
		. = ALIGN(4);
		_data_start = .;      /* 为 start.s 添加：RAM 中的数据起始地址 */
		__data_begin = .;     /* 兼容旧代码 */
    		*(.data .data.* .gnu.linkonce.d.*)
		. = ALIGN(4);
		_data_end = .;        /* 为 start.s 添加：RAM 中的数据结束地址 */
	} > mem

	/* [修改 2] 获取 .data 段在 ROM (Firmware) 中的加载地址 */
	_data_lma_start = LOADADDR(.data); 

	.data.string :
	{
		*(.data.string)
	} > mem

	.sdata :
	{
		__sdata_begin = .;
		*(.sdata .sdata.* .gnu.linkonce.s.*)
	} > mem
	_edata = .;

	. = ALIGN(4);
	__bss_start = .;
	.bss :
	{
		*(.bss)
	} > mem

	/* elf special section */
	.sbss :
	{
		*(.sbss)
	} > mem

	. = ALIGN(4);
	__bss_end = .;
	__global_pointer$ = MIN(__sdata_begin + 0x800,
			    MAX(__data_begin + 0x800, __bss_end - 0x800));
	_end = .;
}