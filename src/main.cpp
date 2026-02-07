#include "globals.h"
#include "universe.h"
#include "Utility/font.h"



template <typename T> T clamp(T in, T min, T max) {
    if (in < min) { return min; }
    if (in > max) { return max; }
    return in;
} 

inline float clamp(float x, float min, float max) {
    if (x < min) {return min;}
    if (x > max) {return max;}
    return x;
}

enum GameStates {
    MENU,
    BUILD,
    FLIGHT,
};




Universe uni;

Fonts fonts;

//Angel Asset bundles
Bundle resource_bundle; //RESOURCE
Bundle planet_bundle;   //BODY  
Bundle parts_bundle;    //PARTS

int main()
{

	if (!is_touchpad) { return 0;} //Only CX/CXII supported
	SDL_Init(SDL_INIT_VIDEO); //Using SDL for timing

    // Initialize nGL
    nglInit();
    // Allocate the framebuffer
    TEXTURE *screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT, 0, false);
    nglSetBuffer(screen->bitmap);

    if (resource_bundle.load_asset_bundle("resources.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }

    //Load font data
    if (fonts.init(&resource_bundle) != 0) {
        printf("Error loading fonts!\n");
    }


    //LOAD PLANETS
    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fonts.drawString("Loading planets...",0xFFFF,*screen,10,220);
    nglDisplay();

    if (uni.load_celestial_bodies(&uni.celestials,&resource_bundle)) return 1;
    

    //will this explode i hope not....
    ProcessedPosition *processed = new ProcessedPosition[9999];
    uni.processed = processed;

    if (planet_bundle.load_asset_bundle("body.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }

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
    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fonts.drawString("Loading complete!",0xFFFF,*screen,10,220);
    nglDisplay();
    
    printf("Loading complete!\n");
    
    #ifdef _TINSPIRE
    while(!isKeyPressed(KEY_NSPIRE_ESC))
    #else
    for(unsigned int i = 1300;--i;)
    #endif
    {
        //Contains physics and render code for the flight scene
        uni.step(); 


        fonts.drawString("DEMO BUILD",0xFFFF,*screen,10,220);

        std::string warp_string = "WARP ";
        warp_string.append(std::to_string(uni.phys_warp_rate));
        fonts.drawString(warp_string.c_str(),0xFFFF,*screen,10,20);


        std::string time_string = "GameTime ";
        time_string.append(std::to_string(uni.universal_time));
        fonts.drawString(time_string.c_str(),0xFFFF,*screen,10,50);

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