#pragma once

#ifdef KSPIRE_PLATFORM_WINDOWS
#define KSPIRE_PLATFORM_LINUX
#include <SDL_main.h>
#endif


#include "../include/angel/angel.h"
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#ifdef KSPIRE_PLATFORM_NSPIRE
#include <os.h>
#endif
#include "../include/linalg.h"
#include <memory>
#include "Utility/xplatkeybinds.h"


//Configuration
#define CONTACT_THRESHOLD  47
#define MOUSE_ACCELERATION 1.23f

//Version
#ifndef BUILD_DATE
#define BUILD_DATE "0"
#endif

#ifndef BUILD_TIME
#define BUILD_TIME "0"
#endif


using namespace::angel;

extern bool is_firebird;
//PC Substitutions for TI things
#ifndef _TINSPIRE
#define is_touchpad 1
extern SDL_Event sdl_event;


extern bool isKeyPressed(unsigned int key);


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



extern touchpad_report_t touchpad_scan(touchpad_report_t *tp);

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