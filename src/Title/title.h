#pragma once
#include "../globals.h"
#include "../Utility/GameTexture.h"
#include "../Utility/font.h"
#include "../World/CelestialBody.h"
#include "../Menu/unimenu.h"

class Title {
public:
    void load_title(Bundle* resources,ngl_object* _obj,ngl_object* _moon);

    int Update();

    void pack_title();

    ProcessedPosition *processed;
    TEXTURE* screen;

    float angle = 0.0f;

    Fonts* fonts;
    
private:
    GameTexture texture_set;
    //GameTexture tex_buttons; //Sliced to show 3 separate buttons from one image in resources
    ngl_object* obj;
    ngl_object* moon;
    DT clock;
    int select_index = 0;
    bool buttons_wiped = true;
    Menu submenu;
    bool submenu_open = false;
    bool submenu_settings = true; //FALSE manage saves, TRUE manage settings

    static void settings_action();
    void setup_settings();
    void setup_manage_saves();
    void render_3D();
};