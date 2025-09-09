#include "riscv_test.h"
#include "test_macros.h"

.set port10, 0x00000000 /* read_begin_addr for matrixA */
.set port11, 0x00000000 /* read_begin_addr for matrixB*/
.set port12, 0x00000000 
.set port13, 0x00080000
.set port14, 0x00100000
.set port15, 0x00280000
.set port16, 0x00300000
.set port17, 0x00380000
.set port18, 0x00200000 /*write address*/

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi a0,zero,801;
	addi a7,zero,0;
	li	a1,port12;
	li	a2,port13;
store:	
	sw	a7,0(a1);
	sw	a7,0(a2);
	addi a1,a1,4;
	addi a2,a2,4;
	addi a7,a7,1;
	bne	a7,a0,store;
	addi t0,zero,20;
	addi s0,zero,40;
	addi a0,zero,0;
	addi a1,zero,0;
	addi a6,zero,0;
	addi t4,zero,0;
	li	a2,port12;
	li	a3,port13;
	li 	t5,port18;
	addi t2,a3,4;
	addi t1,a2,4;
	addi t3,zero,4;
start1:	
	lw	a4,0(t1);
	lw	a5,0(t2);
	mul	a7,a4,a5;
	add	a6,a6,a7;
	addi a0,a0,1;
	addi t1,t1,4;
	addi t2,t2,4;
	bne	a0,s0,start1;
	sw	a6,0(t5);
	addi a6,zero,0;
	addi t5,t5,4;
	addi a0,zero,0;
	add	t1,a2,t3;
	addi a1,a1,1;
	bne	a1,t0,start1;
	addi a1,zero,0;
	addi t3,t3,160;
	addi t2,a3,4;
	add	t1,a2,t3;
	addi t4,t4,1;
	bne	t4,t0,start1;
RVTEST_PASS
end:	RVTEST_CODE_END
