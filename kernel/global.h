#ifndef __KERNEL_GLOBAL_H
#define __KERNEL_GLOBAL_H

#define RPL0 0  // 选择子请求特权级 00b
#define RPL1 1  // 选择子请求特权级 01b
#define RPL2 2  // 选择子请求特权级 10b
#define RPL3 3  // 选择子请求特权级 11b

#define TI_GDT 0    // 选择子 TI 位,指示GDT
#define TI_LDT 1    // 选择子 TI 位,指示IDT

#define SELECTOR_K_CODE	   ((1 << 3) + (TI_GDT << 2) + RPL0)    // 0000_0000_0000_1000b
#define SELECTOR_K_DATA	   ((2 << 3) + (TI_GDT << 2) + RPL0)    // 0000_0000_0001_0000b
#define SELECTOR_K_STACK   SELECTOR_K_DATA                      // 0000_0000_0000_1000b
#define SELECTOR_K_GS	   ((3 << 3) + (TI_GDT << 2) + RPL0)    // 0000_0000_0001_1000b

//--------------   IDT描述符属性  ------------//
#define	 IDT_DESC_P	 1      // 1b
#define	 IDT_DESC_DPL0   0  // 描述符特权级, 00b
#define	 IDT_DESC_DPL3   3  // 描述符特权级, 11b
#define	 IDT_DESC_32_TYPE     0xE   // 32位的门 1110b
#define	 IDT_DESC_16_TYPE     0x6   // 16位的门，不用，定义它只为和32位门区分
#define	 IDT_DESC_ATTR_DPL0  ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)  // 1000_1110b
#define	 IDT_DESC_ATTR_DPL3  ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)  // 1110_1110b


#define NULL ((void*)0)
#define bool int
#define true 1
#define false 0


#endif // GLOBAL_H
