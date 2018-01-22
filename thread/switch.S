[bits 32]
section .text_to
global switch_to
switch_to:
    ; 栈中此处时返回地址
	push esi
	push edi
	push ebx
	push ebp

    mov eax, [esp + 20]
	mov [eax], esp

    ; 以上是备份当前线程的环境，下面是恢复下一个线程的环境
	mov eax, [esp + 24]
	mov esp, [eax]

    pop ebp
	pop ebx
	pop edi
	pop esi
	ret
