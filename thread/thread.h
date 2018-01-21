#ifndef __THREAD_THREAD_H
#define __THREAD_THREAD_H

#include "../lib/stdint.h"
#include "../lib/string.h"
#include "../kernel/global.h"
#include "../kernel/memory.h"



/* 线程函数 */
typedef void thread_func(void*);


/* 进程或线程的状态 */
enum task_status
{
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

/**************** 中断栈 intr_stack ****************
 * 此结构用于中断发生时保护程序(线程或进程)的上下文环境
 * 进程或线程被外部中断或软中断打断时，会按此结构压入上下文
 * 此栈在线程自己的内核栈中位置固定，所以在页的最顶端
 **************************************************/
struct intr_stack
{
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    /* 以下由CPU从低特权级进入高特权级时压入 */
    uint32_t err_code;
    void (*eip) (void);
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

/*********************************
 * 线程栈，用来存储线程中待执行的函数
 * 此结构在线程自己的内核栈中位置不固定
 * 仅用在swithc_to时保存线程环境
 *********************************/
struct thread_stack
{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    /* kernel_thread(thread_func* func, void* func_arg)
       {
           func(arg);
       }
    */
    void (*eip) (thread_func* func, void* func_arg);


    /* 以下供第一次被调度上CPU时使用 */
    void (*unused_retaddr); // 返回地址
    thread_func* function;  // 由kernel_thread 所调用的函数名
    void* func_arg;         // 由kernel_thread 所调用的函数所需的参数
};

/* 进程或线程 PCB */
struct task_struct
{
    uint32_t* self_kstack;      // 各内核线程都用自己的内核栈 thread_stack
    enum task_status status;    // 线程状态
    uint8_t priority;           // 线程的优先级
    char name[16];              // 线程名
    uint32_t stack_magic;       // 魔数，栈的边界标记，用于检测栈的溢出
};

void thread_create(struct task_struct* pthread, thread_func function, void* func_arg);
void init_thread(struct task_struct* pthread, char*name, int prio);
struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg);

#endif // THREAD_H
