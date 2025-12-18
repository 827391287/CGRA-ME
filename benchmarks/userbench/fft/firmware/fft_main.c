#include <stdint.h>
#include <stdlib.h>
#include "firmware.h" 

// =============================================================
//  Part 1: 物理内存 Bank 定义 (基于 hybrid.h)
//  Bank Size = 512KB (0x80000)
// =============================================================
// mem_1 = 524288 -> 0x80000
#define BANK1_BASE (volatile int*)(0x00080000) // 存 Input A (上路)
// mem_2 = 1048576 -> 0x100000
#define BANK2_BASE (volatile int*)(0x00100000) // 存 Input B (下路)
// mem_3 = 1572864 -> 0x180000
#define BANK3_BASE (volatile int*)(0x00180000) // 存 Weight (旋转因子)
// mem_4 = 2097152 -> 0x200000
#define BANK4_BASE (volatile int*)(0x00200000) // 存 Output A (上路结果)
// mem_5 = 2621440 -> 0x280000
#define BANK5_BASE (volatile int*)(0x00280000) // 存 Output B (下路结果)

// =============================================================
//  Part 2: 硬件 IO 端口地址 (基于 Mapping Log + hybrid.h)
//  Base IO Address = 0x400000 (4194304)
// =============================================================

// [Input A] -> input0 -> io_top_1
// 4194372 = 0x400044
#define PORT_IN_A_ADDR   (volatile int*)(0x400044)

// [Input B] -> input1 -> io_bottom_0
// 4194304 = 0x400000
#define PORT_IN_B_ADDR   (volatile int*)(0x400000)

// [Weight]  -> input2 -> io_top_3
// 4194380 = 0x400050
#define PORT_WEIGHT_ADDR (volatile int*)(0x400050)

// [Out A]   -> input3 -> io_bottom_3
// 4194316 = 0x40000C
#define PORT_OUT_A_ADDR  (volatile int*)(0x40000C)

// [Out B]   -> input4 -> io_bottom_1
// 4194308 = 0x400004
#define PORT_OUT_B_ADDR  (volatile int*)(0x400004)

// =============================================================
//  Part 3: 控制寄存器 (基于 hybrid.h 更新!)
//  注意：8x8 架构下，控制寄存器地址后移了
// =============================================================
// counter: 4194400 -> 0x400060
#define CGRA_LOOP_CNT_ADDR  (volatile int*)(0x400060)
// endport: 4194404 -> 0x400064
#define CGRA_DONE_ADDR      (volatile int*)(0x400064)
// reset:   4194408 -> 0x400068
#define CGRA_RESET_ADDR     (volatile int*)(0x400068)
// selport: 4194412 -> 0x40006C
#define CGRA_ENABLE_ADDR    (volatile int*)(0x40006C)

// =============================================================
//  Part 4: 辅助宏与全局变量
// =============================================================
#define MMIO_WRITE(addr, val) (*(addr) = (val))
#define MMIO_READ(addr)       (*(addr))

#define COMBINE(r, i) (((r) << 16) | ((i) & 0xFFFF))
#define GET_REAL(v) ((short)((v) >> 16))
#define GET_IMAG(v) ((short)((v) & 0xFFFF))

#define MAX_FFT_SIZE 512

// 原始数据和旋转因子表 (存放于 .data/.bss 段)
int data_out[MAX_FFT_SIZE];     
int twiddle_table[MAX_FFT_SIZE];
int N = 8; 

// =============================================================
//  Part 5: 初始化与主程序
// =============================================================

// 初始化数据
void init_data() {
    // 初始化全部为0
    for(int i=0; i<MAX_FFT_SIZE; i++) twiddle_table[i] = 0;

    twiddle_table[1] = COMBINE(32767, 0);       // W_2^0
    twiddle_table[2] = COMBINE(32767, 0);       // W_4^0
    twiddle_table[3] = COMBINE(0, -32767);      // W_4^1
    twiddle_table[4] = COMBINE(32767, 0);       // W_8^0
    twiddle_table[5] = COMBINE(23170, -23170);  // W_8^1
    twiddle_table[6] = COMBINE(0, -32767);      // W_8^2
    twiddle_table[7] = COMBINE(-23170, -23170); // W_8^3

    // 初始化输入数据
    for(int i=0; i<N; i++) {
        data_out[i] = COMBINE(i*100, 0); 
    }
}

int app() {
    init_data();
    print_str("Starting Hybrid FFT (II=1, 8x8 8-Bank Mode)...\n");

    // --- 1. 定义指向物理 Bank 的指针 ---
    // i3_load (Input A) -> mem_1 -> Bank 1
    volatile int *ptr_in_a = BANK1_BASE;
    
    // i5_load (Input B) -> mem_2 -> Bank 2
    volatile int *ptr_in_b = BANK2_BASE;

    // i7_load (Weight)  -> mem_3 -> Bank 3
    volatile int *ptr_w    = BANK3_BASE;

    // i43_store (Out A) -> mem_4 -> Bank 4
    volatile int *ptr_out_a = BANK4_BASE;

    // i45_store (Out B) -> mem_5 -> Bank 5
    volatile int *ptr_out_b = BANK5_BASE;


    // --- 2. 设置 CGRA 硬件参数 ---
    const int HW_LATENCY = 30; // 假设延迟，可根据仿真微调
    const int HW_II = 1;       // 映射结果确认 II=1

    int basedist = 1;

    // --- FFT 外层循环 (级数) ---
    for (int BlockSize = 2; BlockSize <= N; BlockSize <<= 1) {
        
        int batch_count = 0;
        
        // --- 3. Data Marshalling (数据分发到各 Bank) ---
        for (int i = 0; i < N; i += BlockSize) {
            for (int j = i, n = 0; n < basedist; j++, n++) {
                int k = j + basedist;
                int table_index = basedist + n;
                
                // Input A (上路) -> Bank 1
                ptr_in_a[batch_count] = data_out[j];
                
                // Input B (下路) -> Bank 2
                ptr_in_b[batch_count] = data_out[k];
                
                // Weight -> Bank 3
                ptr_w[batch_count] = twiddle_table[table_index];
                
                batch_count++;
            }
        }

        // --- 4. 配置 CGRA 端口 (写入 Bank 基地址) ---
        MMIO_WRITE(PORT_IN_A_ADDR,   (int)ptr_in_a);
        MMIO_WRITE(PORT_IN_B_ADDR,   (int)ptr_in_b);
        MMIO_WRITE(PORT_WEIGHT_ADDR, (int)ptr_w);
        MMIO_WRITE(PORT_OUT_A_ADDR,  (int)ptr_out_a);
        MMIO_WRITE(PORT_OUT_B_ADDR,  (int)ptr_out_b);

        // --- 5. 设置运行时间 ---
        int exec_cycles = HW_LATENCY + (batch_count - 1) * HW_II;
        MMIO_WRITE(CGRA_LOOP_CNT_ADDR, exec_cycles);

        // --- 6. 启动硬件 ---
        MMIO_WRITE(CGRA_ENABLE_ADDR, 1);
        
        // --- 7. 轮询等待结束 ---
        while (MMIO_READ(CGRA_DONE_ADDR) != 1);

        // --- 8. 停止与复位 ---
        MMIO_WRITE(CGRA_ENABLE_ADDR, 0);
        MMIO_WRITE(CGRA_RESET_ADDR, 1);
        volatile int dummy = 0; dummy++; 
        MMIO_WRITE(CGRA_RESET_ADDR, 0);


        // --- 9. Data Writeback (从 Output Banks 取回结果) ---
        batch_count = 0;
        for (int i = 0; i < N; i += BlockSize) {
            for (int j = i, n = 0; n < basedist; j++, n++) {
                int k = j + basedist;
                
                // Out A (上路) -> 来自 Bank 4
                data_out[j] = ptr_out_a[batch_count];
                
                // Out B (下路) -> 来自 Bank 5
                data_out[k] = ptr_out_b[batch_count];
                
                batch_count++;
            }
        }

        basedist <<= 1;
    }

    print_str("FFT Done.\n");
    // 验证打印
    for(int i=0; i<N; i++) {
        print_str("Out[");
        print_dec(i);
        print_str("]: ");
        print_dec(GET_REAL(data_out[i]));
        print_str(" + ");
        print_dec(GET_IMAG(data_out[i]));
        print_str("i\n");
    }
    
    return 0;
}