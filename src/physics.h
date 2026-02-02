#pragma once
#include "../include/linalg.h"

//  Trying new comment style
//  Satellite motions


//https://medium.com/intuition/dont-trust-runge-kutta-blindly-be392663fbe4

struct Earth {
    float radius = 6378137.0;      //  m
    float mu = 3.986004418e14;     //  G*M
    float J2 = 1.08262668e-3;      //  oblateness coeff
    float rotation_rate = 7.2921159e-5;    // rad/s
    float tilt = 23.5;
};

struct Sun {
    float mu = 1.3275183e+20;
};

//  loosely based on NOAA 19 
class Phys {
public:
    float deltaTime = 10;
    Earth planet;
    Sun home;
    //  Earth Centered Inertial, Cartesian coordinates pulled from GMAT
    linalg::vec<float,3> POS = {1799.209 * 1000, -3960.856 * 1000, 5797.431 * 1000};    //  m
    linalg::vec<float,3> VEL = {-15.08207 * 1000, 4.3215701 * 1000, 4.413379 * 1000};    //  m/s
    linalg::vec<float,3> T_ACC = {0,0,0}; //Thruster acceleration

    //Sun position in ECI frame
    linalg::vec<float,3> SUN = {146.9 * 1000 * 1000, 0, 0};

    

    //  Mass
    float dry_mass = 1479;        //  kg
    float STAR_37_XFB_FUEL = 924; // kg   @ launch 
    float HYDRAZINE_FUEL = 27.44;   // kg  @ launch
    float COLD_GAS_FUEL = 4.17;   //kg
    float TOTAL_MASS;
    //double wet_mass =  753;     //  kg


    //  Drag
    float cross_section_meters = 5.23;     //  used for drag calcs m^2
    float Cd, Cr;  

    //  Step forward
    void step();

    void kick_motor(float newtons);

    float solar_panel_directivity(linalg::vec<float,3> local_panel_normal);
private:
    void leap_frog();
    linalg::vec<float,3> grav_f();
};