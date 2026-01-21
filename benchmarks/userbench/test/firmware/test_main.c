#include <stdint.h>
#include <stdlib.h>
#include "firmware.h" 

// =============================================================
//  Part 1: 物理内存 Bank 定义 (CPU 视角 - 全局地址)
// =============================================================
// 我们只用到 Input B 和 Output B
#define BANK1_BASE (volatile int*)(0x00100000) // Input B
#define BANK2_BASE (volatile int*)(0x00200000) // Output B

// =============================================================
//  Part 2: 硬件 IO 端口地址 (CGRA 视角 - 物理端口)
//  根据你的 DFG 映射结果 (io_bottom_1 和 io_bottom_0)
// =============================================================

// [Input B] -> Load Address -> io_bottom_1
#define PORT_IN_B_ADDR   (volatile int*)(0x400004)

// [Output B] -> Store Address -> io_bottom_0
// 注意：参考代码里是 0x400008，但根据你的 DFG 分析，Store 用的是 io_bottom_0 (0x400000)
#define PORT_OUT_B_ADDR  (volatile int*)(0x400000)

// =============================================================
//  Part 3: 控制寄存器
//  (保持与你参考代码一致的偏移量，如果你的硬件变了请修改这里)
//  注意：之前讨论中使用的是 0x400030 系列，参考代码是 0x400060 系列。
//  这里我采用了你参考代码中的 0x400060 系列，如果不通请改回 0x30。
// =============================================================
#define CGRA_LOOP_CNT_ADDR  (volatile int*)(0x400030)
#define CGRA_DONE_ADDR      (volatile int*)(0x400034)
#define CGRA_RESET_ADDR     (volatile int*)(0x400038)
#define CGRA_ENABLE_ADDR    (volatile int*)(0x40003C)

// =============================================================
//  Part 4: 辅助宏
// =============================================================
#define MMIO_WRITE(addr, val) (*(addr) = (val))
#define MMIO_READ(addr)       (*(addr))

#define COMBINE(r, i) (((r) << 16) | ((i) & 0xFFFF))
#define GET_REAL(v) ((short)((v) >> 16))
#define GET_IMAG(v) ((short)((v) & 0xFFFF))



// =============================================================
//  Part 5: 初始化数据
// =============================================================
#define MAX_SIZE 512
int data_out[MAX_SIZE];     
int N = 32; 

void init_data() {


    for(int i=0; i<N; i++) {
        data_out[i] = COMBINE((i + 1)*100, i + 1); 
    }
}


// =============================================================
//  Part 6: 主程序
// =============================================================

int app() {
    init_data();
    volatile int *ptr_cpu_in_b  = BANK1_BASE;
    volatile int *ptr_cpu_out_b = BANK2_BASE;

    // 硬件参数
    // 这里的 LATENCY 是你需要在映射算法中自动解决的，C代码不做补偿
    const int HW_LATENCY = 30; 
    const int HW_II = 1;       
    
    // 测试参数：4个批次，每批次8个数据
    const int NUM_BATCHES = 4; 
    const int BATCH_SIZE = 8;  

    int total_errors = 0;

    // --- 外层循环：模拟多次任务提交 (Batch Processing) ---
    for (int i = 0; i < N; i += BATCH_SIZE) {
		
        for (int j = i , n = 0; n < BATCH_SIZE; j ++ , n ++) {             
            ptr_cpu_in_b[n] = data_out[j];            
        }
        

        // 尾部填充 (Tail Padding) - 防止流水线读脏数据
        for(int k=0; k<4; k++) { 
            ptr_cpu_in_b[BATCH_SIZE + k] = 0;
        }


        // -----------------------------------------------------
        // 2. 配置 CGRA (验证映射算法是否解决了时序问题)
        // -----------------------------------------------------
        
        // Input 配置：相对地址 0
        MMIO_WRITE(PORT_IN_B_ADDR, 0);

        MMIO_WRITE(PORT_OUT_B_ADDR, 0);

        // -----------------------------------------------------
        // 3. 复位硬件 (清除流水线残留)
        // -----------------------------------------------------
        MMIO_WRITE(CGRA_RESET_ADDR, 1);
        __asm__("nop"); __asm__("nop");
        MMIO_WRITE(CGRA_RESET_ADDR, 0);
        __asm__("nop"); __asm__("nop");

        // -----------------------------------------------------
        // 4. 启动执行
        // -----------------------------------------------------
        int exec_cycles = HW_LATENCY + (BATCH_SIZE - 1) * HW_II + 30;
        MMIO_WRITE(CGRA_LOOP_CNT_ADDR, exec_cycles);

        // 等待完成 (Polling)
        __asm__ volatile (
            "li t6, 500\n"      
            "1:\n"               
            "addi t6, t6, -1\n"  
            "bnez t6, 1b\n"      
            ::: "t6"
        );
    }
    
    return 0;
}