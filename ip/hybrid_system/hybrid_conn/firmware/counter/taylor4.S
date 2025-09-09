// taylor expasion for z[i] = e^x from 0 to 60, 
// number increments internally, no input, store to mem_0 by CGRA

#include "riscv_test.h"
#include "test_macros.h"
#include "hybrid.h"

RVTEST_RV32U
RVTEST_CODE_BEGIN
	li	a0,counter;
	addi a2,zero,1;
	addi a3,zero,63;
	sw	a3,0(a0);
fetch:	
	lw 	a4,0(a0);
	bne	a4,a2,fetch
/*	addi a4,zero,0;
	li	a1,mem_0;
	addi a6,a1,20;
check: 	
	lw	a0,0(a6);
	addi a6,a6,4;
	addi a4,a4,1;
	bne	a5,a4,check;*/
RVTEST_PASS
end:	RVTEST_CODE_END