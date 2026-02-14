#pragma once
#include "../globals.h"
#include "ProtoVessel.h"
#include "Part.h"
#include "../World/orbit.h"


class Vessel {
    public:

    Orbit orbit; //Both rails and phys

    int home_body = 1;


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