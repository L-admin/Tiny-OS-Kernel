#include "sync.h"


void sema_init(struct semaphore* psema, uint8_t value)
{
    psema->value = value;
}

void sema_down(struct semaphore* psema)
{
    enum intr_status old_status = intr_disable();     // 关中断保证原子操作

    while(psema->value == 0)    // 若 value为 0,表示锁已被他人持有。
    {
        ASSERT(!elem_find(&psema->waiters, &running_thread()->general_tag));
        if (elem_find(&psema->waiters, &running_thread()->general_tag))
            PANIC("sema_down: thread blocked has been in waiters_list\n");

        list_append(&(psema->waiters), &(running_thread()->general_tag));
        thread_block(TASK_BLOCKED);     // 线程调度
    }

    psema->value--;         /* 被唤醒，获得锁, 或者直接获得锁 */
    ASSERT(psema->value == 0);

    intr_set_status(old_status);    // 恢复之前的中断状态
}

void sema_up(struct semaphore* psema)
{
    enum intr_status old_status = intr_disable();

    ASSERT(psema->value == 0);

    if (!list_empty(&(psema->waiters)))
    {
        struct task_struct* thread_blocked =
            elem2entry(struct task_struct, general_tag, list_pop(&psema->waiters));
        thread_unblock(thread_blocked); // 这里解除阻塞，并不是立即可以运行，而是放到就绪队列里面，能够再次被调度
    }

    psema->value++;
    ASSERT(psema->value == 1);

    intr_set_status(old_status);
}



void lock_init(struct lock* plock)
{
    plock->holder = NULL;
    sema_init(&(plock->semaphore), 1);
    plock->holder_repeat_nr = 0;
}

/* 获得锁 */
void lock_acquire(struct lock* plock)
{
    if (plock->holder != running_thread())  // 线程可能会嵌套申请同一把锁，这种情况会形成死锁，自己等待自己释放锁
    {
        sema_down(&plock->semaphore);
        plock->holder = running_thread();

        ASSERT(plock->holder_repeat_nr == 0);

        plock->holder_repeat_nr = 1;
    }
    else
    {
        plock->holder_repeat_nr++;
    }
}

/* 释放锁 */
void lock_release(struct lock* plock)
{
    ASSERT(plock->holder == running_thread());

    if (plock->holder_repeat_nr > 1)
    {
        plock->holder_repeat_nr--;
        return;
    }

    ASSERT(plock->holder_repeat_nr == 1);

    plock->holder = NULL;           // 把锁的持有者置空放在V操作之前
    plock->holder_repeat_nr = 0;

    sema_up(&plock->semaphore);
}
