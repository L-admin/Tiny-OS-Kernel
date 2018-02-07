#ifndef __KERNEL_INIT_H
#define __KERNEL_INIT_H


#include "interrupt.h"
#include "memory.h"
#include "../thread/thread.h"
#include "../device/timer.h"
#include "../device/console.h"
#include "../device/keyboard.h"
#include "../userprog/tss.h"

#include "../lib/kernel/print.h"



void init_all();

#endif // INIT_H
