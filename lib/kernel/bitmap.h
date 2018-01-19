#ifndef __LIB_KERNEL_BITMAP_H
#define __LIB_KERNEL_BITMAP_H

#include "../../kernel/global.h"
#include "../stdint.h"
#include "../string.h"
#include "print.h"
#include "../../kernel/debug.h"
#include "../../kernel/interrupt.h"


#define BITMAP_MASK 1

struct bitmap
{
    uint32_t btmp_bytes_len;   // 位图中字节长度(注意不是位的长度，是字节长度)
    uint8_t* bits;      // 位图存放的起始地址(字节)
};

void bitmap_init(struct bitmap* btmp);
bool bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx);
int bitmap_scan(struct bitmap* btmp, uint32_t cnt);
void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value);

#endif // BITMAP_H
