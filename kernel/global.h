#ifndef __KERNEL_GLOBAL_H
#define __KERNEL_GLOBAL_H

#include "../lib/stdint.h"


/*********************** GDT 属性 ********************/
/*********************** gdt描述符属性 ************************************
**  31-24  23  22   21  20   19-16  15  14-13  12  11-8  7-0
**  段基址  G   D/B  L   AVL  段界限   P    DPL   S  TYPE  段基址  ; 高32位
**
**  31-16   15-0
**  段基址   段界限                                               ; 低32位
//**********************************************************************/

#define DESC_G_4K   1   // 段界限粒度，1表示4K

#define DESC_D_32   1   //

#define DESC_L      0   // 64位代码标记

#define DESC_AVL    0   // CPU 不用此位

#define DESC_P      1   //

#define DESC_DPL_0  0   //  段描述符特权级
#define DESC_DPL_1  1
#define DESC_DPL_2  2
#define DESC_DPL_3  3

#define DESC_S_CODE     1               // S 为 1 表示数据段
#define DESC_S_DATA     DESC_S_CODE     // S 为 1 表示数据段
#define DESC_S_SYS      0               // S 为 0 表示系统段

#define DESC_TYPE_CODE  8       // 1000b x=1,c=0,r=0,a=0 代码段可执行,非一致性,不可读,已访问位清0
#define DESC_TYPE_DATA  2       // 0010b x=0,e=0,w=1,a=0 数据段不可执行,向上扩展,可写,已访问位清0
/* TSS TYPE字段(10B1) */
#define DESC_TYPE_TSS   9       // 1001b B位为0表示不忙



// 1100_0000b  P(1) DPL(2) S(1) TYPE(4)
#define GDT_ATTR_HIGH		 ((DESC_G_4K << 7) + (DESC_D_32 << 6) + (DESC_L << 5) + (DESC_AVL << 4))
// 1111_1000b  P(1) DPL(2) S(1) TYPE(4)
#define GDT_CODE_ATTR_LOW_DPL3	 ((DESC_P << 7) + (DESC_DPL_3 << 5) + (DESC_S_CODE << 4) + DESC_TYPE_CODE)
// 1111_1000b  P(1) DPL(2) S(1) TYPE(4)
#define GDT_DATA_ATTR_LOW_DPL3	 ((DESC_P << 7) + (DESC_DPL_3 << 5) + (DESC_S_DATA << 4) + DESC_TYPE_DATA)


/****************** 选择子属性 ***************/
#define RPL0    0
#define RPL1    1
#define RPL2    2
#define RPL3    3

#define TI_GDT  0
#define TI_LDT  1

/***************** 构造选择子 **************/
#define SELECTOR_K_CODE     ((1 << 3) + (TI_GDT << 2) + RPL0) // 0000_0000_0000_1000b 第1个
#define SELECTOR_K_DATA     ((2 << 3) + (TI_GDT << 2) + RPL0) // 0000_0000_0001_0000b 第2个
#define SELECTOR_K_STACK    SELECTOR_K_DATA                   // 0000_0000_0001_0000b 第3个
#define SELECTOR_K_GS	    ((3 << 3) + (TI_GDT << 2) + RPL0) // 0000_0000_0001_1000b 第4个
#define SELECTOR_U_CODE	    ((5 << 3) + (TI_GDT << 2) + RPL3) // 0000_0000_0010_1011b 第5个
#define SELECTOR_U_DATA	    ((6 << 3) + (TI_GDT << 2) + RPL3) // 0000_0000_0011_0011b 第6个
#define SELECTOR_U_STACK    SELECTOR_U_DATA                   // 0000_0000_0011_0011b 第7个


/***************** TSS 描述符属性 ***********/
#define TSS_DESC_D 0

#define TSS_ATTR_HIGH ((DESC_G_4K << 7) + (TSS_DESC_D << 6) + (DESC_L << 5) + (DESC_AVL << 4) + 0x0)
#define TSS_ATTR_LOW ((DESC_P << 7) + (DESC_DPL_0 << 5) + (DESC_S_SYS << 4) + DESC_TYPE_TSS)
#define SELECTOR_TSS ((4 << 3) + (TI_GDT << 2 ) + RPL0)

struct gdt_desc
{
    uint16_t limit_low_word;        // 段界限 0-15
    uint16_t base_low_word;         // 段基址 0-15
    uint8_t  base_mid_byte;         // 段基址 16-23
    uint8_t  attr_low_byte;         // TYPE S DPL P
    uint8_t  limit_high_attr_high;  // 段界限16-20 AVL L D/B G
    uint8_t  base_high_byte;        // 段基址 24-31
};


/********************* IDT描述符属性 *********************/
/********************* 中断门描述符 ***************************************
** 31-16                           15 14-13  12  11-8  7-0
** 中断处理程序在目标段内偏移量31-16位   P   DPL   S   TYPE  全0       ; 高32位
**
** 31-16                           15-0
** 中断处理程序目标代码段段描述符选择子   中断处理程序在目标段内偏移量15-0位  ; 低32位
************************************************************************/
#define IDT_DESC_P      1
#define IDT_DESC_DPL0   0
#define IDT_DESC_DPL3   3
#define IDT_DESC_32_TYPE    0xe // 32位的门
#define IDT_DESC_16_TYPE    0x6 // 16位的门，不用，定义它只为和32位门区分
#define IDT_DESC_ATTR_DPL0  ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)
#define IDT_DESC_ATTR_DPL3  ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)



#define NULL ((void*)0)
#define DIV_ROUND_UP(X, STEP) ((X + STEP - 1) / (STEP))
#define bool int
#define true 1
#define false 0


#endif // GLOBAL_H
