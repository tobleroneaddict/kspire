#pragma once
#include "CelestialBody.h"

//Definitions for storing solar system relationships.

struct Proto_Body {
    CelestialBody* parent;
    std::vector<CelestialBody*> children;
    CelestialBody* body;
};
