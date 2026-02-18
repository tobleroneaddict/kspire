#pragma once
#include "../include/angel/angel.h"
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <os.h>
#include "../include/linalg.h"
#include <memory>

#define CONTACT_THRESHOLD  47

using namespace::angel;

//Read from this instead.
struct KSPIRE_Touchpad : touchpad_report_t {
    //SET
    bool relative_mode = true;
    float dpi = 1;

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