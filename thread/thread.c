#include "thread.h"

static struct list_elem* thread_tag; // 用于保存队列中的当前运行的线程结点

static void make_main_thread();



/* 获取当前线程PCB */
struct task_struct* running_thread()
{
    uint32_t esp;
    asm ("mov %%esp, %0" : "=g"(esp));

    return (struct task_struct*)(esp & 0xfffff000);
}


static void kernel_thread(thread_func* function, void* func_arg)
{
    /* 执行function前要打开中断，避免后面的时钟中断被屏蔽，无法调度其他线程 */
    intr_enable();
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

    if (pthread == main_thread)
        pthread->status = TASK_RUNNING;
    else
        pthread->status = TASK_READY;

    pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PG_SIZE);
    pthread->priority = prio;
    pthread->ticks = prio;  // 进程优先级体现在 每次在处理器上执行的时间滴答数
    pthread->elapsed_ticks = 0;
    pthread->pgdir = NULL;
    pthread->stack_magic = 0x19870916;      // 自定义魔数
}

/*
 * 创建线程入口，执行完 init_thread 和 thread_create 后,
 * 再通过 list_append 函数把新创建的线程加入就绪队列和全部队列
 */
struct task_struct* thread_start(char* name, int prio,
                                 thread_func function, void* func_arg)
{
    struct task_struct* thread = get_kernel_pages(1);   // PCB都位于内核空间, 包括用户进程的 PCB 也是内核空间。

    init_thread(thread, name, prio);
    thread_create(thread, function, func_arg);

    /* 确保之前不在就绪队列中 */
    ASSERT(!elem_find(&thread_ready_list,  &(thread->general_tag)));
    /* 加入就绪对列 */
    list_append(&thread_ready_list, &(thread->general_tag));


    /* 确保之前不在所有任务队列中 */
    ASSERT(!elem_find(&thread_all_list, &(thread->all_list_tag)));
    /* 加入全部线程队列 */
    list_append(&thread_all_list, &(thread->all_list_tag));

    return thread;
}

static void make_main_thread()
{
    /*
     * 因为 main线程早已运行，咱们在 loader.S 中进入内核时 mov esp, 0xc009f000
     * 就是为其预留了TCB，地址为 0xc009e000, 因此不需要通过get_kernel_page
     * 另分配一页
     */
    main_thread = running_thread();
    init_thread(main_thread, "main", 31);

    /*
     * main 函数是当前线程，当前线程不再thread_ready_list中，
     * 所以只将其加在 thread_all_list 中
     */
    ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
    list_append(&thread_all_list, &main_thread->all_list_tag);
}


/*
 * 实现任务调度, 调度器的主要任务就是读写就绪队列，增删里面的结点
 */
void schedule()
{
    ASSERT(intr_get_status() == INTR_OFF);

    struct task_struct* cur = running_thread();

    /* 根据线程状态判断当前线程是出于什么原因才被换下CPU */
    if (cur->status == TASK_RUNNING)    // 时间片用完
    {
        ASSERT(!elem_find(&thread_ready_list, &(cur->general_tag)));

        cur->ticks = cur->priority;
        cur->status = TASK_READY;
        list_append(&thread_ready_list, &cur->general_tag);    // 加入就绪队列
    }
    else
    {

    }

    ASSERT(!list_empty(&thread_ready_list));

    thread_tag = NULL;
    thread_tag = list_pop(&thread_ready_list);

    struct task_struct* next = elem2entry(struct task_struct, general_tag, thread_tag);
    next->status = TASK_RUNNING;

    switch_to(cur, next);
}


/* 初始化线程环境 */
void threads_init()
{
    put_str("thread_init start\n");
    list_init(&thread_ready_list);
    list_init(&thread_all_list);

    /* 将当前main函数创建为线程 */
    make_main_thread();

    put_str("thread_init_done\n");
}







