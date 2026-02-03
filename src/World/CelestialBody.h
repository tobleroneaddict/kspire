#pragma once
#include "../globals.h"
#include "orbit.h"

class CelestialBody {
    public:
    
    std::string name;

    

    //Load ANGEL model
    int load_model(Bundle* assets);

    void clear_model();

    ngl_object* me;

    private:
    ModelGroup group;

};