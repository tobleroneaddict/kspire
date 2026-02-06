#pragma once
#include "globals.h"
#include "Vessel/Vessel.h"
#include "flight.h"
#include "World/CelestialBody.h"
#include "World/Camera/camera.h"

//Contains all vessels.
class Universe {
    public:

    
    DT clock;
    double universal_time = 0;

    //Physics range things
    Flight flight;
    Camera cam;
    float phys_warp_rate = 1;
    float rails_warp_rate = 1;
    
    Vessel* focused_vessel;


    void step();
    

    ProcessedPosition *processed;

    //All vessels in world. Does not contain part data, until loaded.
    //NTS: I would prefer you to do emplace back. Read up on emplace back.
    std::vector<Vessel> vessels;
    std::vector<CelestialBody> celestials;
    


    //Mode 0 : Distant, up to LO. Mode 1: PQS
    //Calculated inside.
    void render_celestials();
    //Render focused vessel & any surrounding vessels
    void render_nearby_vessels();

    //Step on rails orbit sim
    void step_on_rails_orbits(Vessel* v);
    //Step physics orbits
    void step_physics_orbits(Vessel* v);

    void render();

    Bundle *planet_bundle;
    Bundle *resource_bundle;
    Bundle *parts_bundle;
    
    private:
};