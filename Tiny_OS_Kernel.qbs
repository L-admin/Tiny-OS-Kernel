import qbs

Project {
    minimumQbsVersion: "1.7.1"

    CppApplication {
        consoleApplication: true

        files: [
            "device/timer.h",
            "device/timer.c",
            "device/console.c",
            "device/console.h",
            "device/ide.c",
            "device/ide.h",
            "device/keyboard.c",
            "device/keyboard.h",

            "makefile",

            "boot/include/boot.inc",
            "boot/loader.S",
            "boot/mbr.S",


            "kernel/debug.c",
            "kernel/debug.h",
            "kernel/global.h",
            "kernel/init.c",
            "kernel/init.h",
            "kernel/interrupt.c",
            "kernel/interrupt.h",
            "kernel/kernel.S",
            "kernel/main.c",
            "kernel/memory.c",
            "kernel/memory.h",


            "lib/kernel/list.c",
            "lib/kernel/list.h",
            "lib/kernel/bitmap.c",
            "lib/kernel/bitmap.h",
            "lib/kernel/io.h",
            "lib/kernel/print.h",
            "lib/kernel/print.S",

            "lib/user/syscall.h",
            "lib/user/syscall.c",

            "lib/stdint.h",
            "lib/string.c",
            "lib/string.h",


            "thread/switch.S",
            "thread/sync.c",
            "thread/sync.h",
            "thread/thread.h",
            "thread/thread.c",





            "userprog/process.c",
            "userprog/process.h",
            "userprog/tss.h",
            "userprog/tss.c",
        ]
        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true        
        }
    }
}
