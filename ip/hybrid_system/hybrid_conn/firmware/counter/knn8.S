/* assembly for knn*/
//load0: mem1
//load1: mem7
//store0: mem5
//store1: mem4
//input0: port1/6
//input1: port4
//input00: port7
//input11: port5

#include "riscv_test.h"
#include "test_macros.h"
#include "hybrid.h"

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi a0,zero,200;
	addi a7,zero,0;
	addi t1,zero,0;
	li	a1,mem_1; 
	li	a2,mem_6;
store:	
	sw	t1,0(a1);
	addi t1,t1,1;
	sw	t1,0(a2);
	addi t1,t1,1;
	addi a1,a1,4;
	addi a2,a2,4;
	addi a7,a7,1;
	bne	a7,a0,store;
	li	a0,counter;
	li	a2,io_top_3;
	li	a3,io_top_6;
	li	a4,io_top_0;
	li	a5,io_top_7;
	addi	a6,zero,203;
	addi	t6,zero,1;
	addi	t1,zero,0;
	addi	t2,zero,0;
	addi	t3,zero,1;
	addi	t4,zero,2;
	jal	start0;
	addi	t1,zero,10;
	addi	t2,zero,20;
	addi	t3,zero,5;
	addi	t4,zero,9;
	jal	start0;
	addi	t1,zero,7;
	addi	t2,zero,25;
	addi	t3,zero,148;
	addi	t4,zero,238;
	jal	start0;
	j	branch;
start0:	
	sw	t1,0(a2);
	sw	t2,0(a3);
	sw	t3,0(a4);
	sw	t4,0(a5);
	sw	a6,0(a0);
fetch:  
	lw	a7,0(a0);
	bne	a7,t6,fetch;
	sw	t6,0(a1);
	sw	zero,0(a1);
	ret;
branch:	RVTEST_PASS
end:	RVTEST_CODE_END
