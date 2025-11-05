#include <stdio.h>
#include <math.h>
#include "cgra_complex.h"

volatile int* n = (int*)0;
static int* data_in = (int*)0xa00;
static int* data_out = (int*)0xc00;
static int* twiddle_table = (int*)0xe00;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_FFT_SIZE 512
#define FIXED_POINT_SCALE 32767


int main() {
    int N = *n;
    
    // 预计算旋转因子（Q15有符号）
    for (int m = 2; m <= N; m <<= 1) {
        for (int k = 0; k < m/2; k++) {
            double angle = -2.0 * M_PI * k / m;
            int index = (m/2) + k;
            if (index < MAX_FFT_SIZE/2) {
                short cos_val = (short)(cos(angle) * FIXED_POINT_SCALE);
                short sin_val = (short)(sin(angle) * FIXED_POINT_SCALE);
                twiddle_table[index] = COMBINE(cos_val,sin_val);
            }
        }
    }
    
    // 数据拷贝和位反转
    int NumBits = 0;
    int temp = N;
    while (temp >>= 1) NumBits++;
    
    for (int i = 0; i < N; i++) {
        int j = 0;
        int k = i;
        for (int bit = 0; bit < NumBits; bit++) {
            j = (j << 1) | (k & 1);
            k >>= 1;
        }
        data_out[j] = data_in[i];
    }

    // FFT蝶形运算
    int basedist = 1;

    for (int BlockSize = 2; BlockSize <= N; BlockSize <<= 1) {
        for (int i = 0; i < N; i += BlockSize) {
            for (int j = i, n = 0; n < basedist; j++, n++) { 
				//DFGLoop: loop 
                // 从预计算表中获取旋转因子
                int table_index = basedist + n;
                int twiddle = twiddle_table[table_index];
                // 提取旋转因子
                short ar0 = GET_REAL(twiddle);  // cos
                short ai0 = GET_IMAG(twiddle);   // sin
                int k = j + basedist;             
                // 从合并数据中提取实部和虚部 
                int data_j = data_out[j];
                int data_k = data_out[k];        
                short real_j = GET_REAL(data_j);  // 实部
                short imag_j = GET_IMAG(data_j);   // 虚部
                short real_k = GET_REAL(data_k);  // 实部
                short imag_k = GET_IMAG(data_k);   // 虚部    
                // 蝶形运算 - Q15乘法处理
                int tr = ((int)ar0 * real_k - (int)ai0 * imag_k) >> 15;
                int ti = ((int)ar0 * imag_k + (int)ai0 * real_k) >> 15;
                // 计算新的实部和虚部
                short new_real_j = real_j + tr;
                short new_imag_j = imag_j + ti;
                short new_real_k = real_j - tr;
                short new_imag_k = imag_j - ti; 
                // 合并存储
                data_out[j] = COMBINE(new_real_j,new_imag_j);
                data_out[k] = COMBINE(new_real_k,new_imag_k);
            }
        }
        basedist <<= 1;
    }

    printf("FFT with unified macros completed for N=%d\n", N);
    return 0;
}


