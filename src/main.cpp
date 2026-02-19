#include "globals.h"
#include "universe.h"
#include "Utility/font.h"

#include "Editor/VAB.h"
#include "Utility/GameTexture.h"
#include "Utility/PartLoader.h"
#include "Utility/cursor.h"
#include "Title/title.h"

enum GameStates {
    MENU,
    EDITOR,
    FLIGHT,
};


GameStates current_state = MENU;
bool loading = true;
bool break_game = false;

Title title;
Universe uni;
VAB vab;
PartLoader Parts;
Fonts fonts;
Cursor cursor;

//Angel Asset bundles
Bundle resource_bundle; //RESOURCE
Bundle planet_bundle;   //BODY
Bundle parts_bundle;    //PARTS

//nGL stuff
TEXTURE *screen;
ProcessedPosition *processed;

template <typename T> void debug_print(std::string preamble, T value, int x, int y, TEXTURE *screen, std::string unit = "") {
    preamble.append(std::to_string(value));
    preamble.append(unit);
    fonts.drawString(preamble.c_str(), 0xFFFF, *screen, x, y);
}

template <typename T> void debug_print(T value, int x, int y, TEXTURE *screen) {
    std::string preamble = std::to_string(value);
    fonts.drawString(preamble.c_str(), 0xFFFF, *screen, x, y);
}


//TO BE MOVED

int scene_pack_flight() {
    cursor.set_cursor_visibility(false);
    uni.pack();
    return 0;
}
int scene_pack_vab() {
    cursor.set_cursor_visibility(false);
    vab.destroy_model();
    return 0;
}


int scene_load_flight() {
    loading = true;

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    current_state = GameStates::FLIGHT;
    fonts.drawString("Loading...",0xFFFF,*screen,10,220);
    nglDisplay();

    scene_pack_flight();

    //DEBUG SHIHH
    Vessel new_vess;
    new_vess.is_focused = new_vess.loaded = true;   //Setup for active + phys
    uni.vessels.emplace_back(std::move(new_vess));
    uni.focused_vessel = &uni.vessels.back();
    uni.focused_vessel->home_body = uni.planetarium.find_body_by_name("Earth");
    
    uni.planetarium.celestials[0].load_model(uni.planet_bundle);
    uni.planetarium.celestials[2].load_model(uni.planet_bundle);
    //earth 1 is always loaded in FIRST. at title
    //so is the moon too

    uni.planetarium.celestials[1].load_model(uni.planet_bundle);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fonts.drawString("Loading complete!",0xFFFF,*screen,10,220);
    nglDisplay();
    printf("Loading complete!\n");
    loading = false;
    return 0;
}

int scene_load_vab() {
    scene_pack_vab();
    loading = true;
    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    current_state = GameStates::EDITOR;
    fonts.drawString("Loading VAB...",0xFFFF,*screen,10,220);
    nglDisplay();
    printf("Loading VAB...\n");
    vab.Start(&resource_bundle);
    loading = false;
    cursor.set_cursor_visibility(true);
    return 0;
}


int scene_load_menu() {
    current_state = GameStates::MENU;
    loading = true;

    //Make this call earth and moon instead of index?
    uni.planetarium.celestials[1].load_model(&planet_bundle);
    uni.planetarium.celestials[2].load_model(&planet_bundle);
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    title.load_title(&resource_bundle,uni.planetarium.celestials[1].me,uni.planetarium.celestials[2].me);
    loading = false;
    cursor.set_cursor_visibility(false);
    return 0;
}

int scene_pack_menu() {
    title.pack_title();
    return 0;
}

int main()
{
	if (!is_touchpad) { return 0;} //Only CX/CXII supported
    cursor.set_cursor_visibility(false);
	SDL_Init(SDL_INIT_VIDEO); //Using SDL for timing

    // Initialize nGL
    nglInit();
    // Allocate the framebuffer
    screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT, 0, false);
    nglSetBuffer(screen->bitmap);
    vab.screen = screen;    
    title.screen = screen;

    //Processed position for nGL
    processed = new ProcessedPosition[9999];
    //Condense in a sec
    uni.processed = processed;
    uni.planetarium.processed = processed;
    vab.processed = processed;
    title.processed = processed;
    vab.parts_master = &Parts;

    //Check for firebird dev env presense to affix absolute mouse mode, otherwise stay in relative mode
    if (fopen("firebird.tns","r") != nullptr) {
        kspire_pad.relative_mode = false;
    }

    //Global bundle
    if (resource_bundle.load_asset_bundle("resources.tar.gz.tns")) {
        printf("Asset load error!!\n");
        return 1;
    }

    //Load font data
    if (fonts.init(&resource_bundle) != 0) {
        printf("Error loading fonts!\n");
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fonts.drawString("Loading parts...",0xFFFF,*screen,10,220);
    nglDisplay();
    

    if (parts_bundle.load_asset_bundle("parts.tar.gz.tns")) {
        printf("Asset load error!!\n");
        return 1;
    }

    Parts.load_parts(&parts_bundle);


    //LOAD PLANETS
    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fonts.drawString("Loading planets...",0xFFFF,*screen,10,220);
    nglDisplay();

    if (planet_bundle.load_asset_bundle("body.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }
    
    if (uni.planetarium.load_celestial_bodies(&resource_bundle)) return 1;

    //Set pointers to bundles
    uni.planet_bundle = &planet_bundle;
    uni.resource_bundle = &resource_bundle;
    uni.parts_bundle = &parts_bundle;

    vab.hide_vab = true;
    //Debug init scene

    scene_load_menu();
    scene_load_flight();
    //scene_load_vab();
    //scene_load_menu();

    return 0;
    //Move this please!!! u toopid
    GameTexture ui_altitude;

    ui_altitude.init(&resource_bundle,"resources/ui/altitude.png",screen);
    ui_altitude.tex.transparent_color = 0x00;

    #ifdef _TINSPIRE
    while(!isKeyPressed(KEY_NSPIRE_ESC) && break_game == 0)
    #else
    for(unsigned int i = 1300;--i;)
    #endif
    {
        kspire_pad.Update();
        
        
        if (kspire_pad.true_contact) cursor.set_cursor_position(kspire_pad.x_screen,kspire_pad.y_screen);
        

        //Contains physics and render code for the flight scene
        //Uni contains the main code of handling the flight scene. this is probably
        //Shitty but ill figure out how to do VAB stuff later. okay!

        if (isKeyPressed(KEY_NSPIRE_1) && !loading) {
            switch (current_state) {
                case GameStates::EDITOR:
                scene_pack_vab();break;
                case GameStates::FLIGHT:
                scene_pack_flight();break;
                default:
                break;
            }
            scene_load_flight();
        }

        
        if (isKeyPressed(KEY_NSPIRE_2)  && !loading) {
            switch (current_state) {
                case GameStates::EDITOR:
                scene_pack_vab();break;
                case GameStates::FLIGHT:
                scene_pack_flight();break;
                default:
                break;

            }
            scene_load_vab();
        }

        
        if (current_state == GameStates::FLIGHT) {

            if (uni.focused_vessel != nullptr) {
                uni.Update();

                //Altitude
                ui_altitude.draw(0,0);
                debug_print("",(int)(uni.universal_time),2,3,screen,"s");
                debug_print("",(int)(uni.focused_vessel->protoVessel.altitude/1000),85,3,screen,"km");
            
                debug_print("Warp: x ",(int)(uni.timewarp.warp_rate + 0.5f),200,220,screen);
            }
            /*
            debug_print("SMA ",uni.focused_vessel->orbit.semi_major_axis,10,10,screen);
            debug_print("ECC ",uni.focused_vessel->orbit.eccentricity,10,30,screen);
            debug_print("PRD ",uni.focused_vessel->orbit.period,10,50,screen);
            debug_print("SPD ",uni.focused_vessel->orbit.orbital_speed,10,70,screen);
            debug_print("INC ",uni.focused_vessel->orbit.inclination,10,90,screen);
            debug_print("M-A ",uni.focused_vessel->orbit.mean_anomaly,10,110,screen);
            debug_print("LAN ",uni.focused_vessel->orbit.long_ascending_node,10,130,screen);
            debug_print("EPC ",uni.focused_vessel->orbit.epoch,10,150,screen);
            */
        }
        if (current_state == GameStates::EDITOR) {

            vab.Update();
    
        }
        if (current_state == GameStates::MENU) {
            auto res = title.Update(); 
            if (res == -1) break_game = true;
            if (res == 600) {
                scene_pack_menu(); scene_load_flight();
            }
        }

        if (vab.show_pallete || current_state != GameStates::EDITOR) {
            fonts.drawString("DEMO BUILD",0xFFFF,*screen,10,220);
        }

        nglDisplay();
    }


    scene_pack_flight();
    scene_pack_vab();
    scene_pack_menu();

    delete[] processed;
    // Deinitialize nGL
    nglUninit();
    deleteTexture(screen);

    resource_bundle.free();
    planet_bundle.free();
    parts_bundle.free();

    SDL_Quit();
    return 0;
}