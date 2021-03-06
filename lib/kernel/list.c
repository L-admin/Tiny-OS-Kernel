#include "list.h"


/* 初始化双向链表 */
void list_init(struct list* list)
{
    list->head.prev = NULL;
    list->head.next = &(list->tail);
    list->tail.prev = &(list->head);
    list->tail.next = NULL;
}


/* 判断链表是否为空,空时返回true,否则返回false */
bool list_empty(struct list* plist)
{
    return plist->head.next == &(plist->tail);
}


/* 返回链表长度 */
uint32_t list_len(struct list* plist)
{
    struct list_elem* elem = plist->head.next;
    uint32_t len = 0;

    while(elem != &(plist->tail))
    {
        len++;
        elem = elem->next;
    }

    return len;
}


/* 把链表elem插入在元素before之前 */
void list_insert_before(struct list_elem* before, struct list_elem* elem)
{
    enum intr_status old_status = intr_disable();   // 实现原子操作，关掉中断

    before->prev->next = elem;

    elem->prev = before->prev;
    elem->next = before;

    before->prev = elem;

    intr_set_status(old_status);
}


/* 添加元素到列表队首,类似push_front操作 */
void list_push(struct list* plist, struct list_elem* elem)
{
    list_insert_before(plist->head.next, elem);     // 第一个元素是 head.next
}


/* 追加元素到链表队尾,类似队列的先进先出操作 */
void list_append(struct list* plist, struct list_elem* elem)
{
    list_insert_before(&(plist->tail), elem);
}


/* 使元素pelem脱离链表 */
void list_remove(struct list_elem* pelem)
{

    enum intr_status old_status = intr_disable();   // 实现原子操作，关掉中断

    pelem->prev->next = pelem->next;
    pelem->next->prev = pelem->prev;

    intr_set_status(old_status);
}


/* 将链表第一个元素弹出并返回 */
struct list_elem* list_pop(struct list* plist)
{
    struct list_elem* elem = plist->head.next;
    list_remove(elem);
    return elem;
}


/* 从链表中查找元素obj_elem,成功时返回true,失败时返回false */
bool elem_find(struct list* plist, struct list_elem* obj_elem)
{
    struct list_elem* elem = plist->head.next;

    while(elem != &(plist->tail))
    {
        if (elem == obj_elem)
            return true;
        elem = elem->next;
    }

    return false;
}


struct list_elem* list_traversal(struct list* plist, function func, int arg)
{
    struct list_elem* elem = plist->head.next;

    if (list_empty(plist))
        return NULL;

    while(elem != &(plist->tail))
    {
        if (func(elem, arg))
            return elem;
        elem = elem->next;
    }

    return NULL;
}

