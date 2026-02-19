#include "CelestialBody.h"

int CelestialBody::load_model(Bundle* assets) {
    clear_model();
    
    //Load mars.obj, and store a pointer to my asset bundle so i can use when switch_texture get called!
    my_assets = assets;
    if (group.load_group(assets,"body/body")) return 1;

    //Store ngl pointer
    me = group.get_object("Sphere");

    //Now swap to right texture
    std::string final = "body/";
    final.append(name);
    final.append(".png");
    printf("SWITCH TO %s\n",final.c_str());
    switch_texture(final);

    return 0;
}

int CelestialBody::switch_texture(std::string name) {
    return group.swap_texture(my_assets,"Sphere",name);
}

void CelestialBody::clear_model() {
    group.free_group();
}
