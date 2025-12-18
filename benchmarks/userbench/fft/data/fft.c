#include "cgra_complex.h"

// 定义指针类型，模拟 CGRA 的 IO 端口
// volatile 关键字很重要，防止编译器把读写操作优化掉
void cgra_fft_kernel(volatile int32_t* port_a, 
                     volatile int32_t* port_b, 
                     volatile int32_t* port_w, 
                     volatile int32_t* port_out_a, 
                     volatile int32_t* port_out_b, 
                     int n) { // n 是循环次数
    
    
    for (int i = 0; i < n; i++) {
		//DFGLoop: loop
        // --- 1. Load 阶段 (对应 DFG 的 Input Load 节点) ---
        // 这里模拟从端口读取数据流
        int32_t input_a = port_a[i]; 
        int32_t input_b = port_b[i];
        int32_t twiddle = port_w[i];

        // --- 2. 计算阶段 (使用你的自定义指令) ---
        int16_t ar = GET_REAL(input_a);
        int16_t ai = GET_IMAG(input_a);
        int16_t br = GET_REAL(input_b);
        int16_t bi = GET_IMAG(input_b);
        int16_t wr = GET_REAL(twiddle);
        int16_t wi = GET_IMAG(twiddle);

        // 核心蝶形运算
        int32_t tr = ((int32_t)wr * br - (int32_t)wi * bi) >> 15;
        int32_t ti = ((int32_t)wr * bi + (int32_t)wi * br) >> 15;

        int16_t new_real_j = (ar + tr) >> 1;
        int16_t new_imag_j = (ai + ti) >> 1;
        int16_t new_real_k = (ar - tr) >> 1;
        int16_t new_imag_k = (ai - ti) >> 1;

        // --- 3. Store 阶段 (对应 DFG 的 Output Store 节点) ---
        int32_t res_a = COMBINE(new_real_j, new_imag_j);
        int32_t res_b = COMBINE(new_real_k, new_imag_k);

        // 写回输出端口
        port_out_a[i] = res_a;
        port_out_b[i] = res_b;
    }
}