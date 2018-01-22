#include "init.h"

/* 负责初始化所有模块 */
void init_all()
{
    put_str("init_all\n\n");

    idt_init();   put_str("\n");   // 初始化中断
    mem_init();   put_str("\n");   // 初始化内存管理系统
    threads_init(); put_str("\n");
    timer_init(); put_str("\n");   // 初始化PIT
}
