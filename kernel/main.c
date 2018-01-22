#include "../lib/kernel/print.h"
#include "init.h"
#include "../thread/thread.h"

void k_thread_a(void* arg);
void k_thread_b(void* arg);

int main(void)
{
   put_str("I am kernel\n");
   init_all();

   thread_start("k_thread_a", 31, k_thread_a, "argA ");
   thread_start("k_thread_b", 8, k_thread_b, "argB ");

   while(1)
       ;
   return 0;
}

/*
 * 在线程中运行的函数a
 */
void k_thread_a(void* arg)
{
   char* para = arg;
   while(1)
   {
      put_str(para);
   }
}

/*
 * 在线程中运行的函数b
 */
void k_thread_b(void* arg)
{
   char* para = arg;
   while(1)
   {
      put_str(para);
   }
}
