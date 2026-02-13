#pragma once
#include "globals.h"
#include "Vessel/Vessel.h"
#include "flight.h"
#include "World/CelestialBody.h"
#include "World/Camera/camera.h"
#include "Utility/timewarp_controller.h"
#include "World/Planetarium.h"

//Contains all vessels.
class Universe {
    public:

    
    DT clock;
    double universal_time = 0;

    Timewarp_Controller timewarp;
    
    //Physics range things
    Flight flight;
    Camera cam;
    
    
    Vessel* focused_vessel;


    
    

    ProcessedPosition *processed;

    //All vessels in world. Does not contain part data, until loaded.
    //NTS: I would prefer you to do emplace back. Read up on emplace back.
    std::vector<Vessel> vessels;
    
    Planetarium planetarium;

    void step();
    
    //Render focused vessel & any surrounding vessels
    void render_nearby_vessels();

    //Step on rails orbit sim
    void step_rails_orbit_for_v(Vessel* v);
    //Step physics orbits
    void step_physics_orbit_for_v(Vessel* v);

    void render();

    Bundle *planet_bundle;
    Bundle *resource_bundle;
    Bundle *parts_bundle;
    
    private:
};