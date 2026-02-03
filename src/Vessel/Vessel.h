#pragma once
#include "../globals.h"
#include "ProtoVessel.h"
#include "Part.h"
#include "../World/orbit.h"
#include "../World/physics.h"

class Vessel {
    public:

    Orbit orbit; //rails
    Phys physics;    


    enum Situation {
        LANDED,
        SPLASHED,
        PRELAUNCH,
        FLYING,
        SUB_ORBITAL,
        ORBITING,
        ESCAPING,
        DOCKED
    };
    enum State {
        INACTIVE,
        ACTIVE,
        DEAD
    };

    
    ProtoVessel protoVessel;
    
    //Are parts loaded? (Also Physics range)
    bool loaded = false;
    //Is this the current controller?    
    bool is_focused = false;

    std::vector<Part> part_tree;

    private:
};