#pragma once
#include "../../include/linalg.h"
#include "../globals.h"
#include "CelestialBody.h"
#include "../Vessel/Vessel.h"


//Holds all the planets
class Planetarium {
public:
    std::vector<CelestialBody> celestials;

    ProcessedPosition *processed;

    Vessel *focused_vessel;

    int load_celestial_bodies(Bundle* resources);
    //Mode 0 : Distant, up to LO. Mode 1: PQS
    //Calculated inside.
    void render_celestials();

    int find_body_by_name(std::string name);
    

    double get_soi(int index);

    int get_attractor(Vessel *v);

    void update_planet_positions(double universal_time);

    //Returns a position in the space of any nested body to universal space
    linalg::vec<double,3> planet_to_universe(linalg::vec<double,3> local, int home);
};