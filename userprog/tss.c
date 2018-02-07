#include "tss.h"


static struct gdt_desc make_gdt_desc(uint32_t* desc_addr, uint32_t limit,uint8_t attr_low, uint8_t attr_high);


static struct t_tss tss;


/* 创建gdt描述符 */
// 段基址(16)   段界限(16)
// 段基址(8)    G(1) D/B(1) L(1) AVL(1) 段界限(4)    P(1) DPL(2) S(1) TYPE(4)    段基址(8)
static struct gdt_desc make_gdt_desc(uint32_t* desc_addr, uint32_t limit,
                                    uint8_t attr_low, uint8_t attr_high)
{
    uint32_t desc_base = (uint32_t)desc_addr;

    struct gdt_desc desc;
    desc.limit_low_word = limit & 0x0000ffff;   // 段界限(16)
    desc.base_low_word = desc_base & 0x0000ffff;    // 段基址(16)
    desc.base_mid_byte = ( (desc_base & 0x00ff0000) >> 16 ); // 段基址(8)
    desc.attr_low_byte = (uint8_t)(attr_low);   // P(1) DPL(2) S(1) TYPE(4)
    desc.limit_high_attr_high = ( ((limit & 0x000f0000) >> 16) + (uint8_t)(attr_high) ); // G(1) D/B(1) L(1) AVL(1) 段界限(4)
    desc.base_high_byte = desc_base >> 24;  // 段基址(8)

    return desc;
}



/* 在 gdt 中创建 tss 并重新加载gdt */
void tss_init()
{
    put_str("tss_init start\n");

    uint32_t tss_size = sizeof(tss);
    memset(&tss, 0, tss_size);

    tss.ss0 = SELECTOR_K_STACK;
    tss.io_base = tss_size;

    /* gdt段基址为0x900，把tss 放到第4个位置 */
    *((struct gdt_desc*)0xc0000920) = make_gdt_desc((uint32_t*)&tss, tss_size-1, TSS_ATTR_LOW, TSS_ATTR_HIGH);

    /* 在gdt中添加dpl为3的数据段和代码段描述符 */
    *((struct gdt_desc*)0xc0000928) = make_gdt_desc((uint32_t*)0, 0xfffff, GDT_CODE_ATTR_LOW_DPL3, GDT_ATTR_HIGH);
    *((struct gdt_desc*)0xc0000930) = make_gdt_desc((uint32_t*)0, 0xfffff, GDT_DATA_ATTR_LOW_DPL3, GDT_ATTR_HIGH);

    /* gdt 16位的limit 32位的段基址 */
    uint64_t gdt_operand = ((8 * 7 - 1) | ((uint64_t)(uint32_t)0xc0000900 << 16));   // 7个描述符大小
    asm volatile ("lgdt %0" : : "m" (gdt_operand));
    asm volatile ("ltr %w0" : : "r" (SELECTOR_TSS));
    put_str("tss_init and ltr done\n");
}


