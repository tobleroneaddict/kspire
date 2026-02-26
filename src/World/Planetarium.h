#pragma once
#include "../../include/linalg.h"
#include "../globals.h"
#include "CelestialBody.h"
#include "../Vessel/Vessel.h"


//Holds all the planets
class Planetarium {
public:
    //If you want to call clear on this that's fine, just regenerate with load_celestial_bodies before attempting
    //to load any more bodies.
    //use clear_celestial_models() to free for loading and stuff
    std::vector<CelestialBody> celestials;

    ProcessedPosition *processed;

    Vessel *focused_vessel;

    int load_celestial_bodies(Bundle* resources);
    void clear_celestial_models();

    //Mode 0 : Distant, up to LO. Mode 1: PQS
    //Calculated inside.
    void render_celestials(float fixed_bubble,bool map_mode, linalg::vec<double,3> cam_pos);

    int find_body_by_name(std::string name);
    

    double get_soi(int index);

    int get_attractor(Vessel *v);

    void update_planet_positions(double universal_time);

    //REAL TIME Lighting will not be possible.
    //Rather it is staggered!
    void update_planet_lighting();
    unsigned int working_body = 0;

    //Returns a position in the space of any nested body to universal space
    linalg::vec<double,3> planet_to_universe(linalg::vec<double,3> local, int home);
};