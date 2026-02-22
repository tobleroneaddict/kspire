#pragma once
#include "../globals.h"

class Timewarp_Controller {
public:
    float warp_rate = 1;
    
    bool entered_rails = false;
    bool exited_rails = false;
    
    bool is_physics_warp = true;

    //Tick the timewarp controller.
    void tick(int cannot_enter_timewarp);
    void lerp_rate(float dt);
    void exit_now();

private:
    bool up_pressed = false;
    bool down_pressed = false;

    //Where in warp rates are we
    int warp_index;

    float target_warp_rate = 1;

    //Standard warp rates
    //For calc (short for calculator) simplicity,physwarp and warp is combined.
    std::vector<float> rates = {
        1,2,3,4,    //PHYS
        5,10,50,100,1000, //RAILS
        10000,100000
    };
    
};