#include "physics.h"



void Phys::step() {
    //Add mass
    TOTAL_MASS = dry_mass + STAR_37_XFB_FUEL + HYDRAZINE_FUEL + COLD_GAS_FUEL;
    

    //Run VEL
    leap_frog();
    
    //Zero acc before stepping next
    T_ACC = linalg::vec<float,3> { 0,0,0 };


    //Reset if below earth for testing
    if (sqrt( dot(POS,POS)) < planet.radius) {
        //std::cout << "Crash!\n";
        POS = {1799.209 * 1000, -3960.856 * 1000, 5797.431 * 1000};
        VEL = {-4.08207 * 1000, 4.3215701 * 1000, 4.413379 * 1000};
    }
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
void Phys::leap_frog() {
    linalg::vec<float,3> acc_curr;
    float ddt = (float) deltaTime;

    //Current time sep acceleration
    acc_curr = grav_f(); 
    VEL += (float)0.5 * ddt* acc_curr; //Kick
    POS += ddt*VEL; //Drift
    acc_curr = grav_f(); //New Kick
    //Propagate
    VEL += (float)0.5 * ddt* acc_curr;

    return;
}

