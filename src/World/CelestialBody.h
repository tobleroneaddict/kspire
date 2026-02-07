#pragma once
#include "../globals.h"
#include "orbit.h"



//Uses system / Proto_body for storing relationships. this stores data.
class CelestialBody {
    public:
    
    std::string name = "";

    std::string parent = "";

    linalg::vec<float,3> POS = {0,0,0}; //Pos relative to parent

    double radius = -1; //Meters
    double mass = -1;    //kg
    float rotation_rate = -1; //Radians
    
    int atmosphere = -1;
    int atmosphere_height = -1;
    float sea_level_pressure = -1;

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

