#pragma once
#include "../World/orbit.h"
#include "../globals.h"
#include "Part.h"
#include "ProtoVessel.h"

class Vessel
{
  public:
    Orbit orbit; // Both rails and phys

    int home_body = 1;
    int last_home_body = 1;

    enum Situation
    {
        LANDED,
        SPLASHED,
        PRELAUNCH,
        FLYING,
        SUB_ORBITAL,
        ORBITING,
        ESCAPING,
        DOCKED
    };
    enum State
    {
        INACTIVE,
        ACTIVE,
        DEAD,
    };

    ProtoVessel protoVessel;

    // Are parts loaded? (Also Physics range)
    bool loaded = false;
    // Is this the current controller?
    bool is_focused = false;

    // Part Data
    std::vector<Part> part_tree;
    // Map {id,part_tree index}
    std::unordered_map<unsigned int, unsigned int> database;
    // Rebase all parts, links, and shit
    void rebase();

    std::vector<unsigned int> get_tanks_to_decoupler(unsigned int part);

    void Update();

  private:
    void Update_Physics();
    void Update_Static();

  public:
    unsigned int node_to_part(unsigned int node_id);
    int node_index(unsigned int node_id);
};