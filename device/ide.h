#ifndef __DEVICE_IDE_H
#define __DEVICE_IDE_H

#include "../lib/stdint.h"
#include "../lib/kernel/bitmap.h"
#include "../thread/sync.h"
#include "../device/console.h"  // todo
#include "../kernel/global.h"


/* 分区结构 */
struct partition
{
    uint32_t start_lba;         // 起始扇区
    uint32_t sec_cnt;           // 扇区数
    struct disk* my_disk;       // 分区所属的硬盘
    struct list_elem part_tag;  // 用于队列中的标记
    char name[8];               // 分区名称
    struct super_block* sb;     // 本分区的超级块
    struct bitmap block_bitmap; // 块位图
    struct bitmap inode_bitmap; // i结点的位图
    struct list open_inodes;    // 本分区打开的i结点队列
};


/* 硬盘结构 */
struct disk
{
    char name[8];                       // 本硬盘的名称，如 sda 等
    struct ide_channel* my_channel;	    // 此块硬盘归属于哪个ide通道
    uint8_t dev_no;                     // 本硬盘是主0还是从1
    struct partition prim_parts[4];	    // 主分区顶多是4个
    struct partition logic_parts[8];	// 逻辑分区数量无限,但总得有个支持的上限,那就支持8个
};


/* ata 通道结构 */
struct ide_channel
{
    char name[8];                // 本ata通道名称
    uint16_t port_base;          // 本通道的起始端口号
    uint8_t irq_no;              // 本通道所用的中断号
    struct lock lock;            // 通道锁
    bool expecting_intr;         // 表示等待硬盘的中断
    struct semaphore disk_done;  // 用于阻塞、唤醒驱动程序
    struct disk devices[2];      // 一个通道上连接两个硬盘，一主一从
};


/* 用于定义硬盘个寄存器端口号 */
#define reg_data(chanel)    (chanel->port_base + 0)
#define reg_data(chanel)    (chanel->port_base + 1)
#define reg_sect_cnt(channel)	 (channel->port_base + 2)
#define reg_lba_l(channel)	 (channel->port_base + 3)
#define reg_lba_m(channel)	 (channel->port_base + 4)
#define reg_lba_h(channel)	 (channel->port_base + 5)
#define reg_dev(channel)	 (channel->port_base + 6)

#define reg_status(channel)	 (channel->port_base + 7)
#define reg_cmd(channel)	 (reg_status(channel))

#define reg_alt_status(channel)  (channel->port_base + 0x206)
#define reg_ctl(channel)	 reg_alt_status(channel)


/* reg_status 寄存器的一些关键位 */
#define BIT_STAT_BSY    0x80    // 1000_0000b  7  BSY(为1表示忙)
#define BIT_STAT_DRDY   0x40    // 0100_0000b  6  DRDY(为1设备就绪，等待指令)
#define BIT_STAT_DRQ    0x8     // 0000_1000b  3  DRQ(为1表示硬盘已经准备好数据，随时可以输出)


/* device 寄存器的一些关键位 */
#define BIT_DEV_MBS 0xa0    // 1010_0000b   7,5 固定为1
#define BIT_DEV_LBA 0x40    // 0100_0000b   6   MOD(寻址模式LBA=1, CHS=0)
#define BIT_DEV_DEV 0x10    // 0001_0000b   4   dev(从盘或者主盘)


/* 一些硬盘操作指令 */
#define CMD_IDENTIFY        0xec    // 1110_1100b   identify指令
#define CMD_READ_SECTORY    0x20    // 0010_0000b   读扇区指令
#define CMD_WRITE_SECTOR    0x30    // 0011_0000b   写扇区指令

/* 定义可读写的最大扇区数 */
#define max_lba ( (80*1024*1024/512) - 1)   // 只支持80MB硬盘


uint8_t channel_cnt;                // 按硬盘数计算的通道数
struct ide_channel  channels[2];    // 有 2 个ide通道。




void ide_init();

#endif // IDE_H
