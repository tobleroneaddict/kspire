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

    //Third level UI element
    bool select_box_open = false;
    Menu select_box;
    bool waiting_for_any_key = false;
    int menu_clicked_key_id = -1;

    //callback
    void settings_onclick_action(Menu_Item* item);
    void saves_onclick_action(Menu_Item* item);

    void setup_settings();
    void setup_manage_saves();
    void render_3D();


    //Set key config from select box
    void set_key_config(int key);
};