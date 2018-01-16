BUILD_DIR =./build
ENTRY_POINT = 0Xc001500

AS = nasm
CC = gcc
LD = ld

LIB = -I lib/   -I lib/kernel   -I lib/user    -I kernel/    -I device/

ASFLAGS = -f elf

CFLAGS = -Wall $(LIB) -m32 -c -fno-builtin -W -Wstrict-prototypes \
         -Wmissing-prototypes

OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o \
       $(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
       $(BUILD_DIR)/debug.o


#############  C代码编译  #############
$(BUILD_DIR)/main.o : kernel/main.c
    $(CC) $(CFLAGS) $< -o build/main.o

$(BUILD_DIR)/init.o : kernel/init.c
    $(CC) $(CFLAGS) $< -o build/main.o

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c
    $(CC) $(CFLAGS) $< -o build/main.o

$(BUILD_DIR)/timer.o: device/timer.c
    $(CC) $(CFLAGS) $< -o build/main.o


#############  汇编代码编译  ###########
$(BUILD_DIR)/kernel.o: kernel/kernel.S
        $(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/print.o: lib/kernel/print.S
        $(AS) $(ASFLAGS) $< -o $@


