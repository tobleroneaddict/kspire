#pragma once
#include <os.h>

#define PL111_cursor_ram 0xC0000800
#define PL111_cursor_ctl 0xC0000C00
#define PL111_cursor_cfg 0xC0000C04
#define PL111_cursor_pal 0xC0000C08
#define PL111_cursor_pos 0xC0000C10

struct Cursor {
public:
    void set_cursor_visibility(bool _switch);
    void set_cursor_position(int _x, int _y);
};