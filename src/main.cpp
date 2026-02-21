#include "globals.h"
#include "universe.h"
#include "Utility/font.h"

#include "Editor/VAB.h"
#include "Utility/GameTexture.h"
#include "Utility/PartLoader.h"
#include "Utility/cursor.h"
#include "Title/title.h"
#include "Vessel/Part.h"
#if defined(KSPIRE_PLATFORM_WINDOWS) || defined(KSPIRE_PLATFORM_LINUX)
#include <filesystem>
#include <format>
#endif
#ifdef KSPIRE_PLATFORM_WINDOWS
#include <direct.h>
#endif
enum GameStates {
    MENU,
    EDITOR,
    FLIGHT,
    PRELOAD,
};


GameStates current_state = GameStates::PRELOAD;
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
Vessel loading_vessel_buffer;

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
    //Shove vessel buffer (do other stuff like name soon)
    //Should you clear here? (but not on revert to vab ofc)
    loading_vessel_buffer.part_tree = std::move(vab.part_tree);
    vab.destroy_model();
    return 0;
}


int scene_load_flight() {
    loading = true;

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    current_state = GameStates::FLIGHT;
    fonts.drawString("Loading planets...",0xFFFF,*screen,10,220);
    nglDisplay();

    scene_pack_flight();

    uni.planetarium.clear_celestial_models();
    uni.planetarium.load_celestial_bodies(&resource_bundle);

    //Only using earth moon and sun right now
    uni.planetarium.celestials[0].load_model(&planet_bundle);
    uni.planetarium.celestials[1].load_model(&planet_bundle);
    uni.planetarium.celestials[2].load_model(&planet_bundle);


    //DEBUG SHIHH
    Vessel new_vess;
    new_vess.is_focused = new_vess.loaded = true;   //Setup for active + phys
    uni.vessels.emplace_back(std::move(new_vess));
    uni.focused_vessel = &uni.vessels.back();
    uni.focused_vessel->home_body = uni.planetarium.find_body_by_name("Earth");
    //Push buffer
    uni.focused_vessel->part_tree = loading_vessel_buffer.part_tree;

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
    uni.planetarium.clear_celestial_models();
    uni.planetarium.load_celestial_bodies(&resource_bundle);
    //Only using earth and moon
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

// get_binary_directory () -> load_asset_bundle ( path ) -> bundle
// get_binary_directory() -> std::string

char* get_binary_directory(const char* exe_path){
    std::string directory;
    // ./kspire -> prune filename -> ./
    // ../../kspire -> prune -> ../../
    std::filesystem::path p(exe_path);
    std::string parent = p.parent_path().string();
    return strdup(parent.c_str());
}

#if defined(KSPIRE_PLATFORM_WINDOWS) || defined(KSPIRE_PLATFORM_LINUX)
int main(int argc, char* argv[])
#else
int main()
#endif
{
    #if defined(KSPIRE_PLATFORM_LINUX) || defined(KSPIRE_PLATFORM_WINDOWS)
    // switch dir to exedir using argv
        char* directory = get_binary_directory(argv[0]);
        int dir_change_code;
        #ifdef KSPIRE_PLATFORM_LINUX
            dir_change_code = chdir(directory);
        #elifdef KSPIRE_PLATFORM_WINDOWS
            dir_change_code = _chdir(directory);
        #endif
        if (dir_change_code!=0){
            std::string s = std::format("failed to change to executable directory: {}",directory); 
            perror("failed to change to executable directory");
        } else{
            printf("changed directory to executable directory %s",directory);
        }
    #endif

    //Set pointers to bundles
    uni.planet_bundle = &planet_bundle;
    uni.resource_bundle = &resource_bundle;
    uni.parts_bundle = &parts_bundle;

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
    uni.parts_master = &Parts;

    
    //Check for firebird dev env presense to affix absolute mouse mode, otherwise stay in relative mode
    if (fopen("firebird.tns","r") != nullptr) {
        kspire_pad.relative_mode = false;
    }
    #if defined(KSPIRE_PLATFORM_LINUX)
    kspire_pad.relative_mode = false;
    #endif

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
    
    //Load part bundle
    if (parts_bundle.load_asset_bundle("parts.tar.gz.tns")) {
        printf("Asset load error!!\n");
        return 1;
    }
    //Load definitions
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

    

    //vab.hide_vab = true;
    //Debug init scene
    scene_load_menu();
    //scene_load_flight();
    //scene_load_vab();


    //Move this please!!! u toopid
    GameTexture ui_altitude;

    ui_altitude.init(&resource_bundle,"resources/ui/altitude.png",screen);
    ui_altitude.tex.transparent_color = 0x00;

    #ifdef KSPIRE_PLATFORM_NSPIRE
    while(!isKeyPressed(K_ESC) && break_game == 0)
    #else
    while (break_game == 0 && sdl_event.type != SDL_QUIT)
    #endif
    {

        //Tell wayland were healthy and happy
        #ifndef _TINSPIRE

        SDL_PollEvent(&sdl_event);
        if (sdl_event.type == SDL_KEYDOWN) {
            if (sdl_event.key.keysym.sym == SDLK_ESCAPE) // ESC key
                        break_game = true;
        }
        #endif

        kspire_pad.Update();

        if (kspire_pad.true_contact) cursor.set_cursor_position(kspire_pad.x_screen,kspire_pad.y_screen);



        //Contains physics and render code for the flight scene
        //Uni contains the main code of handling the flight scene. this is probably
        //Shitty but ill figure out how to do VAB stuff later. okay!

        if (isKeyPressed(K_DEBUG_SCENE_1) && !loading) {
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

        
        if (isKeyPressed(K_DEBUG_SCENE_2)  && !loading) {
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
                scene_pack_menu(); scene_load_vab();
            }
        }

        if (vab.show_pallete || current_state != GameStates::EDITOR) {
            
            #if defined(KSPIRE_PLATFORM_LINUX)
            const char* VERSION = "PC_" BUILD_DATE "_" BUILD_TIME;
            #else
            #if  defined(_FIREBIRD) && defined(_TINSPIRE)
            const char* VERSION = "FB_" BUILD_DATE "_" BUILD_TIME;
            #else
            const char* VERSION = "TI_" BUILD_DATE "_" BUILD_TIME;
            #endif
            #endif
            fonts.drawString(VERSION,0xFFFF,*screen,10,220);

        }

        nglDisplay();
    }


    scene_pack_flight();
    scene_pack_vab();
    scene_pack_menu();
    uni.planetarium.clear_celestial_models();
    uni.planetarium.celestials.clear(); //Only ever really call this here

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