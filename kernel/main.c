#include "../lib/kernel/print.h"
#include "init.h"
#include "debug.h"

int main(void)
{
    put_str("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t)(0xb00));
    mem_pool_init(mem_bytes_total);
    while(1)
        ;
}
