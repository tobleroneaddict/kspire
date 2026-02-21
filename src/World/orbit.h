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

    double semi_major_axis;
    double eccentricity;
    double epoch;
    double period;
    double orbital_speed;
    double inclination;
    double mean_anomaly;
    double mean_anomaly_at_epoch;
    double long_ascending_node;
    double argument_of_periapsis;
    
    double mu = 0;  //transfer mu from parent into here!!

    //TESTING: Set vessel starting point. 

    //EQUI REFERENCE FRAME TO PARENT BODY
    linalg::vec<double,3> POS = {0 * 1000, 0 * 1000, -2250 * 1000};    //  m
    linalg::vec<double,3> VEL = {1 * 1000, 1 * 1000, 1 * 1000};    //  m/s

    //Calculate keplarian elements based on input universal time
    void calculate_state_from_keplers(double _UNIVERSAL_TIME);

    //Used for vessels
    void physics_step(float sdl_dt, float phys_warp_rate);

    void physics_to_rails(double epoch);

private:
    const double pi = 3.14159265; 
    double universal_time;


    double solveEccentricAnomaly(double M, double ecc, double maxError);
    //double solveEccentricAnomalyExtremeEcc(double M, double ecc, int iterations = 8);
    double solveEccentricAnomalyHyp(double M, double ecc, double maxError);

    void leap_frog(float sdl_dt, float phys_warp_rate);
    linalg::vec<double,3> grav_f();

};