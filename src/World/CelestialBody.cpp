#include "CelestialBody.h"

int CelestialBody::load_model(Bundle* assets) {
    clear_model();
    
    //Load mars.obj, and store a pointer to my asset bundle so i can use when switch_texture get called!
    my_assets = assets;
    if (group.load_group(assets,"body/body")) return 1;

    //Store ngl pointer
    //LOD1 sucks, makes the moon in LEO look awful.
    //LOD2 is okay, some weird artifacts on the moon but it's totally playable.
    //LOD3 sweet spot, nothing major until you go below 600 km
    //LOD4 is beautiful
    me = group.get_object("LOD4"); //Sets default for title screen, gets
    //switched out during rendering

    //Now swap to right texture
    std::string final = "body/";
    final.append(name);
    final.append(".png");
    //printf("SWITCH TO %s\n",final.c_str());
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

void CelestialBody::switch_lod(LODLevels level) {

    if (_stored_lod_level == level) return; //return if same
    switch (level) {
        case (LOD1):
        me = group.get_object("LOD1");break;
        case (LOD2):
        me = group.get_object("LOD2");break;
        case (LOD3):
        me = group.get_object("LOD3");break;
        case (LOD4):
        me = group.get_object("LOD4");break;
        default:
        me = group.get_object("LOD1");break;
    }
    printf("%s switched LOD level to LOD{%d}.\n",name.c_str(),level);
    switch_texture(_stored_texture_name);
    _stored_lod_level = level;
}

int CelestialBody::switch_texture(std::string name) {
    _stored_texture_name = name;
    return group.swap_texture(my_assets,me->name,name);
}

void CelestialBody::clear_model() {
    group.free_group();
}

CelestialBody:: ~CelestialBody() {
    clear_model();
}