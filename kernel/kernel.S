[bits 32]
%define ERROR_CODE
%define ZERO push 0

extern put_str  ; 声明外部函数

section .data
    intr_str db "interrupt occur!", 0xa, 0
    global intr_entry_table

    intr_entry_table:
        %macro VECTOR 2
            section .text
                intr%1entry:
                %2
                push intr_str   ; 压入参数
                call put_str    ; 调用打印函数
                add esp, 4
            
                mov al, 0x20    ; 中断结束命令EOI
                out 0xa0, al    ; 向从片发送
                out 0x20, al    ; 向主片发送

                add esp, 4      ; 跨过错误码
                iret            ; 从中断返回

            section .data
                dd intr%1entry  ; 存储各个中断程序入口
        %endmacro

VECTOR 0x00, ZERO
VECTOR 0x01, ZERO
VECTOR 0x02, ZERO
VECTOR 0x03, ZERO 
VECTOR 0x04, ZERO
VECTOR 0x05, ZERO
VECTOR 0x06, ZERO
VECTOR 0x07, ZERO 
VECTOR 0x08, ERROR_CODE
VECTOR 0x09, ZERO
VECTOR 0x0a, ERROR_CODE
VECTOR 0x0b, ERROR_CODE 
VECTOR 0x0c, ZERO
VECTOR 0x0d, ERROR_CODE
VECTOR 0x0e, ERROR_CODE
VECTOR 0x0f, ZERO 
VECTOR 0x10, ZERO
VECTOR 0x11, ERROR_CODE
VECTOR 0x12, ZERO
VECTOR 0x13, ZERO

VECTOR 0x14, ZERO
VECTOR 0x15, ZERO
VECTOR 0x16, ZERO
VECTOR 0x17, ZERO 
VECTOR 0x18, ERROR_CODE
VECTOR 0x19, ZERO
VECTOR 0x1a, ERROR_CODE
VECTOR 0x1b, ERROR_CODE 
VECTOR 0x1c, ZERO
VECTOR 0x1d, ERROR_CODE
VECTOR 0x1e, ERROR_CODE
VECTOR 0x1f, ZERO 

VECTOR 0x20, ZERO

