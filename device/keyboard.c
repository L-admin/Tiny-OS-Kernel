#include "keyboard.h"


static void intr_keyboard_hadnler();


/*
 * 键盘中断处理程序
 */
static void intr_keyboard_handler()
{
    put_char("k");
    inb(KBD_BUF_PORT);  // 必须读取"输出缓冲区寄存器"，否则80482不再继续响应键盘中断
    return;
}

void keyboard_init()
{
    put_str("keyboard init start\n");
    register_handler(0x21, intr_keyboard_hadnler);
    put_str("keyboard init done\n");
}
