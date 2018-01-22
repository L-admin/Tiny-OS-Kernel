#ifndef __DEVICE_TIME_H
#define __DEVICE_TIME_H

#include "../lib/stdint.h"
#include "../lib/kernel/io.h"
#include "../lib/kernel/print.h"
#include "../kernel/interrupt.h"
#include "../thread/thread.h"
#include "../kernel/debug.h"

void timer_init();

#endif
