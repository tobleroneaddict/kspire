#include "globals.h"

#ifndef _TINSPIRE
SDL_Event sdl_event;


bool isKeyPressed(unsigned int key) {
    Uint8 *state = SDL_GetKeyState(NULL);
    return state[key];
    return false;
}

touchpad_report_t touchpad_scan(touchpad_report_t *tp) {
    
    int mx,my;
    Uint8 press = SDL_GetMouseState(&mx,&my);

    tp->pressed = false;
    tp->proximity = 0;
    tp->contact = false;
    if (press != 0) {
    tp->pressed = true; 
    tp->contact = true;
    tp->proximity = 70;
    }
    tp->x = mx; tp->y = 240-my;
    tp->arrow = 0;
    
    tp->dummy = 0;
    
    tp->x_velocity = 0;
    tp->y_velocity = 0;
    
    return *tp;
}



#endif

KSPIRE_Touchpad::KSPIRE_Touchpad() {
    auto tp_i = touchpad_getinfo();
    h = (float)tp_i->height;
    w = (float)tp_i->width;
}
void KSPIRE_Touchpad::Update() {
    touchpad_report_t touchpad;
    touchpad_scan(&touchpad);
    arrow      =      touchpad.arrow;
    contact    =    touchpad.contact;
    dummy      =      touchpad.dummy;
    pressed    =    touchpad.pressed;
    proximity  =  touchpad.proximity;
    x          =          touchpad.x;
    x_velocity = touchpad.x_velocity;
    y          =          touchpad.y;
    y_velocity = touchpad.y_velocity;
    true_contact = (touchpad.proximity >= CONTACT_THRESHOLD) ? true : false;
    
    if (!relative_mode) {
        x_screen_normalized = ((float)x / (float)w);
        y_screen_normalized = ((float)y / (float)h);
        x_screen = x_screen_normalized * (float)SCREEN_WIDTH;
        y_screen = y_screen_normalized * (float)SCREEN_HEIGHT;
    } else if (relative_mode) {
        auto goodpow = [](float num, float pow) {
            return (num < 0 ? -1.0f : 1.0f) * linalg::pow(fabs(num), pow);
        };

        float yvsigned = (y_velocity > 127) ? y_velocity - 256 : y_velocity;
        float xvsigned = (x_velocity > 127) ? x_velocity - 256 : x_velocity;

        xvsigned = goodpow(xvsigned,MOUSE_ACCELERATION);
        yvsigned = goodpow(yvsigned,MOUSE_ACCELERATION);

        y_screen += yvsigned / 2;
        x_screen += xvsigned / 2;
        
        
        
        if (x_screen < 0) x_screen = 0;
        if (x_screen > SCREEN_WIDTH) x_screen = SCREEN_WIDTH;
        if (y_screen < 0) y_screen = 0;
        if (y_screen > SCREEN_HEIGHT) y_screen = SCREEN_HEIGHT;

        x_screen_normalized = ((float)x_screen / SCREEN_WIDTH);
        y_screen_normalized = ((float)y_screen / SCREEN_HEIGHT);


        #if defined(KSPIRE_PLATFORM_LINUX)
        //Hacky fix
        x_screen = x;
        y_screen = y;
        x_screen_normalized = (float)x / 320.0f;
        y_screen_normalized = (float)y / 320.0f;
        

        printf("YS:%d\n",y_screen);

        #endif
    }

    
}

KSPIRE_Touchpad kspire_pad;