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

    double eccentricity;
    double epoch;
    double period;
    double orbital_speed;
    double inclination;
    double mean_anomaly;
    double mean_anomaly_at_epoch;
    double long_ascending_node;
    double argument_of_periapsis;
    
    double mu = 0;  //Propogate mu from parent into here!!

    void sim();
};