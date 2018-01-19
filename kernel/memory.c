#include "memory.h"

static void mem_pool_init(uint32_t all_mem)
{
    put_str("   mem_pool_init start\n");

    uint32_t page_table_size = PG_SIZE * 256;   // 1个页目录表 + 0和768页目录项指向同一个页表，+ 769~1023=254个页表=256
    uint32_t used_mem = page_table_size + 0x100000; // 1MB低端内存, 用于kernel

    uint32_t free_mem = all_mem - used_mem;

    uint16_t all_free_pages = free_mem / PG_SIZE;
    uint16_t kernel_free_pages = all_free_pages / 2;
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;


    uint32_t kbm_length = kernel_free_pages / 8; // kernel 内存池中位图长度,位图中每一位表示一个页框
    uint32_t ubm_length = user_free_pages / 8;   // user 内存池中位图长度，位图中每一位表示一个页框

    uint32_t kp_start = used_mem;       // kernel能用的内存起始地址
    uint32_t up_start = kp_start + kernel_free_pages * PG_SIZE; // 用户能用的内存起始地址

    kernel_pool.phy_addr_start = kp_start;
    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;

    user_pool.phy_addr_start = up_start;
    user_pool.pool_size = user_free_pages * PG_SIZE;
    user_pool.pool_bitmap.btmp_bytes_len = ubm_length;


    kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE; // MEM_BITMAP_BASE = 0xc009a00 内核内存位图
    user_pool.pool_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length); // 用户内存位图紧随其后

    /******************** 输出内存池信息 **********************/
    put_str("      kernel_pool_bitmap_start:");put_int((int)kernel_pool.pool_bitmap.bits);
    put_str(" kernel_pool_phy_addr_start:");put_int(kernel_pool.phy_addr_start);
    put_str("\n");
    put_str("      user_pool_bitmap_start:");put_int((int)user_pool.pool_bitmap.bits);
    put_str(" user_pool_phy_addr_start:");put_int(user_pool.phy_addr_start);
    put_str("\n");

    /* 将位图置0 */
    bitmap_init(&(kernel_pool.pool_bitmap));
    bitmap_init(&(user_pool.pool_bitmap));

    /* 下面初始化内核虚拟地址的位图,按实际物理内存大小生成数组。*/
    kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;      // 用于维护内核堆的虚拟地址,所以要和内核内存池大小一致

    /* 位图的数组指向一块未使用的内存,目前定位在内核内存池和用户内存池之外 */
    kernel_vaddr.vaddr_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length + ubm_length);

    kernel_vaddr.vaddr_start = K_HEAP_START;
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("   mem_pool_init done\n");
}

/* 内存管理部分初始化入口 */
void mem_init()
{
    put_str("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t*)(0xb03));
    mem_pool_init(mem_bytes_total);	  // 初始化内存池
    put_str("mem_init done\n");
}

