#include "physics.h"



void Phys::step(float sdl_dt , float phys_warp_rate) {
    //Add mass
    TOTAL_MASS = dry_mass + STAR_37_XFB_FUEL + HYDRAZINE_FUEL + COLD_GAS_FUEL;
    
    //test clamper

    //Run VEL
    leap_frog(sdl_dt, phys_warp_rate);
    
    //Zero acc before stepping next
    T_ACC = linalg::vec<float,3> { 0,0,0 };
}



// TODO: add oblateness
//  Calculate gravity force
linalg::vec<float,3> Phys::grav_f() {
    float r2, r3;

    r2 = dot(POS,POS); //distance**
    r3 = r2 * sqrt(r2);
    linalg::vec<float,3> pull_earth;
    pull_earth = -planet.mu * POS / r3;
    
    return pull_earth;
}

//  Step forward
void Phys::leap_frog(float sdl_dt, float phys_warp_rate) {

    //Accumulation maybe?
    for (int phys_step_count = 0; phys_step_count < phys_warp_rate; phys_step_count++) {
        linalg::vec<float,3> acc_curr;
        float ddt = (float) sdl_dt;

        //Current time sep acceleration
        acc_curr = grav_f(); 
        VEL += (float)0.5 * ddt* acc_curr; //Kick
        POS += ddt*VEL; //Drift
        acc_curr = grav_f(); //New Kick
        //Propagate
        VEL += (float)0.5 * ddt* acc_curr;
    }
    return;
}

