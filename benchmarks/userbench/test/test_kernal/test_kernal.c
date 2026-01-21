#include "cgra_complex.h"

void cgra_test_kernel(volatile int32_t* port_b,
                      volatile int32_t* port_out_b, 
                      int n) {
    


    for (int i = 0 ; i < n; i++) {
		//DFGLoop: loop
        // --- Load 阶段 ---
        int32_t input_b = port_b[i];

        int16_t br = GET_REAL(input_b);
        int16_t bi = GET_IMAG(input_b);

		int32_t cr = br + 1;
		int32_t ci = bi * 2;


        // --- Store 阶段 ---
        int32_t res_b = COMBINE(cr, ci);

        port_out_b[i] = res_b;
    }
}