/* assembly for fir */
#include "riscv_test.h"
#include "test_macros.h"
.set port12, 0x00100000 

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi a0,zero,600;
	addi a7,zero,0;
	addi t1,zero,0;
	addi a1,zero,0; 
store:	
	sw	t1,0(a1);
	addi t1,t1,1;
	addi a1,a1,4;
	addi a7,a7,1;
	bne	a7,a0,store;
	addi	a0,zero,0;
	addi a1,zero,200;
	addi a2,zero,0;
	addi a3,zero,4;
	addi a4,zero,6;
	addi a5,zero,2;
	li 	t1,port12;
compute:
	lw	a6,0(a0);
	mul	a7,a6,a3;
	addi a0,a0,4;
	lw	a6,0(a0);
	mul	t0,a6,a4;
	add	a7,a7,t0;
	addi a0,a0,4;
	lw	a6,0(a0);
	mul	t0,a6,a5;
	add	a7,a7,t0;
	addi a0,a0,4;
	sw	a7,0(t1);
	addi t1,t1,4;
	addi a2,a2,1;
	bne	a2,a1,compute;
/*	addi a6,zero,200;
	addi a7,zero,0;
  	li	a0,port12;
check:  
	lw	a1,0(a0);
	addi a0,a0,4;
	addi a7,a7,1;
	bne	a7,a6,check;*/
RVTEST_PASS
end:	RVTEST_CODE_END
