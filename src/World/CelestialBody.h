#pragma once
#include "../globals.h"
#include "orbit.h"

struct Planet_Data {
    double radius; //Meters
    double mass;    //kg
    float rotation_rate; //Radians
    
    bool atmosphere;
    int atmosphere_height;
    float sea_level_pressure;
};

//Uses system / Proto_body for storing relationships. this stores data.
class CelestialBody {
    public:
    
    std::string name;
    bool is_home_body = false;
    Orbit orbit; //rails


    //Load ANGEL model
    int load_model(Bundle* assets);

    int switch_texture(std::string name);

    void clear_model();

    ngl_object* me;

    private:
    ModelGroup group; //INDIVDUAL group. stores texture data (!)
    Bundle* my_assets;

};

//Load celestials from system.json
void load_celestial_bodies(std::vector<CelestialBody> *celestials, Bundle* resources);