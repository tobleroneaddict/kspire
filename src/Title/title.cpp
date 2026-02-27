#include "title.h"

void Title::settings_onclick_action(Menu_Item* item) {
    printf("Button pressed: %s\n",item->label.c_str());
    item->value = std::to_string(global_settings.Gameplay.NAVBALL);
}

void Title::saves_onclick_action(Menu_Item* item) {
    printf("Button pressed: %s\n",item->label.c_str());
}

void Title::setup_settings() 
{
    submenu.items.clear();
    submenu.centered_to_screen = true;
    submenu.titlebar = "Settings";
    submenu.titlebar_centered = true;
    submenu.select_base = 0;
    submenu.scroll = 0;

    submenu.add_item("[Flight]:\n",nullptr,0); submenu.items.back().value = "";
    submenu.add_item("Flight Cam: Orbit Speed:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Flight Cam: Zoom Speed:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Max Debris:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("[Editor]:\n",nullptr,0); submenu.items.back().value = "";
    submenu.add_item("Editor Cam: Orbit Speed:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Editor Cam: Zoom Speed:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("[Graphics]:\n",nullptr,0); submenu.items.back().value = "";
    submenu.add_item("Show Navball:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Sun Shadows:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Detailed Shadows:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Terrain Quality:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Show Skybox:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Ambient Light:",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    //Controls
   //std::to_string(global_settings.Controls.S_K_PAD_N)
    submenu.add_item("[Controls]:\n",nullptr,0); submenu.items.back().value = "";
    submenu.add_item("Pad N:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad S:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad E:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad W:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad NW:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad SW:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad NE:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Pad SE:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Warp Up:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Warp Down:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Editor Up:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Editor Down:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Editor In:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Editor Out:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Control:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Shift:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);
    submenu.add_item("Map View:\n",[this](Menu_Item* item){ settings_onclick_action(item);},0);




}
void Title::setup_manage_saves() 
{
    submenu.items.clear();
    submenu.centered_to_screen = true;
    submenu.titlebar = "Manage Saves";
    submenu.titlebar_centered = true;
    submenu.select_base = 0;
    submenu.scroll = 0;

    submenu.add_item("Default ",[this](Menu_Item* item){ saves_onclick_action(item);},0);
    submenu.add_item("Default (0)",[this](Menu_Item* item){ saves_onclick_action(item);},0);
    submenu.add_item("Default (1)",[this](Menu_Item* item){ saves_onclick_action(item);},0);
    submenu.add_item("Default (2)",[this](Menu_Item* item){ saves_onclick_action(item);},0);
    submenu.add_item("Default (3)",[this](Menu_Item* item){ saves_onclick_action(item);},0);

}


//To avoid memory leaks it uses the nglobject from the loaded planets.
//It was orignally just gonna load it on it's own, but apparently that crashes
//the game after a few retries despite having used the same setup as planetarium.celestials...
void Title::load_title(Bundle* resources,ngl_object* _obj,ngl_object* _moon) 
{
    obj = _obj;
    moon = _moon;
    angle = 0.0f;

    //Move this to a new delete
    texture_set.init(resources,"resources/ui/title.png",screen);
    
    
    submenu.init(resources,screen,0,32,200,280,fonts);
}

int Title::Update() 
{
    clock.tick();

    //Top menu control state
    if (!submenu_open) { //Cannot access top menu while in settings!
        if (isKeyPressed(K_PAD_S) && buttons_wiped)
            {select_index++; buttons_wiped = false;}
        if (isKeyPressed(K_PAD_N) && buttons_wiped)
            {select_index--; buttons_wiped = false;}
    
        if (!isKeyPressed(K_PAD_N) && !isKeyPressed(K_PAD_S)) buttons_wiped = true;

        if (select_index < 0) select_index = 4 + select_index; //Loop
        else if (select_index > 3) select_index -= 4;

        //Click events
        if (select_index == 3 && isKeyPressed(K_ENTER)) {
            return -1;
        }
        //Start game. In the release, this will not return 600 just yet. There will be a save loading menu 
        if (select_index == 0 && isKeyPressed(K_ENTER)) {
            return 600;
        }
        //Open Submenu. blocks top menu
        if ((select_index == 1 || select_index == 2) && isKeyPressed(K_ENTER)) {
            if (select_index == 1) {submenu_settings = false; setup_manage_saves();}
            if (select_index == 2) {submenu_settings = true;  setup_settings();}
            submenu_open = !submenu_open;
        }
    } else { //non unimenu keys
        if (isKeyPressed(K_ESC)) submenu_open = false;
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Render
    render_3D();

    //Title screen
    texture_set.draw(0,0,229,81,229,81);
    //Half Opacity Orbit line
    drawTextureOverlay(texture_set.tex,0,82,*screen,0,0,229,92);
    //Main Buttons
    drawTexture(texture_set.tex,*screen,0,174,149,75,SCREEN_WIDTH-149,SCREEN_HEIGHT-75,149,75);

    //Top Menu buttons
    switch (select_index) {
        case 0: drawTexture(texture_set.tex,*screen,0,249,149,23,SCREEN_WIDTH-149,SCREEN_HEIGHT-75,149,23);
        break;
        case 1: drawTexture(texture_set.tex,*screen,0,249,149,23,SCREEN_WIDTH-149,SCREEN_HEIGHT-49,149,23);
        break;
        case 2: drawTexture(texture_set.tex,*screen,0,275
            ,89,23,SCREEN_WIDTH-149,SCREEN_HEIGHT-23,89,23);
        break;
        case 3: drawTexture(texture_set.tex,*screen,92,275
            ,57,23,SCREEN_WIDTH-58,SCREEN_HEIGHT-23,57,23);
        break;
        default:
        break;
    }

    //Submenu
    if (submenu_open) {
        submenu.Update();
    }


        
    angle += 10.0f * clock.dt;

    return 0;
}


void Title::pack_title() 
{
    texture_set.tex.bitmap = nullptr;
    texture_set.tex_data.clear();
}


void Title::render_3D() 
{
    if (obj != nullptr) {

        
        glPushMatrix();

        glTranslatef(30,-23,100);

        glScale3f(60,60,60);

        float a = fmod(fmod(angle, 360.0f) + 360.0f, 360.0f);
        nglRotateZ(20);
        nglRotateY(360.0f - a);
        
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);
        glPopMatrix();

        
        
    }
    if (moon != nullptr) {
        //Hacky title screen animation
        float off = fmod(fmod((-angle)/2, 360.0f) + 360.0f, 360.0f);
        
        glPushMatrix();
        nglRotateZ(10);
    
        //Fix texture explosion from overrotation

        //why the fuck is this necessary
        #ifdef _TINSPIRE
        nglRotateY(
            fmod(fmod((off+100)/1, 360.0f) + 360.0f, 360.0f)
        ); //Orbit
        #else
        nglRotateY(off); //Orbit
        #endif
        glTranslatef(0,-23,500);

        glScale3f(30,30,30);

        nglRotateZ(20);
        nglRotateY(360.0f - 30);
        nglRotateY(110);

        glBindTexture(moon->texture);
        nglDrawArray(moon->vertices, moon->count_vertices, moon->positions, moon->count_positions, processed, moon->draw_mode);
        
        glPopMatrix();
    }
}