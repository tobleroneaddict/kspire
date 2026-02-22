#include "timewarp_controller.h"

void Timewarp_Controller::lerp_rate(float dt) {
    warp_rate = linalg::lerp(warp_rate,target_warp_rate + 0.001,dt*3);
}
void Timewarp_Controller::exit_now() {
    if (warp_index > 4) exited_rails = true;
    warp_rate = 1;
    target_warp_rate = 1;
    warp_index = 0;
}
void Timewarp_Controller::tick(int cannot_enter_timewarp) {
    //Read key states
    bool up_oneshot = false;
    bool down_oneshot = false;

    entered_rails = false;
    exited_rails = false;

    //Tw UP
    if (isKeyPressed(K_WARP_UP) && !up_pressed) {
        up_pressed = true; up_oneshot = true;}
    if (!isKeyPressed(K_WARP_UP)) {
        up_pressed = false; }
    //Tw DOWN
    if (isKeyPressed(K_WARP_DOWN) && !down_pressed) {
        down_pressed = true; down_oneshot = true;}
    if (!isKeyPressed(K_WARP_DOWN)) {
        down_pressed = false; }


    //Cancel out
    if (up_oneshot && down_oneshot) {
        up_oneshot = false;
        down_oneshot = false;
    }

    //Key up/down
    if (up_oneshot) {
        warp_index++;
    }
    if (down_oneshot) {
        warp_index--;
    }

    //Restrict to phys if asked
    if (warp_index > 3 && cannot_enter_timewarp)
        warp_index = 3;

    //Set status
    if (warp_rate > 4.2) {
        is_physics_warp = false;
    } else {
        is_physics_warp = true;
    }

    //Flag!
    if (up_oneshot && warp_index == 4) entered_rails = true;
    if (down_oneshot && warp_index == 3) exited_rails = true;

    //Clamp
    if (warp_index < 0) warp_index = 0;
    if ((unsigned int)warp_index >= rates.size()) warp_index = rates.size() -1;

    target_warp_rate = rates[warp_index];
}