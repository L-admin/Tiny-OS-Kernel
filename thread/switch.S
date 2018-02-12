[bits 32]
section .text_to
global switch_to
switch_to:
    ; 栈中此处时返回地址
	push esi
	push edi
	push ebx
	push ebp

    mov eax, [esp + 20]		; [esp+20]即当前线程PCB地址
	mov [eax], esp			; 将当前线程栈指针保存到当前线程PCB起始处(self_kstack)

    ;以上是备份当前线程的环境，下面是恢复下一个线程的环境

    mov eax, [esp + 24]		; [esp+24]即即将要执行的线程PCB地址
	mov esp, [eax]			; [eax] PCB起始处(self_kstack), 线程栈指针

    pop ebp
	pop ebx
	pop edi
	pop esi
	ret

