#include "cgra_complex.h"

// 定义指针类型，模拟 CGRA 的 IO 端口
// volatile 关键字很重要，防止编译器把读写操作优化掉
void cgra_fft_kernel(volatile int32_t* port_a, 
                     volatile int32_t* port_b, 
                     volatile int32_t* port_w, 
                     volatile int32_t* port_out_a, 
                     volatile int32_t* port_out_b, 
                     int n) {
    
    // 【关键步骤 1】定义用于平衡流水线的 volatile 常量
    // 使用 volatile 是为了告诉编译器：不要把这些看起来愚蠢的运算优化掉！
    volatile int32_t bal_one = 1;   //用于乘法平衡
    volatile int32_t bal_zero = 0;  //用于加法/移位平衡

    for (int i = 0; i < n; i++) {
		//DFGLoop: loop
        // --- Load 阶段 ---
        int32_t input_a = port_a[i]; 
        int32_t input_b = port_b[i];
        int32_t twiddle = port_w[i];

        int16_t ar = GET_REAL(input_a);
        int16_t ai = GET_IMAG(input_a);
        int16_t br = GET_REAL(input_b);
        int16_t bi = GET_IMAG(input_b);
        int16_t wr = GET_REAL(twiddle);
        int16_t wi = GET_IMAG(twiddle);

        // =========================================================
        // 下路 (Lower Arm): 真实的复数乘法与缩放
        // 硬件路径特征: Mul -> Add/Sub -> Ashr
        // =========================================================
        
        // 1. 乘法与合并 (Mul + Add/Sub)
        int32_t tr_raw = ((int32_t)wr * br - (int32_t)wi * bi);
        int32_t ti_raw = ((int32_t)wr * bi + (int32_t)wi * br);

        // 2. 移位缩放 (Ashr)
        int32_t tr = tr_raw >> 15;
        int32_t ti = ti_raw >> 15;


        // =========================================================
        // 上路 (Upper Arm): 伪造的平衡运算 (Dummy Operations)
        // 目标: 构造完全一样的路径 Mul -> Add/Sub -> Ashr
        // =========================================================

        // 1. 模拟乘法 (Mul): 乘以 1
        // 这会消耗一个乘法器 PE 的延迟
        int32_t ar_mul = ar * bal_one;
        int32_t ai_mul = ai * bal_one;

        // 2. 模拟中间加法 (Add): 加上 0
        // 下路在这里做了减法/加法合并，我们也加个 0 占位
        // 这会消耗一个 ALU PE 的延迟
        int32_t ar_add = ar_mul + bal_zero;
        int32_t ai_add = ai_mul + bal_zero;

        // 3. 模拟移位 (Ashr): 右移 0
        // 这会消耗一个 Shift PE 的延迟，最终与下路的 tr/ti 在时间上对齐
        int32_t ar_balanced = ar_add >> bal_zero;
        int32_t ai_balanced = ai_add >> bal_zero;


        // =========================================================
        // 最终蝶形加减 (Butterfly)
        // =========================================================
        
        // 注意：这里必须使用经过平衡处理的 ar_balanced 和 ai_balanced
        int16_t new_real_j = (ar_balanced + tr) >> 1;
        int16_t new_imag_j = (ai_balanced + ti) >> 1;
        int16_t new_real_k = (ar_balanced - tr) >> 1;
        int16_t new_imag_k = (ai_balanced - ti) >> 1;

        // --- Store 阶段 ---
        int32_t res_a = COMBINE(new_real_j, new_imag_j);
        int32_t res_b = COMBINE(new_real_k, new_imag_k);

        port_out_a[i] = res_a;
        port_out_b[i] = res_b;
    }
}