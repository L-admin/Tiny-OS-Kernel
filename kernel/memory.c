#include "memory.h"



static void mem_pool_init(uint32_t all_mem)
{
    put_str("   mem_pool_init start\n");

    uint32_t page_table_size = PG_SIZE * 256;   // 1个页目录表 + 0和768页目录项指向同一个页表，+ 769~1023=254个页表=256
    uint32_t used_mem = 0x100000 + page_table_size; // 0x100000, 1MB低端内存, 用于OS

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


/*
 * 在pf表示的内存池中申请pg_cnt个虚拟页
 * 成功则返回虚拟页的起始地址，失败则返回NULL
 */
static void* vaddr_get(enum pool_flags pf, uint32_t pg_cnt)
{
    int vaddr_start = 0;
    int bit_idx_start = -1;
    uint32_t cnt = 0;

    if (pf == PF_KERNEL)
    {
        bit_idx_start = bitmap_scan(&(kernel_vaddr.vaddr_bitmap), pg_cnt);  // bit_idx_start 虚拟页起始位

        if (bit_idx_start == -1)    // 没有足够的虚拟页
            return NULL;

        while(cnt < pg_cnt)
        {
            bitmap_set(&(kernel_vaddr.vaddr_bitmap), bit_idx_start + cnt, 1);
            cnt++;
        }

        vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PG_SIZE;
    }
    else if (pf == PF_USER)
    {
        // 用户内存池，待实现
    }

    return (void*)vaddr_start;
}

/* 得到虚拟地址vaddr对应的页表项pte()地址 */
uint32_t* pte_ptr(uint32_t vaddr)
{
    // 虚地址 0xffc00000 映射物理地址 0x00100000(页目录表地址)
    uint32_t* pte = (uint32_t*)(0xffc00000 +
            ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr)*4);
    return pte;
}

/* 得到虚拟地址vaddr对应的pde(页目录项)指针 */
uint32_t* pde_ptr(uint32_t vaddr)
{
    // 虚地址 0xfffff000 映射物理地址:0x00100000(页目录表起始地址)
    uint32_t* pde = (uint32_t*)((0xfffff000) + PDE_IDX(vaddr) * 4);
    return pde;
}

/*
 * 在m_pool指向的物理内存池中分配1个物理页
 * 成功则返回页框的物理地址，失败则返回NULL
 */
static void* palloc(struct pool* m_pool)
{
    /* 扫描或设置位图要保证原子操作 */
    int bit_idx = bitmap_scan(&(m_pool->pool_bitmap), 1);

    if (bit_idx == -1)
        return NULL;

    bitmap_set(&(m_pool->pool_bitmap), bit_idx, 1);
    uint32_t page_phyaddr = ( m_pool->phy_addr_start + (bit_idx * PG_SIZE));

    return (void*)page_phyaddr;
}

/*
 * 页表中添加虚拟地址_vaddr到物理地址_page_phyaddr的映射
 */
static void page_table_add(void* _vaddr, void* _page_phyaddr)
{
    uint32_t vaddr = (uint32_t)_vaddr;
    uint32_t page_phyaddr = (uint32_t)_page_phyaddr;

    uint32_t* pde = pde_ptr(vaddr);
    uint32_t* pte = pte_ptr(vaddr);

    if (*pde & 0x00000001)  // 页目录项存在
    {
        ASSERT(!(*pte & 0x00000001));   // 页表项存在报错

        if (!(*pte & 0x00000001))   // 页表项不存在
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    }
    else    // 页目录项不存在，先创建页目录项，再创建页表项,申请页框的内存
    {
        uint32_t pde_phyaddr = (uint32_t)palloc(&kernel_pool);  // 页表用到的页框一律从内核空间分配
        *pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        memset((void*)((int)pte & 0xfffff000), 0, PG_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}

/*
 * malloc_page 分配pg_cnt个页空间,成功则返回起始虚拟地址,失败时返回NULL
 * 1 通过vaddr_get在虚拟内存池中申请虚拟地址
 * 2 通过palloc在物理内存池中申请物理页
 * 3 通过page_table_add将以上得到的虚拟地址和物理地址在页表中完成映射
 */
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt)
{
    ASSERT(pg_cnt > 0 && pg_cnt < 3840);


    void* vaddr_start = vaddr_get(pf, pg_cnt);
    if (vaddr_start == NULL)
        return NULL;

    uint32_t vaddr = (uint32_t)vaddr_start, cnt = pg_cnt;
    struct pool* mem_pool = ((pf == PF_KERNEL) ? &kernel_pool : &user_pool);

    while(cnt-- > 0)
    {
        void* page_phyaddr = palloc(mem_pool);
        if (page_phyaddr == NULL)
            return NULL;
        page_table_add((void*)vaddr, page_phyaddr);
        vaddr += PG_SIZE;
    }
    return vaddr_start;
}

/*
 * 从内核内存池中申请 pg_cnt 页内存
 */
void* get_kernel_pages(uint32_t pg_cnt)
{
    void* vaddr =  malloc_page(PF_KERNEL, pg_cnt);

    if (vaddr != NULL)	   // 若分配的地址不为空,将页框清0后返回
        memset(vaddr, 0, pg_cnt * PG_SIZE);

    return vaddr;
}

/* 内存管理部分初始化入口 */
void mem_init()
{
    put_str("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t*)(0xb00));
    mem_pool_init(mem_bytes_total);	  // 初始化内存池     // 虚拟机物理内存总量32MB(0x02000000)
    put_str("mem_init done\n");
}

