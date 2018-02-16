#include "ide.h"

void ide_init()
{
    console_put_str("ide_init start\n");    // todo
    uint8_t hd_cnt = *((uint8_t*)(0x475));

    ASSERT(hd_cnt > 0);

    channel_cnt = DIV_ROUND_UP(hd_cnt, 2);  // 硬盘数乘以2便是通道数

    uint8_t channel_no = 0;
    struct ide_channel* channel;
    while(channel_no < channel_cnt)
    {
        channel = &channels[channel_no];

        // todo


        switch (channel_no) {
        case 0:
            channel->port_base = 0x1f0;
            channel->irq_no = 0x20 + 14;
            break;
        case 1:
            channel->port_base = 0x170;
            channel->irq_no = 0x20 + 15;
            break;
        default:
            break;
        }

        channel->expecting_intr = false;
        lock_init(&(channel->lock));
        sema_init(&channel->disk_done, 0);

        channel_no++;
    }

    console_put_str("ide_init done\n"); // todo
}
