#include "universe.h"
void Universe::render_celestials() {
    
    if (focused_vessel == nullptr) { printf("E 218754: Cannot render planets!\n");return;}
    if (celestials.size() == 0) { printf("E 128585: Cannot render planets!\n");return;}

    //Should i do distance from CAMERA instead??????

    for (CelestialBody& c : celestials) {
        int mode = 0;
        //Check if vessel is low enough to render in nearby mode
        if (1 == 5) {
            mode = 1;
        }

        if (mode) {
            //Mode 1 PQS
            printf("Mode 1 not implemented!\n");
        } else {
            //Mode 0 Distant
            //Planet renderer works by scaling the glscale3f, and keeping the planet at a fixed distance

            //Vessel coordinate in planet space, per planet
            float v_x = focused_vessel->physics.POS.x;
            float v_y = focused_vessel->physics.POS.y;
            float v_z = focused_vessel->physics.POS.z;
            
            float len = sqrtf(v_x*v_x +
            v_y*v_y +
            v_z*v_z);

            //3000 meter bubble
            float fixed_bubble = 3000;
            glTranslatef(
                -(v_x  / len)* fixed_bubble        * 1,
                -(v_y  / len)* fixed_bubble        * 1,
                -(v_z   / len)* fixed_bubble       * 1
            );



            //Debug rotation
            //GLFix normd = planet_angle;
            //nglRotateY(normd.normaliseAngle());

            float mars_radius = 3389500;
            float angular_radius = mars_radius / len;    //NTS: MAKE THIS REFERENCE CELESTIAL (but make sure it can do moons too so detached. run this for each visible planet?)
            float render_radius  = angular_radius * fixed_bubble;


            glScale3f(render_radius, render_radius, render_radius);
            
            
            //glScale3f(20, 20, 20);
            //would do planet pos here
            
            auto obj = c.me;
            if (obj == nullptr) continue;
            
            glBindTexture(obj->texture);

            nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);

        }
    }
}
void Universe::render_nearby_vessels() {
    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated
        if (v.loaded) {
            //RENDER CODE HERE

        }
    }
}
void Universe::step_on_rails_orbits(Vessel* v) {
    if ( v == nullptr) return;  //Compiler chloroform
    //When you do this: dont forget to apply the phys warp rate to on rails, 
    //when in phys warp.
}
void Universe::step_physics_orbits(Vessel* v) {
    v->physics.step(clock.dt,phys_warp_rate);
}



void Universe::step() {
    clock.tick();
    cam.camera_controller(Camera::FREE);

    //Update focused vessel, in case # of vessels changes. Avoids std::vector fuckery messing with the pointer i think?
    //Is this necessary? if so, i should recalc this ONLY if the vessels.size() changes.
    focused_vessel = nullptr;
    for (Vessel &v : vessels) {
        if (v.is_focused) {
            focused_vessel = &v;    break;
        }
    }



    //Timewarp keys, move this to a TimewarpController
    if (isKeyPressed(KEY_NSPIRE_Z)) {
        phys_warp_rate *= 2;
    }
    if (isKeyPressed(KEY_NSPIRE_X)) {
        phys_warp_rate = 1; if (phys_warp_rate <= 1) phys_warp_rate = 1;
    }

    //Debug to swap texture of planet
    if(isKeyPressed(KEY_NSPIRE_ENTER)) {
        if (celestials[0].switch_texture("luna.png") != 0) {
            printf("ERROR\n");
        }
    }

    //Step vessel orbits
    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated
        if (v.loaded) {
            step_physics_orbits(&v);
            
        } else {
            step_on_rails_orbits(&v);
        }
    }



    //Deltatime stuff
    cam.dt = clock.dt;
    universal_time += (clock.dt * phys_warp_rate) * rails_warp_rate;

    //Render
    render();
}

void Universe::render() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //IN CAM
    glPushMatrix();
    //Move back
    cam.pos.x = 0;
    cam.pos.y = 0;
    cam.pos.z = -200;
    glTranslatef(-cam.pos.x, -cam.pos.y, -cam.pos.z);
    //Camera rotation
    

    //Gonna have to configulate this for orbit mode
    //camera.cpp has kinda a way to do the auto mode using sub modes.
    if (cam.mode == Camera::FREE) {
        linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        nglRotateX(out.x);
        nglRotateY(out.y);
        nglRotateZ(out.z);
    }
    //IN PLANETS
    glPushMatrix();
    

    render_celestials();
    //OUT PLANETS
    glPopMatrix();
    //IN VESSELS
    glPushMatrix();

    render_nearby_vessels();
    //OUT VESSELS
    glPopMatrix();
    //OUT CAM
    glPopMatrix();
    
}


