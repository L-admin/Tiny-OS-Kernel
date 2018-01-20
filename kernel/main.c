#include "../lib/kernel/print.h"
#include "init.h"
#include "../thread/thread.h"

void k_thread_a(void* arg);

int main(void)
{
   put_str("I am kernel\n");
   init_all();

   thread_start("k_thread_a", 31, k_thread_a, "argA ");

   while(1)
       ;
   return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg)
{
   char* para = arg;
   while(1)
   {
      put_str(para);
   }
}
