/* assembly for fir4*/
//load0: mem2
//load1: mem3
//load2: mem1
//store: mem0
#include "riscv_test.h"
#include "test_macros.h"
#include "hybrid.h"

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi a0,zero,200;
	addi a7,zero,0;
	addi t1,zero,0;
	li	a1,mem_1; 
	li	a2,mem_2;
	li	a3,mem_3;
store:	
	sw	t1,0(a1);
	addi t1,t1,1;
	sw	t1,0(a2);
	addi t1,t1,1;
	sw	t1,0(a3);
	addi t1,t1,1;
	addi a1,a1,4;
	addi a2,a2,4;
	addi a3,a3,4;
	addi a7,a7,1;
	bne	a7,a0,store;
	li	a0,counter;
	addi a6,zero,203;
	addi t6,zero,1;
	sw	a6,0(a0);
fetch:  
	lw	a7,0(a0);
	bne	a7,t6,fetch;
/*	addi a6,zero,200;
	addi a7,zero,0;
  	add  a0,zero,zero;
check:  
	lw	a1,16(a0);
	addi a0,a0,4;
	addi a7,a7,1;
	bne	a7,a6,check;*/	
RVTEST_PASS
end:	RVTEST_CODE_END
