#include "init.h"

/* 负责初始化所有模块 */
void init_all()
{
    put_str("init_all\n\n");

    idt_init();         put_str("\n");   // 初始化中断
    mem_init();         put_str("\n");   // 初始化内存管理系统
    threads_init();     put_str("\n");   // 初始化线程相关
    timer_init();       put_str("\n");   // 初始化PIT
    console_init();     put_str("\n");   // 初始化终端
    keyboard_init();    put_str("\n");   // 键盘初始化
    tss_init();         put_str("\n");   // tss初始化
}
