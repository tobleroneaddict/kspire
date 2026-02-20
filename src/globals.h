#pragma once
#include "../include/angel/angel.h"
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#ifdef KSPIRE_PLATFORM_NSPIRE
#include <os.h>
#endif
#include "../include/linalg.h"
#include <memory>

#define CONTACT_THRESHOLD  47
#define MOUSE_ACCELERATION 1.23f

using namespace::angel;



#ifndef _TINSPIRE
#define is_touchpad true

#define KEY_NSPIRE_ESC 0
#define KEY_NSPIRE_W 1
#define KEY_NSPIRE_A 2
#define KEY_NSPIRE_S 3
#define KEY_NSPIRE_D 4
#define KEY_NSPIRE_1 5
#define KEY_NSPIRE_2 6
#define KEY_NSPIRE_ENTER 7
#define KEY_NSPIRE_Q 2
#define KEY_NSPIRE_E 3
#define KEY_NSPIRE_CTRL 4
#define KEY_NSPIRE_SHIFT 5
#define KEY_NSPIRE_TAB 6
#define KEY_NSPIRE_Z 2
#define KEY_NSPIRE_X 3
#define KEY_NSPIRE_F 4
#define KEY_NSPIRE_R 5
#define KEY_NSPIRE_SCRATCHPAD 6
#define KEY_NSPIRE_LEFT 4
#define KEY_NSPIRE_DOWN 5
#define KEY_NSPIRE_UP 4
#define KEY_NSPIRE_RIGHT 5

#define KEY_NSPIRE_DOWNLEFT 4
#define KEY_NSPIRE_RIGHTDOWN 5
#define KEY_NSPIRE_LEFTUP 4
#define KEY_NSPIRE_UPRIGHT 5


//Testing
inline bool isKeyPressed(unsigned int key) {
    return false;
}


struct touchpad_info {
    uint16_t height;
    uint16_t width;
};
static touchpad_info tp_thing;

inline touchpad_info* touchpad_getinfo() {
    tp_thing.height = 240;
    tp_thing.width = 320;
    return &tp_thing;
}

struct touchpad_report_t {
    bool pressed;
    bool contact;
    uint16_t x;
    uint16_t y;
    uint16_t x_velocity;
    uint16_t y_velocity;
    uint16_t proximity;
    uint16_t dummy;
    uint16_t arrow;
    

};



inline touchpad_report_t touchpad_scan(touchpad_report_t *tp) {
    tp->pressed = false;
    tp->arrow = 0;
    tp->contact = false;
    tp->dummy = 0;
    tp->proximity = 0;
    tp->x = 4;
    tp->x_velocity = 4;
    tp->y = 4;
    tp->y_velocity = 4;
    return *tp;
}


#endif

//Read from this instead.
struct KSPIRE_Touchpad : touchpad_report_t {
    //SET
    bool relative_mode = true;
    //GET
    bool true_contact;
    int w;
    int h;
    int x_screen;
    int y_screen;
    float x_screen_normalized;
    float y_screen_normalized;
    KSPIRE_Touchpad();
    void Update();

    
};



struct DT {
    float dt = 0;
    float dt_last = 0;

	void init() {
		dt_last = SDL_GetTicks() * 0.001f;
	}
    void tick() {
        float dt_now = SDL_GetTicks() * 0.001f;
        dt = dt_now - dt_last;
        dt_last = dt_now;
    }
};



extern KSPIRE_Touchpad kspire_pad;