#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H

#include "../lib/stdint.h"
#include "../lib/stdint.h"
#include "global.h"
#include "../lib/kernel/io.h"
#include "../lib/kernel/print.h"


typedef void* intr_handler;

#define PIC_M_CTRL 0x20     // 主片的控制端口是 0x20
#define PIC_M_DATA 0x21     // 主片的数据端口是 0x21
#define PIC_S_CTRL 0xa0     // 从片的控制端口是 0xa0
#define PIC_S_DATA 0xa1     // 从片的数据端口是 0xa1

#define IDT_DESC_CNT 0x30   // 目前总共支持的中断数 33个

#define EFLAGS_IF 0x00000200    // eflags寄存器中的if(第9位)位为1
#define GET_EFLAGS(EFLAG_VAR) asm volatile("pushfl; popl %0" : "=g"(EFLAG_VAR))

/* 中断门描述符 */
/////////////////////////////// 中断门描述符 ///////////////////////////////
// 31-16                           15 14-13  12  11-8  7-0
// 中断处理程序在目标段内偏移量31-16位   P   DPL   S   TYPE  全0       ; 高32位
//
// 31-16                           15-0
// 中断处理程序目标代码段段描述符选择子   中断处理程序在目标段内偏移量15-0位  ; 低32位
/////////////////////////////////////////////////////////////////////////
struct gate_desc
{
    uint16_t func_offset_low_word;  /* 中断处理程序在目标代码段偏移量0-15位 */
    uint16_t selector;              /* 选择子 */
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offset_high_word; /* 中断处理程序在目标代码段偏移量16-31位 */
};

enum intr_status
{
    INTR_OFF,   // 中断关闭
    INTR_ON     // 中断打开
};


void idt_init();

enum intr_status intr_get_status();
enum intr_status intr_set_status(enum intr_status status);
enum intr_status intr_enable();
enum intr_status intr_disable();

void register_handler(uint8_t vector_no, intr_handler function);


struct gate_desc idt[IDT_DESC_CNT];      // idt是中断描述符表,本质上就是个中断门描述符数组
char* intr_name[IDT_DESC_CNT];           // 保存异常的名字，方便调试
intr_handler idt_table[IDT_DESC_CNT];    // 定义中断处理函数指针数组
extern intr_handler intr_entry_table[IDT_DESC_CNT]; // 声明引用定义在kernel.S中的中断处理函数入口数组

#endif // INTERRUPT_H
