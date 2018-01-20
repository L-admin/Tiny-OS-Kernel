#include "thread.h"


static void kernel_thread(thread_func* function, void* func_arg)
{
    function(func_arg);
}


/* 初始化线程栈的thread_stack */
void thread_create(struct task_struct* pthread, thread_func function,
                   void* func_arg)
{
    /* 留出中断栈和线程栈的空间 */
    pthread->self_kstack -= sizeof(struct intr_stack);
    pthread->self_kstack -= sizeof(struct thread_stack);

    struct thread_stack* kthread_stack =
            (struct thread_stack*)pthread->self_kstack;

    kthread_stack->eip = kernel_thread;
    kthread_stack->function = function;
    kthread_stack->func_arg = func_arg;
    kthread_stack->ebp = kthread_stack->ebx
            = kthread_stack->edi = kthread_stack->esi;
}


/* 初始化线程基本信息 */
void init_thread(struct task_struct* pthread, char*name, int prio)
{
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);
    pthread->status = TASK_RUNNING;

    pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PG_SIZE);
    pthread->stack_magic = 0x19870916; // 自定义魔数
}


/* 创建一优先级为prio的线程,线程名为name,线程所执行的函数是function(func_arg) */
struct task_struct* thread_start(char* name, int prio,
                                 thread_func function, void* func_arg)
{
    struct task_struct* thread = get_kernel_page(1);

    init_thread(thread, name, prio);
    thread_create(thread, function, func_arg);

     asm volatile ("movl %0, %%esp; pop %%ebp; pop %%ebx; pop %%edi; pop %%esi; ret" : : "g" (thread->self_kstack) : "memory");

    return thread;
}


