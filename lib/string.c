#include "string.h"

/* 将dst_起始的size个字节置为value */
void memset(void* dst_, uint8_t value, uint32_t size)
{
    ASSERT(dst_ != NULL);

    uint8_t* dst = (uint8_t*)dst_;
    while(size-- > 0)
        *dst++ = value;
}

/* 将src_起始的size个字节复制到dst_ */
void memcpy(void *dst_, const void *src_, uint32_t size)
{
    ASSERT(dst_ != NULL && src_ != NULL);

    uint8_t* dst = (uint8_t*)dst_;
    const uint8_t* src = src_;  // const不能通过*src修改值
    while (size-- > 0)
        *dst++ = *src++;    /* 注意踩内存 */
}

/* 连续比较以地址a_和地址b_开头的size个字节,若相等则返回0,若a_大于b_返回+1,否则返回-1 */
int memcmp(const void* a_, const void* b_, uint32_t size)
{
    ASSERT(a_ != NULL || b_ != NULL);

    const char* a = a_;
    const char* b = b_;

    while (size-- > 0)
    {
        if(*a != *b)
            return *a > *b ? 1 : -1;
        a++;
        b++;
    }

    return 0;
}


/* 返回字符串长度 */
uint32_t strlen(const char *str)
{
    ASSERT(str != NULL);

    const char* p = str;
    while(*p++)
        ;
    return p-str-1;
}

/* 比较两个字符串,若a_中的字符大于b_中的字符返回1,相等时返回0,否则返回-1. */
int8_t strcmp (const char* a, const char* b)
{
   ASSERT(a != NULL && b != NULL);

   while (*a != 0 && *a == *b) {
      a++;
      b++;
   }
    /* 如果*a小于*b就返回-1,否则就属于*a大于等于*b的情况。在后面的布尔表达式"*a > *b"中,
    * 若*a大于*b,表达式就等于1,否则就表达式不成立,也就是布尔值为0,恰恰表示*a等于*b */
   return *a < *b ? -1 : *a > *b;
}


/* 从左到右查找字符串str中首次出现字符ch的地址(不是下标,是地址) */
char* strchr(const char* str, const uint8_t ch)
{
    ASSERT(str != NULL);

    const char* p = str;
    while(*p != 0)
    {
        if (*p == ch)
            return (char*)p;
        p++;
    }
    return NULL;
}

/* 从后往前查找字符串str中首次出现字符ch的地址(不是下标,是地址) */
char* strrchr(const char* str, const uint8_t ch)
{
    ASSERT(str != NULL);

    const char* p = str;
    const char* last_char = NULL;

    while(*p != 0)
    {
        if (*p == ch)
             last_char = p;
        p++;
     }
     return (char*)last_char;
}

/* 将字符串src_拼接到dst_后,返回拼接的串地址 */
char* strcat(char* dst_, const char* src_)
{
    ASSERT(dst_ != NULL && src_ != NULL);

    char* p1 = dst_;
    const char* p2 = src_;

    while(*p1++)
        ;

    --p1;

    while((*p1++ = *p2++))
        ;

    return dst_;
}

/* 在字符串str中查找指定字符ch出现的次数 */
uint32_t strchrs(const char *str, uint8_t ch)
{
    ASSERT(str != NULL);

    uint32_t ch_cnt = 0;
    const char* p = str;

    while(*p != 0)
    {
        if (*p == ch)
            ch_cnt++;
        p++;
    }

    return ch_cnt;
}
