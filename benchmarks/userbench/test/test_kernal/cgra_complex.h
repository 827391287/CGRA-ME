// cgra_complex.h
#ifndef CGRA_COMPLEX_H
#define CGRA_COMPLEX_H

#include <stdint.h>


// 确保这个函数在LLVM IR中是一个可被调用的函数，而不是被内联展开
__attribute__((noinline))
int16_t GET_REAL(int32_t data) {
    return data >> 16;
}

__attribute__((noinline))
int16_t GET_IMAG(int32_t data) {
    return (int16_t)((data << 16) >> 16);
}

__attribute__((noinline))
int32_t COMBINE(int16_t real, int16_t imag) {
    return ((int32_t)real << 16) | (imag & 0xFFFF);
}





// // FFT专用的复数加法
// static inline int32_t fft_complex_add(int32_t a, int32_t b) {
//     return fft_combine(
//         fft_get_real(a) + fft_get_real(b),
//         fft_get_imag(a) + fft_get_imag(b)
//     );
// }

// // FFT专用的复数乘法
// static inline int32_t fft_complex_multiply(int32_t a, int32_t b) {
//     int16_t a_real = fft_get_real(a);
//     int16_t a_imag = fft_get_imag(a);
//     int16_t b_real = fft_get_real(b);
//     int16_t b_imag = fft_get_imag(b);
    
//     // (a+bi)*(c+di) = (ac-bd) + (ad+bc)i
//     int16_t real_part = a_real * b_real - a_imag * b_imag;
//     int16_t imag_part = a_real * b_imag + a_imag * b_real;
    
//     return fft_combine(real_part, imag_part);
// }

#endif