#ifndef __KERNEL_INIT_H
#define __KERNEL_INIT_H

#include "../lib/kernel/print.h"
#include "interrupt.h"

#include "../device/timer.h"
#include "../device/keyboard.h"

#include "memory.h"
#include "../thread/thread.h"


void init_all();

#endif // INIT_H
