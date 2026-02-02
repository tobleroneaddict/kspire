#pragma once
#include "../globals.h"
#include "ProtoVessel.h"
#include "Part.h"


class Vessel {
    public:
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

    //Is loaded?
    bool loaded;


    std::vector<Part> part_tree;

    private:
};