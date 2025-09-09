/* assembly for fir8*/
//load0: mem1
//load1: mem3
//load2: mem4
//store0: mem2
//load00: mem5
//load11: mem7
//load22: mem0
//store00: mem6

#include "riscv_test.h"
#include "test_macros.h"
#include "hybrid.h"

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi a0,zero,100;
	addi a7,zero,0;
	addi t1,zero,0;
	add	a1,zero,zero; 
	li	a2,mem_1;
	li	a3,mem_3;
	li	a4,mem_4;
	li	a5,mem_5;
	li	a6,mem_7;
store:	
	sw	t1,0(a1);
	addi t1,t1,1;
	sw	t1,0(a2);
	addi t1,t1,1;
	sw	t1,0(a3);
	addi t1,t1,1;
	sw	t1,0(a4);
	addi t1,t1,1;
	sw	t1,0(a5);
	addi t1,t1,1;
	sw	t1,0(a6);
	addi t1,t1,1;
	addi a1,a1,4;
	addi a2,a2,4;
	addi a3,a3,4;
	addi a4,a4,4;
	addi a5,a5,4;
	addi a6,a6,4;
	addi a7,a7,1;
	bne	a7,a0,store;
	li	a0,port0;
	addi a6,zero,103;
	addi t6,zero,1;
	sw	a6,0(a0);
fetch:  
	lw	a7,0(a0);
	bne	a7,t6,fetch;
/*	addi a6,zero,100;
	addi a7,zero,0;
  	li 	a0,mem_2;
	li 	a1,mem_6;
check:  
	lw	a1,16(a0);
	lw	a1,16(a1);
	addi a0,a0,4;
	addi a1,a1,4;
	addi a7,a7,1;
	bne	a7,a6,check;*/	
RVTEST_PASS
end:	RVTEST_CODE_END
