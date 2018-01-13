#ifndef __LIB_IO_H
#define __LIB_IO_H

#include "../stdint.h"

/*
 * 内联汇编格式
 * asm [volatile] ("assembly code" : output : input : clobber/modify)
 */


/* 向端口port 写入1个字节 */
static inline void outb(uint16_t port, uint8_t data)
{
    /*
     * outb %al, %dx; %al作为源操作数，8位数据; %dx 目的操作数，数据写入端口
     * N: 立即数约束，表示操作数为0-255之间的立即数
     * d: 表示寄存器 edx/dx/dl
     * a: 表示寄存器 eax/ax/al
     */
    asm volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}


/* 将addr处起始的word_cnt个字写入端口 */
static inline void outsw(uint16_t port, const void* addr, uint32_t word_cnt)
{
    asm volatile ("cld; rep outsw" : "+S" (addr), "+c" (word_cnt) : "d" (port));
}

/* 将从端口 port 读入一个字节返回 */
static inline uint8_t inb(uint6_t port)
{
    uint8_t data;
    asm volatile("inb %w1, %b0": "=a"(data) : "Nd"(port));
    return data;
}

/* 将从端口 port 读入的 word_cnt 个字写入 addr */
static inline void insw(uint16_t port, void* addr, uint32_t word_cnt)
{
    asm volatile ("cld; rep insw" : "+D" (addr), "+c" (word_cnt) : "d" (port) : "memory");
}

#endif // IO_H
