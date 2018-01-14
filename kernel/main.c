#include "../lib/kernel/print.h"
#include "init.h"

int main()
{
    put_str("I am kernel\n");
    init_all();
    asm volatile("sti");	     // 打开中断 sti 指令，已将除时钟之外的所有设备中断都屏蔽了
    while(1);
}
