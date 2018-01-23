#ifndef __DEVICE_KEYBOARD_H
#define __DEVICE_KEYBOARD_H

#include "../lib/kernel/print.h"
#include "../kernel/interrupt.h"
#include "../lib/kernel/io.h"


#define KBD_BUF_PORT 0x60   // 键盘buffer 寄存器端口号为0x60



void keyboard_init();

#endif // KEYBOARD_H
