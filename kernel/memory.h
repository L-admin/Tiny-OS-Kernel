#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H

#include "../lib/stdint.h"
#include "../lib/kernel/bitmap.h"
#include "../lib/kernel/print.h"



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


void mem_init();


#endif // MEMORY_H
