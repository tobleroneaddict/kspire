#pragma once
#include "globals.h"
#include "Vessel/Vessel.h"
#include "flight.h"
#include "World/CelestialBody.h"
#include "World/Camera/camera.h"
#include "Utility/timewarp_controller.h"
#include "World/Planetarium.h"
#include "Utility/PartLoader.h"

//Contains all vessels.
class Universe {
    public:

    bool in_map_view = false;
    

    DT clock;
    double universal_time = 0;

    Timewarp_Controller timewarp;
    

    ModelGroup skybox;

    //Physics range things
    Flight flight;
    Camera cam;
    
    
    Vessel* focused_vessel;

    ProcessedPosition *processed;
    PartLoader* parts_master;

    //All vessels in world. Does not contain part data, until loaded.
    //NTS: I would prefer you to do emplace back. Read up on emplace back.
    std::vector<Vessel> vessels;
    
    Planetarium planetarium;

    void Update();
    
    //Render focused vessel & any surrounding vessels
    void render_nearby_vessels();

    //Step on rails orbit sim
    void step_rails_orbit_for_v(Vessel* v);
    //Step physics orbits
    void step_physics_orbit_for_v(Vessel* v);

    void render_flight();
    void render_map();

    //Unload flight scene
    void pack();

    Bundle *planet_bundle;
    Bundle *resource_bundle;
    Bundle *parts_bundle;
    
    private:

    void rotate_camera();
    void render_skybox();
    bool map_button_held = false;

    float map_zoom = -200;
};