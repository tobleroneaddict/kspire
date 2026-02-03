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

    //Physics range things
    Flight flight;
    Camera cam;
    
    Vessel* focused_vessel;

    //Angel Asset Loading
    int load_bundles();
    void free_bundles();


    void step();


    ProcessedPosition *processed;

    private:
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
    void step_on_rails_orbits();
    //Step physics orbits
    void step_physics_orbits();

    void render();

    Bundle planet_bundle;

};