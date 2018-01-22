#ifndef __LIB_KERBEL_LIST_H
#define __LIB_KERBEL_LIST_H

#include "../../kernel/interrupt.h"
#include "../../kernel/global.h"


#define offset(struct_type,member) (int)(&((struct_type*)0)->member)
#define elem2entry(struct_type, struct_member_name, elem_ptr) \
     (struct_type*)((int)elem_ptr - offset(struct_type, struct_member_name))


struct list_elem
{
    struct list_elem* prev;
    struct list_elem* next;
};


/* 实现队列 */
struct list
{
    struct list_elem head;  // head 队首，固定不变. 第一个元素是head.next
    struct list_elem tail;  // tail 队尾，固定不变.
};


typedef bool (function)(struct list_elem*, int arg);



void list_init(struct list* list);
bool list_empty(struct list* plist);
uint32_t list_len(struct list* plist);
void list_insert_before(struct list_elem* before, struct list_elem* elem);
void list_push(struct list* plist, struct list_elem* elem);
void list_append(struct list* plist, struct list_elem* elem);
void list_remove(struct list_elem* pelem);
struct list_elem* list_pop(struct list* plist);
bool elem_find(struct list* plist, struct list_elem* obj_elem);
struct list_elem* list_traversal(struct list* plist, function func, int arg);


#endif // LIST_H
