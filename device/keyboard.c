#include "keyboard.h"


static void intr_keyboard_hadnler();


/*
 * 键盘中断处理程序, 目前只处理主键盘
 */
static void intr_keyboard_handler()
{
    /* 这次中断发生前的上一次中断，以下任意3个键是否有按下 */
    bool ctrl_down_last = ctrl_status;
    bool shift_down_last = shift_status;
    bool caps_lock_last = caps_lock_status;

    bool break_code;
    uint16_t scancode = inb(KBD_BUF_PORT);  // 记录这次中断来的扫描码

    /* 若扫描码是e0开头的，表示扫描码前缀。<R-alt>(e0,38) ,<R-ctrl>(e0,1d) */
    /* 所以结束中断处理函数，等待下一个扫描码进来 */
    if (scancode == 0xe0)
    {
        ext_scancode = true;
        return;
    }

    /* 如果上次是0xe0开头，将扫描码合并 */
    if (ext_scancode)
    {
        scancode = ((0xe000) | scancode);
        ext_scancode = false;   // 关闭 e0 标价
    }


    break_code = ((scancode & 0x0080) != 0);   // 断码和通码的区别就是扫描码第8位的值，断码第8位为1，通码为0

    if (break_code)     // 若为断码
    {
        uint16_t make_code = (scancode &= 0xff7f);   // 得到其通码

        if (make_code == ctrl_l_make || make_code == ctrl_r_make)
            ctrl_status = false;
        else if (make_code == shift_l_make || make_code == shift_r_make)
            shift_statu = false;
        else if (make_code == alt_l_make || make_code == alt_r_make)
            alt_status = false;

        return;
    }
    // 若为通码，只处理keymap中定义的和 alt_right 和 ctrl 键
    else if ( (scancode > 0x00 && scancode < 0x3b) ||
              (scancode == alt_r_make) ||
               (scancode == ctrl_r_make) )
    {
        bool shift = false;
        if ( (scancode < 0x0e) || (scancode == 0x29) ||
             (scancode == 0x1a) || (scancode == 0x1b) ||
             (scancode == 0x2b) || (scancode == 0x27) ||
             (scancode == 0x28) || (scancode == 0x33) ||
             (scancode == 0x34) || (scancode == 0x35)    )
        {
            if (shift_down_last)
                shift = true;
        }
        else   // 默认为字母
        {
            if (shift_down_last && !caps_lock_last)     // shift 按下, caps没有按下
                shift = true;
            else if (shift_down_last && caps_lock_last) // shift 按下，caps按下
                shift = false;
            else if (!shift_down_last || caps_lock_last)    // shift 没按下。caps按下
                shift = true;
        }


        uint8_t index = (scancode &= 0x00ff);    // 将扫描码前缀置0
        char cur_char = keymap[index][shift];

        if (cur_char)   // 是可以打印的字符
        {
            put_char(cur_char);
            return;
        }

        /* 记录本次是否按下了下面几类控制键之一,供下次键入时判断组合键 */
        if (scancode == ctrl_l_make || scancode == ctrl_r_make)
            ctrl_status = true;
        else if (scancode == shift_l_make || scancode == shift_r_make)
            shift_status = true;
        else if (scancode == alt_l_make || scancode == alt_r_make)
            alt_status = true;
        else if (scancode == caps_lock_make)
            caps_lock_status = !caps_lock_status;   // 不管之前是否有按下caps_lock键,当再次按下时则状态取反,
    }
    else
    {
        put_str("unknow key\n");
    }

}

void keyboard_init()
{
    put_str("keyboard init start\n");
    register_handler(0x21, intr_keyboard_handler);
    put_str("keyboard init done\n");
}
