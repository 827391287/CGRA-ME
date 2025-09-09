/* assembly for knn_riscy*/

#include "riscv_test.h"
#include "test_macros.h"

.set port13, 0x00080000

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi 	a0,zero,400;
	addi	a1,zero,0;
	addi	a7,zero,0;
store:	
	sw	a7,0(a1);
	addi a1,a1,4;
	addi a7,a7,1;
	bne	a7,a0,store;
	li	a0,port13;
	addi a1,zero,0;
	addi a6,zero,200;
	addi a7,zero,0;
	addi t1,zero,0;
	addi t2,zero,0;
	jal	start0;
	addi t1,zero,1;
	addi t2,zero,2;
	jal	start0;
	addi t1,zero,10;
	addi t2,zero,20;
	jal	start0;
	addi t1,zero,5;
	addi t2,zero,9;
	jal	start0;
	addi t1,zero,7;
	addi t2,zero,25;
	jal	start0;
	addi t1,zero,148;
	addi t2,zero,238;
	jal	start0;
	j	branch;
start0:	
	lw	a2,0(a1);
	addi a1,a1,4;
	lw	a3,0(a1);
	addi a1,a1,4;
	sub	a2,a2,t1;
	sub	a3,a3,t2;
	mul	a2,a2,a2;
	mul	a3,a3,a3;
	add	a2,a2,a3;
	sw	a2,0(a0);
	addi a0,a0,4;
	addi a7,a7,1;
	bne	a7,a6,start0;
	addi a7,zero,0;
	addi a1,zero,0;
	ret;
branch:	RVTEST_PASS
end:	RVTEST_CODE_END