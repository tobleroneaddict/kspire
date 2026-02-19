#pragma once
#include "../globals.h"
#include "../Utility/GameTexture.h"
#include "../World/CelestialBody.h"

class Title {
public:
    void load_title(Bundle* resources,ngl_object* _obj,ngl_object* _moon);

    int Update();

    void pack_title();

    ProcessedPosition *processed;
    TEXTURE* screen;

    float angle = 0.0f;

private:
    GameTexture texture_set;
    //GameTexture tex_buttons; //Sliced to show 3 separate buttons from one image in resources
    ngl_object* obj;
    ngl_object* moon;

    int select_index = 0;
    bool buttons_wiped = true;
};