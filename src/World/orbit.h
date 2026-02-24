#pragma once
#include "../globals.h"


//On rails orbit things
struct Orbit {

    //ON RAILS
    double ApA; //Apopsis Altitude, to surface
    double ApR; //Apopsis Radius, to center
    double PeA; //Periapsis Altitude, to surface
    double PeR; //Periapsis Altitude, to center
    double semi_minor_axis;
    double semi_latus_rectum;


    //STO
    double semi_major_axis = 0;
    double eccentricity = 0;
    double epoch = 0;
    double inclination = 0;
    double mean_anomaly = 0;
    double mean_anomaly_at_epoch = 0;
    double long_ascending_node = 0;
    double argument_of_periapsis = 0;
    
    double period = 0;
    double orbital_speed = 0;
    double mu = 0;  //transfer mu from parent into here!!

    //TESTING: Set vessel starting point. 

    //EQUI REFERENCE FRAME TO PARENT BODY
    linalg::vec<double,3> POS = {10 * 1000, 600+1500 * 1000, 0 * 1000};    //  m
    linalg::vec<double,3> VEL = {-0 * 1000, 0.01 * 1000, 1.51 * 1000};    //  m/s

    //Calculate keplarian elements based on input universal time
    void calculate_state_from_keplers(double _UNIVERSAL_TIME);

    //Used for vessels
    void physics_step(float sdl_dt, float phys_warp_rate);

    void physics_to_rails(double epoch);

private:
    const double pi = 3.14159265; 
    double universal_time;


    double Ecc_A(double M, double ecc, double max_error);
    //double solveEccentricAnomalyExtremeEcc(double M, double ecc, int iterations = 8);
    double Ecc_A_Hyp(double M, double ecc, double max_error);
    double Ecc_A_Extreme(double M, double ecc, double max_error, int max_it);
    void leap_frog(float sdl_dt, float phys_warp_rate);
    linalg::vec<double,3> grav_f();


    double h_mag = 0;

};