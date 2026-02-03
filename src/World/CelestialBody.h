#pragma once
#include "../globals.h"
#include "orbit.h"

class CelestialBody {
    public:
    
    std::string name;

    

    //Load ANGEL model
    int load_model();

    int render(float distance);



    ngl_object* me;

    private:
    ModelGroup group;

};