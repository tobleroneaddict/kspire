#pragma once
#include "../globals.h"
#include "orbit.h"


//Uses system / Proto_body for storing relationships. this stores data.
class CelestialBody {
    public:
    
    enum LODLevels {
        LOD1,
        LOD2,
        LOD3,
        LOD4
    };

    std::string name = "";

    std::string parent = "";

    double radius = -1; //Meters
    double mass = -1;    //kg
    float rotation_rate = -1; //Radians
    float angle;

    int atmosphere = -1;
    int atmosphere_height = 70000;
    float sea_level_pressure = -1;

    bool is_home_body = false;
    Orbit orbit; //rails
    double _SOI = -1;  //Calculated and replaced with Planetarium::get_soi (Optimization). Not accurate for sol
    
    //Load ANGEL model
    int load_model(Bundle* assets);

    
    int switch_texture(std::string name);

    void switch_lod(LODLevels level);

    void clear_model();
    ~CelestialBody();
    ngl_object* me;


    //Altitude color intensity from altitude above sea lvl (0-1)
    float get_atm_intensity(float altitude);

    private:
    ModelGroup group; //INDIVDUAL group. stores texture data (!)
    Bundle* my_assets;

    std::string _stored_texture_name = "";
    LODLevels _stored_lod_level = LOD3;
};

