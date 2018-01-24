#include "interrupt.h"


static void pic_init();
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function);
static void idt_desc_init();
static void general_init_hadnler(uint8_t vec_nr);
static void exception_init();


/*
 * 初始化可编程中断控制器8259A
 */
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


    /* 测试键盘，只打开键盘中断，其余关闭 */
//    outb (PIC_M_DATA, 0xfd);
//    outb (PIC_S_DATA, 0xff);


    put_str("   pic_init done\n");
}


/*
 * 创建中断门描述符
 */
/////////////////////////////// 中断门描述符 //////////////////////////////
// 31-16                           15 14-13  12  11-8  7-0
// 中断处理程序在目标段内偏移量31-16位   P   DPL   S   TYPE  全0       ; 高32位
//
// 31-16                           15-0
// 中断处理程序目标代码段段描述符选择子   中断处理程序在目标段内偏移量15-0位  ; 低32位
/////////////////////////////////////////////////////////////////////////
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function)
{
    p_gdesc->func_offset_low_word = (uint32_t)function & 0x0000FFFF; // 取出低16位
    p_gdesc->selector = SELECTOR_K_CODE;    // 0000_0000_0000_1000b
    p_gdesc->dcount = 0;                    // 0000_0000
    p_gdesc->attribute = attr;              // 1000_1110b 或 1110_1110b  (DPL不同)
    p_gdesc->func_offset_high_word = (uint32_t)function & 0xFFFF0000;   // 取出高16位
}


/*
 * 初始化中断描述符表
 */
static void idt_desc_init()
{
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
        make_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);    // IDT_DESC_ATTR_DPL0 = 1000_1110b
    put_str("   idt_desc_init done\n");
}


/*
 * 默认中断处理函数
 */
static void general_init_hadnler(uint8_t vec_nr)
{
    if (vec_nr == 0x27 || vec_nr == 0x2f)
        return;

    set_cursor(0);
    int cursor_pos = 0;
    while(cursor_pos < 320)
    {
        put_char(' ');
        cursor_pos++;
    }

    set_cursor(0);      // 重置光标为屏幕左上角
    put_str("!!!!!!!      excetion message begin  !!!!!!!!\n");
    set_cursor(88);     // 从第2行第8个字符开始打印
    put_str(intr_name[vec_nr]);

    if (vec_nr == 14)   // 若为Pagefault,将缺失的地址打印出来并悬停
    {
       int page_fault_vaddr = 0;
       asm ("movl %%cr2, %0" : "=r" (page_fault_vaddr));	  // cr2是存放造成page_fault的地址
       put_str("\npage fault addr is ");put_int(page_fault_vaddr);
    }

    put_str("\n!!!!!!!      excetion message end    !!!!!!!!\n");

    while(1)
        ;
}


/*
 * 完成中断函数注册和异常名称注册
 */
static void exception_init()
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


/*
 * 获取当前中断状态
 */
enum intr_status intr_get_status()
{
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF & eflags) ? INTR_ON : INTR_OFF;
}


/*
 * 设置中断状态为status
 */
enum intr_status intr_set_status(enum intr_status status)
{
    return status & INTR_ON ? intr_enable() : intr_disable();
}


/*
 * 打开中断并返回打开中断前的"中断是否打开状态"
 */
enum intr_status intr_enable()
{
    enum intr_status old_status;

    old_status = intr_get_status();

    if (old_status == INTR_OFF)
       asm volatile("sti");     // 打开中断

    return old_status;
}


/*
 * 关闭中断并返回关闭中断前的"中断是否打开状态"
 */
enum intr_status intr_disable()
{
    enum intr_status old_status;

    old_status = intr_get_status();

    if (old_status == INTR_ON)
        asm volatile("cli" : : : "memory"); // 关中断

    return old_status;
}


/*
 * 在中断处理程序数组第vector_no个元素中注册中断处理函数
 */
void register_handler(uint8_t vector_no, intr_handler function) // intr_handler 函数指针
{
    idt_table[vector_no] = function;
}


void idt_init()
{
    put_str("idt_init start\n");

    idt_desc_init();    // 初始化中断描述符表
    exception_init();   // 异常名初始化并注册默认的中断处理函数
    pic_init();         // 初始化8259A

    /* 加载IDT */
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)(uint32_t)idt << 16));

    /* 打开中断 */
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
