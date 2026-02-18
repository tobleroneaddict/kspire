#include "globals.h"
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
        y_screen = x_screen_normalized * (float)SCREEN_HEIGHT;
    }

    if (relative_mode && true_contact) {

        printf("CONTACT!\n");
    }

    
}

KSPIRE_Touchpad kspire_pad;