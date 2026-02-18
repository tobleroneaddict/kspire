#include "cursor.h"

void Cursor::set_cursor_visibility(bool _switch) {
    volatile int* cur_ctl = (volatile int*)PL111_cursor_ctl;
    cur_ctl[0] = _switch;
}
void Cursor::set_cursor_position(int _x, int _y) {
    volatile int* cur_pos = (volatile int*)PL111_cursor_pos;

    uint32_t val = *cur_pos;
    val &= ~0x3FF;
    val |= (_y & 0x3FF);
    val &= ~((0xFFF)<< 16);
    val |= ((_x & 0x3FF) << 16);
    *cur_pos = val;
}