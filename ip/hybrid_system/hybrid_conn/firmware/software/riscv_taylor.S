/* assembly for taylor expansion*/
#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV32U
RVTEST_CODE_BEGIN
	addi	a0,zero,60;
	addi	a1,zero,64;
	addi	a2,zero,32;
	addi	a3,zero,12;
	addi	a5,zero, 0;
	addi	a6,zero, 0;
compute:
	mul	t0,a5,a5; /* x^2 */
	mul	t2,t0,a5; /* x^3 */
	mul	t0,t0,a2; /* 32x^2 */
	mul	t2,t2,a3; /* 12x^3 */
	mul	t3,a5,a1; /* 64x */
	add	t0,t0,a1; /* 64+32x^2 */
	add	t0,t0,t2; /* +12x^3 */
	add	t0,t0,t3; /* +64x */
	sw	t0,0(a6);
	addi a6,a6,4;
	addi a5,a5,1;
	bne	a0,a5, compute;
/*	addi a1,zero, 0;
	addi a2,zero, 0;
check: 	
	lw	a3,0(a1);
	addi	a1,a1,4;
	addi	a2,a2,1;
	bne	a0,a2,check;*/
RVTEST_PASS
end:	RVTEST_CODE_END