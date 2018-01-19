;主引导程序
;---------------------------

%include "boot.inc"
SECTION MBR vstart=0x7c00	; 实模式下BIOS将MBR加载到0x7c00
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov sp, 0x7c00
    mov ax, 0xb800
    mov gs, ax

;-------------------------------------
; int 0x10 功能号: 0x06 功能描述: 清屏
; 输入:
; ah=0x06
; al = 上卷的行数
; bh = 上卷行的属性
; (cl, ch) = 窗口左上角位置
; (dl, dh) = 窗口右下角位置
; 无返回值
;--------------------------------------
mov ax, 0600h
mov bx, 0700h
mov cx, 0           ; 左上角: (0, 0)
mov dx, 184fh       ; 右下角: (80, 25)

int 10h

; 输出字符串"MBR"
mov byte [gs:0x00], '4'
mov byte [gs:0x01], 0xA4

mov byte [gs:0x02], ' '
mov byte [gs:0x03], 0xA4

mov byte [gs:0x04], 'M'
mov byte [gs:0x05], 0xA4

mov byte [gs:0x06], 'B'
mov byte [gs:0x07], 0xA4

mov byte [gs:0x08], 'R'
mov byte [gs:0x09], 0xA4


mov eax, LOADER_START_SECTOR    ; 参数: 起始扇区lba地址
mov bx, LOADER_BASE_ADDR        ; 参数: 写入的地址
mov cx, 4                       ; 参数: 待读入的扇区数
call rd_disk_m_16


jmp LOADER_BASE_ADDR + 0x300            ; 跳到内核加载器代码部分

;----------------------
;功能: 读取硬盘n个扇区
;----------------------
rd_disk_m_16:
    mov esi, eax
    mov di, cx

; 读写硬盘
; 第一步: 设置要读取的扇区数量
    mov dx, 0x1f2   ; dx存储端口号
    mov al, cl
    out dx, al

    mov eax, esi
; 第二步: 将LBA地址存入0x1f3 - 0x1f6
    ; LBA地址7-0位写入端口
    mov dx, 0x1f3
    out dx, al

    ; LBA地址15-8位写入端口
    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al

    ; LBA地址23-16位写入端口
    shr eax, cl
    mov dx, 0x1f5
    out dx, al

    ; 设置device寄存器, 0-3位表示LBA第24-27位, 1110设置LBA模式
    shr eax, cl
    and al, 0x0f
    or al, 0xe0
    mov dx, 0x1f6
    out dx, al

; 第三步: 向0x1f7端口写入读命令
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

; 第四步: 检测硬盘状态
 .not_ready:
    nop
    in al, dx
    and al, 0x88

    cmp al, 0x08
    jnz .not_ready

; 第五步: 从0x1f0端口读数据
    mov ax, di
    mov dx, 256
    mul dx
    mov cx, ax
    
    mov dx, 0x1f0

 .go_on_read:
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .go_on_read
    ret

    times 510 - ($-$$) db 0
    db 0x55, 0xaa

