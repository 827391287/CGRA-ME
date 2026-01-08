#include <stdint.h>
#include <stdlib.h>
#include "firmware.h" 

// =============================================================
//  Part 1: 物理内存 Bank 定义 (CPU 视角 - 全局地址)
//  用于 RISC-V 进行数据搬运 (Data Marshalling)
// =============================================================
#define BANK1_BASE (volatile int*)(0x00080000) // Input A
#define BANK2_BASE (volatile int*)(0x00100000) // Input B
#define BANK3_BASE (volatile int*)(0x00180000) // Weight
#define BANK4_BASE (volatile int*)(0x00200000) // Output A
#define BANK5_BASE (volatile int*)(0x00280000) // Output B

// =============================================================
//  Part 2: 硬件 IO 端口地址 (CGRA 视角 - 物理端口)
//  已根据 Mapping Result 和 hybrid.h (.set) 修正
// =============================================================

// [Input A] -> input0 -> io_top_3 (ASM: 4194380)
#define PORT_IN_A_ADDR   (volatile int*)(0x40004C)

// [Input B] -> input1 -> io_bottom_1 (ASM: 4194308)
#define PORT_IN_B_ADDR   (volatile int*)(0x400004)

// [Weight]  -> input2 -> io_bottom_3 (ASM: 4194316)
#define PORT_WEIGHT_ADDR (volatile int*)(0x40000C)

// [Out A]   -> input3 -> io_top_1 (ASM: 4194372)
#define PORT_OUT_A_ADDR  (volatile int*)(0x400044)

// [Out B]   -> input4 -> io_bottom_2 (ASM: 4194312)
#define PORT_OUT_B_ADDR  (volatile int*)(0x400008)

// =============================================================
//  Part 3: 控制寄存器
// =============================================================
#define CGRA_LOOP_CNT_ADDR  (volatile int*)(0x400060)
#define CGRA_DONE_ADDR      (volatile int*)(0x400064)
#define CGRA_RESET_ADDR     (volatile int*)(0x400068)
#define CGRA_ENABLE_ADDR    (volatile int*)(0x40006C)

// =============================================================
//  Part 4: 辅助宏与时序补偿参数
// =============================================================
#define MMIO_WRITE(addr, val) (*(addr) = (val))
#define MMIO_READ(addr)       (*(addr))

#define COMBINE(r, i) (((r) << 16) | ((i) & 0xFFFF))
#define GET_REAL(v) ((short)((v) >> 16))
#define GET_IMAG(v) ((short)((v) & 0xFFFF))

#define MAX_FFT_SIZE 512

// 【关键修改】时序补偿偏移量 (Latency Offsets)
// 用于解决 Store 地址超前数据到达的问题
// OUT_A (Bank4) 观测到超前 0x30 (52字节)
// OUT_B (Bank5) 观测到超前 0x1C (28字节)
#define LATENCY_OFFSET_A 0x30
#define LATENCY_OFFSET_B 0x1C
// 1. 定义重复带来的额外偏移 (3个副本)
// 为什么是 12? 因为我们要把前 3 个数(占据 0,4,8) 移到负数区(-12, -8, -4)
// 这样第 4 个数(占据 12) 就会变成 12 - 12 = 0 
#define REPEAT_OFFSET 12 

int data_out[MAX_FFT_SIZE];     
int twiddle_table[MAX_FFT_SIZE];
int N = 8; 

// =============================================================
//  Part 5: 主程序
// =============================================================

void init_data() {

    for(int i=0; i<MAX_FFT_SIZE; i++) twiddle_table[i] = 0;
    twiddle_table[1] = COMBINE(32767, 0);       
    twiddle_table[2] = COMBINE(32767, 0);       
    twiddle_table[3] = COMBINE(0, -32767);      
    twiddle_table[4] = COMBINE(32767, 0);       
    twiddle_table[5] = COMBINE(23170, -23170);  
    twiddle_table[6] = COMBINE(0, -32767);      
    twiddle_table[7] = COMBINE(-23170, -23170); 

    for(int i=0; i<N; i++) {
        data_out[i] = COMBINE(i*100, 0); 
    }
}

int app() {
    init_data();
    print_str("Starting Hybrid FFT (Fixed IO & Latency)...\n");

    // 定义 CPU 视角的指针 (用于数据搬运)
    volatile int *ptr_cpu_in_a = BANK1_BASE;
    volatile int *ptr_cpu_in_b = BANK2_BASE;
    volatile int *ptr_cpu_w    = BANK3_BASE;
    volatile int *ptr_cpu_out_a = BANK4_BASE;
    volatile int *ptr_cpu_out_b = BANK5_BASE;

    const int HW_LATENCY = 30;
    const int HW_II = 1;
    int basedist = 1;

    // --- FFT 外层循环 ---
    for (int BlockSize = 2; BlockSize <= N; BlockSize <<= 1) {
        
        int batch_count = 0;
        
        // --- 1. Data Marshalling (CPU 搬运数据) ---
        // 使用全局地址 (BANKx_BASE)
        for (int i = 0; i < N; i += BlockSize) {
            for (int j = i, n = 0; n < basedist; j++, n++) {
                int k = j + basedist;
                int table_index = basedist + n;
                
                ptr_cpu_in_a[batch_count] = data_out[j];
                ptr_cpu_in_b[batch_count] = data_out[k];
                ptr_cpu_w[batch_count]    = twiddle_table[table_index];
                
                batch_count++;
            }
        }
		// =========================================================
        // 【新增修复】尾部填充 (Tail Padding)
        // 在有效数据之后填充 0，防止流水线多读时读到 X 导致结果污染
        // =========================================================
        int tail_padding = 4; // 多清零4个位置，覆盖流水线尾部
        for(int k=0; k<tail_padding; k++) {
            ptr_cpu_in_a[batch_count + k] = 0;
            ptr_cpu_in_b[batch_count + k] = 0;
            ptr_cpu_w[batch_count + k]    = 0; // 权重清零最重要，X乘任何数都是X
        }

        // --- 2. 配置 CGRA (关键修改区) ---

        
        // Input 配置：使用本地相对地址 0
        // 告诉 CGRA 从 Bank 的第 0 个字开始读。
        // 这解决了 "0x80004" 越界导致的取数停止问题。
        MMIO_WRITE(PORT_IN_A_ADDR,   0);
        MMIO_WRITE(PORT_IN_B_ADDR,   0);
        MMIO_WRITE(PORT_WEIGHT_ADDR, 0);

        // Output 配置：使用负偏移量进行时序补偿
        // 告诉 CGRA 基地址是负数，这样当地址计数器跑到 Latency 时，
        // 实际物理地址刚好回到 0。
        // Bank 4: 0 - 0x30
        MMIO_WRITE(PORT_OUT_A_ADDR,  (int)(0 - LATENCY_OFFSET_A - REPEAT_OFFSET));
        // Bank 5: 0 - 0x1C
        MMIO_WRITE(PORT_OUT_B_ADDR,  (int)(0 - LATENCY_OFFSET_B - REPEAT_OFFSET));


        // --- 3. 彻底复位硬件 ---
        MMIO_WRITE(CGRA_RESET_ADDR, 1);
        // 延时确保复位生效
		__asm__("nop");
        __asm__("nop");
        MMIO_WRITE(CGRA_RESET_ADDR, 0);
        // 延时确保信号稳定
		__asm__("nop");
        __asm__("nop");

        // --- 4. 设置运行时间 ---
        // 额外增加 50 个周期，确保最后的数据有足够时间写入
        int exec_cycles = HW_LATENCY + (batch_count - 1) * HW_II + 50;
        MMIO_WRITE(CGRA_LOOP_CNT_ADDR, exec_cycles);

       
        // --- 5. 等待结束 ---
        __asm__ volatile (
            "li t6, 200\n"      
            "1:\n"               
            "addi t6, t6, -1\n"  
            "bnez t6, 1b\n"      
            ::: "t6"
		);


        // --- 6. Data Writeback (CPU 取回结果) ---
        // 使用全局地址读取 Bank4/5
        batch_count = 0;
        for (int i = 0; i < N; i += BlockSize) {
            for (int j = i, n = 0; n < basedist; j++, n++) {
                int k = j + basedist;
                
                data_out[j] = ptr_cpu_out_a[batch_count];
                data_out[k] = ptr_cpu_out_b[batch_count];
                
                batch_count++;
            }
        }

        basedist <<= 1;
    }

    print_str("FFT Done.\n");
    // 验证打印
    for(int i=0; i<N; i++) {
        print_str("Out["); print_dec(i); print_str("]: ");
        print_dec(GET_REAL(data_out[i]));
        print_str(" + ");
        print_dec(GET_IMAG(data_out[i]));
        print_str("i\n");
    }
    
    return 0;
}