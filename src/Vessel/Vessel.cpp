#include "Vessel.h"

// Overarching update function for all vessels. Does not handle orbit states or state changes.
// Used for vessel-specific code such as aerodynamics + acceleration, resource drain, and staging + recalculations.
void Vessel::Update()
{
    // DEBUG STUFF
    if (isKeyPressed(SDLK_i))
    {
        orbit.VEL.z -= 10;
    }
    if (isKeyPressed(SDLK_k))
    {
        orbit.VEL.z += 10;
    }

    if (isKeyPressed(SDLK_j))
    {
        orbit.VEL.x -= 10;
    }
    if (isKeyPressed(SDLK_l))
    {
        orbit.VEL.x += 10;
    }

    if (isKeyPressed(SDLK_u))
    {
        orbit.VEL.y -= 10;
    }
    if (isKeyPressed(SDLK_o))
    {
        orbit.VEL.y += 10;
    }

    // Focused-specific stuff
    if (is_focused)
    {
        //......
    }

    // Any physics updates
    if (loaded)
        Update_Physics();

    // Any static updates (Electric drain and stuff)
    Update_Static();
};

// Physics updates
void Vessel::Update_Physics()
{
    auto com = &protoVessel.CoM;
    *com = {0, 0, 0};
    float _mass = 0;
    if (part_tree.size() > 0)
    {
        // Recalc CoM
        for (Part &p : part_tree)
        {
            *com += (p.pos + p.attPos) * p.mass;
            _mass += p.mass;
        }
        if (_mass > 0)
        {
            *com /= _mass;
        }
        // then relative to root part
        // *com = part_tree[0].pos - *com;
        protoVessel.mass = _mass;
    }
};

// Static updates
void Vessel::Update_Static() {

};

void Vessel::rebase()
{
    printf("RB\n");
    // Redo DB
    database.clear();
    for (size_t i = 0; i < part_tree.size(); ++i)
    {
        database.emplace(part_tree[i].unique_id, i);
    }
    // Calculate links

    // 1 avoids the root
    for (unsigned int i = 1; i < part_tree.size(); i++)
    {
        Part *curr = &part_tree[i];

        for (Node &n : curr->nodes)
        {

            if (n.attached_node == -1)
                continue; // Skip detached
            unsigned int att_part = n.attached_node;

            // TODO: what is the goal here?
        }
    }

    // TODO : Calc Links
}

// Helpers

unsigned int Vessel::node_to_part(unsigned int node_id)
{
    return node_id / 10;
}
int Vessel::node_index(unsigned int node_id)
{
    return node_id % 10;
}
