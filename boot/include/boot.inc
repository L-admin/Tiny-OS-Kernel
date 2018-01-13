;----------- loader和kernel -----------
LOADER_BASE_ADDR equ 0x900
LOADER_STACK_TOP equ LOADER_BASE_ADDR

LOADER_START_SECTOR equ 0x2     ; kernel loader 所在扇区号

KERNEL_START_SECTOR equ 0x9     ; kernel.bin 所在扇区号
KERNEL_BIN_BASE_ADDR equ 0x70000    ; kernel.bin 存储的内存地址
PT_NULL equ 0
KERNEL_ENTRY_POINT equ 0xc0001500


;;;;;;;;;;;;;;;;;;; gdt描述符属性 ;;;;;;;;;;;;;;;

DESC_G_4K equ 1_00000000000000000000000b       ; G位 段描述符第23位


DESC_D_32 equ  1_0000000000000000000000b       ; D/B 段描述符第22位


; 64位代码标记,为0表示32位代码
DESC_L    equ   0_000000000000000000000b    ; 段描述符第21位


; 没有专门用途
DESC_AVL  equ    0_00000000000000000000b       ; 段描述符第20位


; 段界限值
DESC_LIMIT_CODE2    equ 1111_0000000000000000b  ; 段描述符第16-19位
DESC_LIMIT_DATA2    equ DESC_LIMIT_CODE2 
DESC_LIMIT_VIDEO2   equ 0000_0000000000000000b


; 检测此段是否在内存中
DESC_P      equ 1_000000000000000b             ; 段描述符第15位


; ---描述符特权级, 0, 1, 2, 3共４个特权级---
DESC_DPL_0  equ  00_0000000000000b             ; 段描述符第13-14位
DESC_DPL_1  equ  01_0000000000000b
DESC_DPL_2  equ  10_0000000000000b
DESC_DPL_3  equ  11_0000000000000b
; ------------------------------------------


; ---配合TYPE字段,0表示系统段, 1表示数据段--
DESC_S_CODE equ    1_000000000000b             ; S字段,段描述符第12位 
DESC_S_DATA equ    DESC_S_CODE                 
DESC_S_sys  equ    0_000000000000b 
; -------------------------------------------


; --------------- TYPE字段 -------------------
DESC_TYPE_CODE equ  1000_00000000b  ; x=1,c=0,r=0,a=0 代码段可执行,非一致性,不可读,已访问位清0       
DESC_TYPE_DATA equ  0010_00000000b  ; x=0,e=0,w=1,a=0 数据段不可执行,向上扩展,可写,已访问位清0
; --------------------------------------------


DESC_CODE_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + \
                    DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + \
                    DESC_P + DESC_DPL_0 + DESC_S_CODE + \
                    DESC_TYPE_CODE + 0x00

                   
DESC_DATA_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + \
                    DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + \
                    DESC_P + DESC_DPL_0 + DESC_S_DATA + \
                    DESC_TYPE_DATA + 0x00


DESC_VIDEO_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + \
                    DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + \
                    DESC_P + DESC_DPL_0 + DESC_S_DATA + \
                    DESC_TYPE_DATA + 0x0b

; ---------------- 选择子属性 -----------------
RPL0 equ 00b
RPL1 equ 01b
RPL2 equ 10b
RPL3 equ 11b

TI_GDT equ 000b
TI_LDT equ 100b


;;;;;;;;;;;;;;;;;;;;;;;; 分页相关 ;;;;;;;;;;;;;;;;;;;;
; ----------------------------------------------------
PAGE_DIR_TABLE_POS equ 0x100000
; ----------------------------------------------------
PG_P equ 1b
PG_RW_R equ 00b
PG_RW_W equ 10b
PG_US_S equ 000b
PG_US_U equ 100b


