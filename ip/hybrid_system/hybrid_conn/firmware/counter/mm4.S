/* assembly for matrix multiply 4_2*/
//port3: mem2/1
//port0: mem3/0
//port2: output
//matrixA: mem2 mem1
//matrixB: mem3 mem0 

#include "riscv_test.h"
#include "test_macros.h"
#include "hybrid.h"

.set write, 0x00080000 /*write address*/

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi a0,zero,400;
	addi a7,zero,0;
	addi a5,zero,0;
	addi a4,zero,0;
	li	a1,mem_1;
	li	a2,mem_2;
	li	a3,mem_3;
	sw	a7,0(a2);
	sw	a7,0(a1);
	sw	a7,0(a4);
	sw	a7,0(a3);
	addi a7,a7,1;
	addi a1,a1,4;
	addi a2,a2,4;
	addi a3,a3,4;
	addi a4,a4,4;
store:	
	sw	a7,0(a2);
	sw	a7,0(a3);
	addi a7,a7,1;
	sw	a7,0(a4);
	sw	a7,0(a1);
	addi a7,a7,1;
	addi a1,a1,4;
	addi a2,a2,4;
	addi a3,a3,4;
	addi a4,a4,4;
	addi a5,a5,1;
	bne	a5,a0,store;
	li	a0,io_top_2;
	li	a1,io_top_3;
	addi a4,zero,0;
	addi a5,zero,0;
	li	a6,couner;
	li	a7,reset;
	addi t0,zero,25;
	addi t1,zero,1;
	li	t3,io_top_0;
	li	t4,write
	addi t5,zero,0;
	addi t6,zero,0;
	addi s0,zero,20;
start0:	
	sw 	a4,0(a0);
start1:	
	sw	a5,0(a1);
	sw	t0,0(a6);
fetch: 
	lw	s1,0(a6);
	bne	s1,t1,fetch;
	lw	s2,0(t3);
	sw	s2,0(t4);
	addi t4,t4,4;
	addi a5,a5,80;
	addi t5,t5,1;
	sw	t1,0(a7);
	sw	zero,0(a7);
	bne	t5,s0,start1;
	addi a5,zero,0;
	addi t5,zero,0;
	addi a4,a4,80;
	addi t6,t6,1;
	bne	t6,s0,start0;
RVTEST_PASS

end:	RVTEST_CODE_END
