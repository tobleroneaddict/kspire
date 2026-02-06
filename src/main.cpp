#include "globals.h"
#include "universe.h"




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






Universe uni;

int main()
{

    if (uni.load_bundles()) {return 1;}

	if (!is_touchpad) { return 0;} //Only CX/CXII supported
	SDL_Init(SDL_INIT_VIDEO); //Using SDL for timing
    
    // Initialize nGL
    nglInit();
    // Allocate the framebuffer
    TEXTURE *screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT, 0, false);
    nglSetBuffer(screen->bitmap);
    ProcessedPosition *processed = new ProcessedPosition[9999];
    uni.processed = processed;

    Vessel new_vess;
    new_vess.is_focused = new_vess.loaded = true;   //Setup for active + phys
    uni.vessels.emplace_back(new_vess);
    
    CelestialBody body;
    body.load_model(&uni.planet_bundle);
    uni.celestials.emplace_back(body);
    

    
    printf("Loading complete!\n");
    
    #ifdef _TINSPIRE
    while(!isKeyPressed(KEY_NSPIRE_ESC))
    #else
    for(unsigned int i = 1300;--i;)
    #endif
    {
        uni.step();

        
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
    

    uni.free_bundles();
    SDL_Quit();
    return 0;
}