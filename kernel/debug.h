#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H

void panic_spin(char *filename, int line, const char* func,
                const char* condition);

#define PANIC(...) panic_spin(__FILE__, __LINE, __FUNC__, __VA_ARGS__)


#ifndef NDEBUG
    #define ASSERT(CONDITION) ((void)0)\
#else
    #define ASSERT(CONDITION) \
        if (CONDITION){ }     \
        else                  \
        { \
            PANIC(CONDITION); \
        }
#endif



#endif // DEBUG_H
