#include "memory.h"

static void mem_pool_init(uint32_t all_mem)
{
    put_str("   mem_pool_init start\n");

    uint32_t page_table_size = PG_SIZE * 256;   // 1个页目录表 + 0和768页目录项指向同一个页表，+ 769~1023=254个页表=256
    uint32_t used_mem = 0x100000 + page_table_size + ; // 0x100000, 1MB低端内存, 用于OS

    uint32_t free_mem = all_mem - used_mem;     // 32MB-2MB = 30MB

    uint16_t all_free_pages = free_mem / PG_SIZE;   //  30MB/4K = 7680
    uint16_t kernel_free_pages = all_free_pages / 2;    // 7680/2 = 3840
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;  // 7680-3840 = 3840


    uint32_t kbm_length = kernel_free_pages / 8; // kernel 内存池中位图长度,位图中每一位表示一个页框 3840/8 = 480
    uint32_t ubm_length = user_free_pages / 8;   // user 内存池中位图长度，位图中每一位表示一个页框  3840/8 = 480

    uint32_t kp_start = used_mem;       // kernel 内存池能用的内存起始地址
    uint32_t up_start = kp_start + kernel_free_pages * PG_SIZE; // user内存池 能用的内存起始地址


    kernel_pool.phy_addr_start = kp_start;  // 1MB OS, 1MB页目录表和页表
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;    // 15MB kernel 内存池
    kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;

    user_pool.phy_addr_start = up_start;
    user_pool.pool_size = user_free_pages * PG_SIZE;     // 15MB user 内存池
    user_pool.pool_bitmap.btmp_bytes_len = ubm_length;

    kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE; // MEM_BITMAP_BASE = 0xc009a000 存放内核内存池位图
    user_pool.pool_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length); // 存放用户内存池位图紧随其后

    /* 将位图置0 */
    bitmap_init(&(kernel_pool.pool_bitmap));
    bitmap_init(&(user_pool.pool_bitmap));


    /******************** 输出内存池信息 **********************/
    put_str("   kernel_pool_bitmap_start: ");   put_int((int)kernel_pool.pool_bitmap.bits); // 0xc009a000
    put_str(" kernel_pool_phy_addr_start: ");   put_int(kernel_pool.phy_addr_start);    // 0x00200000
    put_str("\n");
    put_str("   user_pool_bitmap_start: ");     put_int((int)user_pool.pool_bitmap.bits);   // 0xc009a1e0
    put_str(" user_pool_phy_addr_start: ");     put_int(user_pool.phy_addr_start);      // 0x01100000
    put_str("\n");



    /* 下面初始化内核虚拟地址的位图,按实际物理内存大小生成数组 */
    kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;      // 用于维护内核堆的虚拟地址,所以要和内核内存池大小一致

    /* 虚拟地址0xc0000000~0xc00fffff 映射 物理地址0x00000000~0x000fffff(1MB) */
    kernel_vaddr.vaddr_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length + ubm_length); // 0xc009a3c0
    bitmap_init(&kernel_vaddr.vaddr_bitmap);

    kernel_vaddr.vaddr_start = K_HEAP_START;    // 0xc0100000

    put_str("   mem_pool_init done\n");
}

/* 内存管理部分初始化入口 */
void mem_init()
{
    put_str(" mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t*)(0xb00));
    mem_pool_init(mem_bytes_total);	  // 初始化内存池     // 虚拟机物理内存总量32MB(0x02000000)
    put_str(" mem_init done\n");
}

