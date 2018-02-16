#ifndef __USERPROG_PROCESS_H
#define __USERPROG_PROCESS_H



#include "../lib/stdint.h"
#include "../kernel/global.h"
#include "../thread/thread.h"
#include "../kernel/memory.h"
#include "tss.h"
#include "../device/console.h"


#define default_prio 31
#define USER_STACK3_VADDR (0xc0000000 - 0x1000)
#define USER_VADDR_START 0x8048000



#endif // PROCESS_H
