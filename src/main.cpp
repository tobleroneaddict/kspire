#include "globals.h"
#include "universe.h"
#include "Utility/font.h"

#include "Editor/VAB.h"
#include "Utility/GameTexture.h"

enum GameStates {
    MENU,
    EDITOR,
    FLIGHT,
};


GameStates current_state = MENU;
bool loading = true;

Universe uni;
VAB vab;

Fonts fonts;

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



int scene_load_flight() {
    loading = true;

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    current_state = GameStates::FLIGHT;
    //LOAD PLANETS
    
    fonts.drawString("Loading planets...",0xFFFF,*screen,10,220);
    nglDisplay();
    
    if (planet_bundle.load_asset_bundle("body.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }
    
    if (uni.planetarium.load_celestial_bodies(&resource_bundle)) return 1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fonts.drawString("Loading parts...",0xFFFF,*screen,10,220);
    nglDisplay();
    

    if (parts_bundle.load_asset_bundle("parts.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }

    //Set pointers to bundles
    uni.planet_bundle = &planet_bundle;
    uni.resource_bundle = &resource_bundle;
    uni.parts_bundle = &parts_bundle;

    //DEBUG SHIHH
    Vessel new_vess;
    new_vess.is_focused = new_vess.loaded = true;   //Setup for active + phys
    uni.vessels.emplace_back(new_vess);
    uni.focused_vessel = &new_vess;
    
    
    uni.planetarium.celestials[0].load_model(uni.planet_bundle);
    uni.planetarium.celestials[2].load_model(uni.planet_bundle);

    uni.planetarium.celestials[1].load_model(uni.planet_bundle);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fonts.drawString("Loading complete!",0xFFFF,*screen,10,220);
    nglDisplay();
    printf("Loading complete!\n");
    loading = false;
    return 0;
}

int scene_load_vab() {
    loading = true;
    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    current_state = GameStates::EDITOR;
    fonts.drawString("Loading VAB...",0xFFFF,*screen,10,220);
    nglDisplay();
    printf("Loading VAB...\n");
    vab.load_model(&resource_bundle);
    loading = false;
    return 0;
}



int scene_pack_flight() {
    uni.pack();
    return 0;
}
int scene_pack_vab() {
    vab.destroy_model();
    return 0;
}


int main()
{
	if (!is_touchpad) { return 0;} //Only CX/CXII supported
	SDL_Init(SDL_INIT_VIDEO); //Using SDL for timing

    // Initialize nGL
    nglInit();
    // Allocate the framebuffer
    screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT, 0, false);
    nglSetBuffer(screen->bitmap);
    vab.screen = screen;    

    //Processed position for nGL
    processed = new ProcessedPosition[9999];
    uni.processed = processed;
    uni.planetarium.processed = processed;

    //Global bundle
    if (resource_bundle.load_asset_bundle("resources.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }

    //Load font data
    if (fonts.init(&resource_bundle) != 0) {
        printf("Error loading fonts!\n");
    }


    //Debug init scene
    //scene_load_flight();
    scene_load_vab();

    planet_bundle.debug_list_assets();

   
    GameTexture ui_altitude;

    ui_altitude.init(&resource_bundle,"resources/ui/altitude.png",screen);
    ui_altitude.tex.transparent_color =  ui_altitude.tex_data.back() - 1;

    #ifdef _TINSPIRE
    while(!isKeyPressed(KEY_NSPIRE_ESC))
    #else
    for(unsigned int i = 1300;--i;)
    #endif
    {
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
            uni.step();

            //Altitude
            ui_altitude.draw(0,0);
            debug_print("",(int)(uni.universal_time),2,3,screen,"s");
            debug_print("",(int)(uni.focused_vessel->protoVessel.altitude/1000),85,3,screen,"km");
           
            debug_print("Warp: x ",(int)(uni.timewarp.warp_rate + 0.5f),200,220,screen);
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

            vab.render(processed);
    
        }
        fonts.drawString("DEMO BUILD",0xFFFF,*screen,10,220);
            
        
        

        nglDisplay();
    }

    

    /*



        //CAPSULE
        glPushMatrix();

		glTranslatef(0, 0, 0);

        nglRotateX(200);
        nglRotateZ(300);
		nglRotateY(270);
        //nglRotateX(angle.normaliseAngle());
        //nglRotateY(angle.normaliseAngle());


        glScale3f(20, 20, 20);


        obj = scene[1];
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);



        glPopMatrix();


        glPopMatrix();

        nglDisplay();
        big_counter ++;
        planet_angle -= 0.0002f * physics.step_time * clock.dt;

        physics.step(clock.dt);
    }



    */
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