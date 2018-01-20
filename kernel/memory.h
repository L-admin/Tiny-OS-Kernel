#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H

#include "../lib/stdint.h"
#include "../lib/kernel/bitmap.h"
#include "../lib/kernel/print.h"
#include "global.h"


#define PG_SIZE 4096

#define MEM_BITMAP_BASE 0xc009a000  // 0xc009e000是内核进程PCB，打算用4页存放位图(可管理512MB内存)，-0x400 = 0xc009a000

#define K_HEAP_START 0xc0100000     // 内核所使用堆空间的起始虚拟地址, 虚拟地址0xc0000000 ~ 0xc00fffff 映射
                                    // 物理地址0x00000000~0x000fffff, 为了让虚拟地址连续，所以将其定义为0xc0100000

struct virtual_addr
{
    struct bitmap vaddr_bitmap;     // 虚拟地址用到的位图结构
    uint32_t vaddr_start;           // 虚拟地址起始地址
};

struct virtual_addr kernel_vaddr;   // 用来给内核分配虚拟地址


struct pool
{
    struct bitmap pool_bitmap;  // 本内存池用到的位图结构 用于管理物理内存
    uint32_t phy_addr_start;    // 本内存池所管理物理内存的起始地址
    uint32_t pool_size;         // 本内存池字节容量
};

struct pool kernel_pool;
struct pool user_pool;



/* 内存池标记，用于判断用哪个内存池 */
enum pool_flags
{
    PF_KERNEL = 1,  // 内核内存池
    PF_USER = 2     // 用户内存池
};

#define PG_P_1  1   // 页表项或页目录项存在属性位
#define PG_P_0  0   // 页表项或页目录项存在属性位

#define PG_RW_R 0   // R/W 属性位值，读/执行
#define PG_RW_W 2   // R/W 属性位值，读/写/执行

#define PG_US_S 0   // U/S 属性位值，系统级
#define PG_US_U 4   // U/S 属性位值，用户级


#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)   // 取出页目录项
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)   // 取出页表项


void mem_init();
uint32_t* pte_ptr(uint32_t vaddr);
uint32_t* pde_ptr(uint32_t vaddr);
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt);
void* get_kernel_pages(uint32_t pg_cnt);
void malloc_init();



#endif // MEMORY_H
