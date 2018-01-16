#include "interrupt.h"
#include "../lib/stdint.h"
#include "global.h"
#include "../lib/kernel/io.h"
#include "../lib/kernel/print.h"

#define PIC_M_CTRL 0x20     // 主片的控制端口是 0x20
#define PIC_M_DATA 0x21     // 主片的数据端口是 0x21
#define PIC_S_CTRL 0xa0     // 从片的控制端口是 0xa0
#define PIC_S_DATA 0xa1     // 从片的数据端口是 0xa1

#define IDT_DESC_CNT 0x21   // 目前总共支持的中断数

/* 中断门描述结构体 */
struct gate_desc
{
    uint16_t func_offset_low_word;  /* 中断处理程序在目标代码段偏移量0-15位 */
    uint16_t selector;              /* 选择子 */
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offset_high_word; /* 中断处理程序在目标代码段偏移量16-31位 */
};

static struct gate_desc idt[IDT_DESC_CNT];      // idt是中断描述符表,本质上就是个中断门描述符数组

char* intr_name[IDT_DESC_CNT];  // 保存异常的名字，调试
intr_handler idt_table[IDT_DESC_CNT];   // 定义中断处理程序指针数组
extern intr_handler intr_entry_table[IDT_DESC_CNT]; // 声明引用定义在kernel.S中的中断处理函数入口数组


/* 初始化可编程中断控制器8259A */
static void pic_init()
{

   /* 初始化主片 */
   outb (PIC_M_CTRL, 0x11);   // ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_M_DATA, 0x20);   // ICW2: 起始中断向量号为0x20,也就是IR[0-7] 为 0x20 ~ 0x27.
   outb (PIC_M_DATA, 0x04);   // ICW3: IR2接从片.
   outb (PIC_M_DATA, 0x01);   // ICW4: 8086模式, 正常EOI

   /* 初始化从片 */
   outb (PIC_S_CTRL, 0x11);   // ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_S_DATA, 0x28);	  // ICW2: 起始中断向量号为0x28,也就是IR[8-15] 为 0x28 ~ 0x2F.
   outb (PIC_S_DATA, 0x02);	  // ICW3: 设置从片连接到主片的IR2引脚
   outb (PIC_S_DATA, 0x01);	  // ICW4: 8086模式, 正常EOI

   /* 打开主片上IR0,也就是目前只接受时钟产生的中断 */
   outb (PIC_M_DATA, 0xfe);
   outb (PIC_S_DATA, 0xff);

   put_str("    pic_init done\n");
}

/* 创建中断门描述符 */
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function)
{
    p_gdesc->func_offset_low_word = (uint32_t)function & 0x0000FFFF; // 取出低16位
    p_gdesc->selector = SELECTOR_K_CODE;    // 0000_0000_0000_1000b
    p_gdesc->dcount = 0;                    // 0000_0000
    p_gdesc->attribute = attr;
    p_gdesc->func_offset_high_word = (uint32_t)function & 0xFFFF0000;   // 取出高16位
}

/* 初始化中断描述符表 */
static void idt_desc_init()
{
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
        make_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);    // IDT_DESC_ATTR_DPL0 = 1000_1110b
    put_str("   idt_desc_init done\n");
}

/* 默认中断处理函数 */
static vopid general_init_hadnler(uint8_t vec_nr)
{
    if (vec_nr == 0x27 || vec_nr == 0x2f)
    {
        return;
    }

    put_str("init vector: 0x");
    put_int(vec_nr);
    put_char('\n');
}

/* 完成中断函数注册和异常名称注册 */
static void esception_init()
{
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
    {
        idt_table[i] = general_init_hadnler;   // 默认用general_init_handler，以后具体中断再修改
        intr_name[i] = "unknow";
    }
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
    // intr_name[15] 第15项是intel保留项，未使用
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
}

void idt_init()
{
    put_str("   idt_init start\n");
    idt_desc_init();    // 初始化中断描述符表
    pic_init();         // 初始化8259A

    /* 加载IDT */
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)(uint32_t)idt << 16));
    asm volatile("lidt %0" : : "m" (idt_operand));
    put_str("idt_init done\n");
}

/*
 * idt_all
 *    |
 *    |
 *   \|/       |--- pic_init
 * idt_init ---|
 *    |        |--- idt_desc_init
 *    |
 *   \|/
 *  加载IDT
 */
