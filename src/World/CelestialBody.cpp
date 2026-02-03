#include "CelestialBody.h"


int CelestialBody::load_model(Bundle* assets) {
    //Load mars.obj
    if (group.load_group(assets,"mars")) return 1;

    //Store ngl pointer
    me = group.get_object("Sphere");


    return 0;
}

void CelestialBody::clear_model() {
    group.free_group();
}