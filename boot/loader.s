;内核加载器
;---------------------------------------

%include "boot.inc"
section loader vstart=LOADER_BASE_ADDR      ;0x900
    
;    LOADER_STACK_TOP equ LOADER_BASE_ADDR   

    jmp loader_start                ; 0x900

    GDT_BASE: dd 0x00000000         ; 0x903
              dd 0x00000000         ; 0x907

    CODE_DESC: dd 0x0000FFFF        ; 0x90b
               dd DESC_CODE_HIGH4   ; 0x90f

    DATA_STACK_DESC: dd 0x0000FFFF  ; 0x913
                     dd DESC_DATA_HIGH4 ;0x917

    VIDEO_DESC: dd 0x80000007       ; 0x91b
                dd DESC_VIDEO_HIGH4 ; 0x91f
   
    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1

    times 60 dq 0       ; 段描述符备用空间60*8=480

    ; 构造选择子
    SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
    SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
    SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

    gdt_ptr dw GDT_LIMIT            ; 0xb03   2字节 GDT_LIMIT=31
            dd GDT_BASE             ; 0xb05   4字节


loader_start:
;;;;;;;;;;;;;;;;;;;;;;;; 准备进入保护模式 ;;;;;;;;;;;;;;;;;
    ; 打开A20
    in al, 0x92             ; 0x928
    or al, 00000010b        ; 0x92a
    out 0x92, al            ; 0x92c

    ; 加载gdt
    lgdt [gdt_ptr]          ; 0x92e

    ; CR0第0位置1
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax

    ; 刷新流水线
    jmp dword SELECTOR_CODE: p_mode_start
;----------------------------------------------------------
[bits 32]
p_mode_start:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov esp, LOADER_STACK_TOP

    mov ax, SELECTOR_VIDEO
    mov gs, ax


;;;;;;;;;;;;;;;;;;;;;;;; 加载内核 ;;;;;;;;;;;;;;;;;;;;;
    mov eax, KERNEL_START_SECTOR       ; eax = 扇区号
    mov ebx, KERNEL_BIN_BASE_ADDR      ; ebx = 将磁盘读入到内存指定地址, 0x70000
    mov ecx, 200                       ; ecx 读取磁盘扇区数

    call rd_disk_m_32

;;;;;;;;;;;;;;;;;;;;;;; 开启内存分页 ;;;;;;;;;;;;;;;;;;
    call setup_page     
    
    sgdt [gdt_ptr]

    mov ebx, [gdt_ptr + 2]      ; 得到GDT地址
    or dword [ebx + 0x18 + 4], 0xc0000000   ; ebx + 0x18 VIDEO段描述符

    add dword [gdt_ptr + 2], 0xc0000000

    add esp, 0xc0000000         ; 将栈指针同样映射到内核地址

    ; 把页目录地址赋值给cr3
    mov eax, PAGE_DIR_TABLE_POS
    mov cr3, eax

    ; 打开CR0的PG位
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; 在开启分页后, 重新加载gdt
    lgdt [gdt_ptr]

;;;;;;;;;;;;;;;;;;;;;;; 内核初始化 ;;;;;;;;;;;;;;;;;;;;;
jmp SELECTOR_CODE:enter_kernel
enter_kernel:
    call kernel_init

    mov esp, 0xc009f000
    jmp KERNEL_ENTRY_POINT  ; 内核代码段地址, 0xc0001500
    
;------------------------------------------
; kernel_init 
; 功能: 内核初始化
; 参数: 无
;------------------------------------------
kernel_init:
    xor eax, eax
    xor ebx, ebx    ; ebx 将用来记录每一个(遍历)程序头表地址
    xor ecx, ecx    ; ecx 将用来记录程序头表中的program header数量
    xor edx, edx    ; edx 将用来记录program header尺寸,即e_phentsize

    mov dx, [KERNEL_BIN_BASE_ADDR+42]  ; 偏移42字节是e_phentsize,表示每个program header的大小
    mov ebx, [KERNEL_BIN_BASE_ADDR+28] ; 偏移28字节是e_phoff,表示第一个program header偏移量
    add ebx, KERNEL_BIN_BASE_ADDR        ; ebx值为第一个program header偏移量
    mov cx, [KERNEL_BIN_BASE_ADDR+44]   ; cx 表示程序头表的数目
    .each_segment:
        cmp byte [ebx + 0], PT_NULL     ; p_type=PT_NULL, 忽略 
        je .PTNULL  
        
        ; 为 mem_cpy(dst, src, size) 函数调用压入参数
        push dword [ebx + 16]   ; [ebx+16] = p_filesize, 本段在文件中的大小
        mov eax, [ebx + 4]      ; [ebx+4] = p_offset, 本段在文件内的偏移量
        add eax, KERNEL_BIN_BASE_ADDR   ; eax 表示本程序段的起始地址
        push eax
        push dword [ebx + 8]    ; [ebx+8] 本段的在内存中的起始虚拟地址

        call mem_cpy
        add esp, 12
        
        .PTNULL:
        add ebx, edx    ; 设置ebx为下一个程序头表的位置 
        loop .each_segment
        
    ret


;---------------------------------------------
; mem_cpy(dst, src, size)
; 功能: 逐字节拷贝
; 参数: [ebp(esp)+8]  dst
;       [ebp(esp)+12] src
;       [ecx(esp)+16] size
;----------------------------------------------
mem_cpy:
    cld
    push ebp
    mov ebp, esp

    push ecx    ; rep指令会用到ecx, 所以将其备份

    mov edi, [ebp+8]    ; dst
    mov esi, [ebp+12]   ; src
    mov ecx, [ebp+16]   ; size
    rep movsb       
    
    pop ecx
    pop ebp
    ret
    
    
;----------------------------------------------------------
; setup_page
; 功能: 创建页目录和页表
; 参数: 无
;----------------------------------------------------------
setup_page:
    ; 先把页目录占用的空间逐字节清0
    mov ecx, 4096
    mov esi, 0
    .clear_page_dir:
        mov byte [PAGE_DIR_TABLE_POS + esi], 0
        inc esi
        loop .clear_page_dir

    ; 开始创建页目录项
    .create_pde:
    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x1000     ; eax = 0x100000 + 0x1000 = 0x00101000 
    mov ebx, eax        ; ebx = 0x00101000

    or eax, PG_US_U | PG_RW_W | PG_P     ; eax = 0x00101007
    mov [PAGE_DIR_TABLE_POS + 0x0], eax  ;       
    mov [PAGE_DIR_TABLE_POS + 0xc00], eax

    sub eax, 0x1000
    mov [PAGE_DIR_TABLE_POS + 4092], eax

    ; 下面创建页表项
    mov ecx, 256        ; 1M/4K=256
    mov esi, 0
    mov edx, PG_US_U | PG_RW_W | PG_P   ; edx=0x00000007
    
    .create_pte:
        mov [ebx + esi*4], edx

        add edx, 4096
        inc esi
        loop .create_pte

    ; 创建内核(高1G, 768-1023)页目录项
    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x2000     ; eax = 0x00100000 + 0x2000 = 0x00102000
    or eax, PG_US_U | PG_RW_W | PG_P       ; eax = 0x00102007

    mov ebx, PAGE_DIR_TABLE_POS     ; ebx = 0x00100000
    mov ecx, 254
    mov esi, 769
    .create_kernel_pde:
        mov [ebx+esi*4], eax
        inc esi, 
        add eax, 0x1000
        loop .create_kernel_pde

    ret


;----------------------------------------------------------
; rd_disk_m_32
; 功能: 读取硬盘n个扇区内容到内存
; 参数: eax: LBA扇区号
;       ebx: 将数据读入到内存地址
;       ecx: 读如的扇区数
;----------------------------------------------------------
rd_disk_m_32:
    mov esi, eax    ; 备份eax
    mov di, cx      ; 备份扇区数到di

    ; 设置要读取的扇区数
    mov dx, 0x1f2
    mov al, cl
    out dx, al
    
    mov eax, esi
    
    ; 将LBA地址存入0x1f3 - 0x1f6
    mov dx, 0x1f3
    out dx, al      ; LBA 7-0位写入端口0x1f3

    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al      ; LBA 15-8位写入端口0x1f4

    shr eax, cl
    mov dx, 0x1f5
    out dx, al      ; LBA 23-16位写入端口0x1f5

    shr eax, cl
    and al, 0x0f    ; LBA 第24-27位
    or al, 0xe0     ; 设置7-4位为1110, 表示LBA模式
    mov dx, 0x1f6
    out dx, al

    ; 向0x1f7端口写入读命令
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al
    
    ; 检测硬盘状态
    .not_ready:
        nop
        in al, dx
        and al, 0x88
        cmp al, 0x08
        jnz .not_ready

     ; 从 0x1f0端口读取数据
     mov ax, di
     mov dx, 256
     mul dx
     mov cx, ax
     mov dx, 0x1f0
     .go_on_read:
        in ax, dx
        mov [ebx], ax
        add ebx, 2
        loop .go_on_read

    ret




