#include "CelestialBody.h"

int CelestialBody::load_model(Bundle* assets) {
    clear_model();
    
    //Load mars.obj, and store a pointer to my asset bundle so i can use when switch_texture get called!
    my_assets = assets;
    if (group.load_group(assets,"body/body")) return 1;

    //Store ngl pointer
    me = group.get_object("LOD3");

    //Now swap to right texture
    std::string final = "body/";
    final.append(name);
    final.append(".png");
    printf("SWITCH TO %s\n",final.c_str());
    switch_texture(final);

    return 0;
}

float CelestialBody::get_atm_intensity(float altitude) {
    float power = 0;
    if (altitude > (float)atmosphere_height)
        return 0;
    if (altitude < 0) 
        return 1;

    //Have a more complex function here for the real deal
    power = altitude / (float)atmosphere_height;
    power = 1.0f - power; //inverse
    return power;
}


int CelestialBody::switch_texture(std::string name) {
    return group.swap_texture(my_assets,me->name,name);
}

void CelestialBody::clear_model() {
    group.free_group();
}

CelestialBody:: ~CelestialBody() {
    clear_model();
}