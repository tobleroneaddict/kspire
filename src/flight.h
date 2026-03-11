#pragma once
#include "Vessel/Vessel.h"
#include "globals.h"

// Contains all the vessels in the flight scene. If a vessel is in physics range, it
// exists here.

class Flight
{
  public:
    Vessel *active_vessel;
    std::vector<Vessel *> vessels_in_range;

    int physics_range = 2300; // 2300m

  private:
};