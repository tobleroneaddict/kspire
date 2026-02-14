#include "universe.h"

void Universe::render_nearby_vessels() {
    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated
        if (v.loaded) {
            //RENDER CODE HERE

        }
    }
}
void Universe::step_rails_orbit_for_v(Vessel* v) {
    v->orbit.calculate_state_from_keplers(universal_time);
}
void Universe::step_physics_orbit_for_v(Vessel* v) {
    if ( v == nullptr) return;
    v->physics.step(clock.dt,timewarp.warp_rate);
    v->orbit.mu = 3.986004418e14;   //NEED TO GET FOR HOME BODY

}


void Universe::step() {
    clock.tick();
    cam.camera_controller(Camera::FREE);

    timewarp.tick(0);
    timewarp.lerp_rate(clock.dt);

    //planetarium.update_planet_positions(universal_time);
    planetarium.celestials[2].orbit.calculate_state_from_keplers(universal_time);

    //Rails enter/exit handling

    if (timewarp.entered_rails) {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                
                
                v.orbit.physics_to_rails(v.physics.POS,v.physics.VEL,universal_time);
            }
        }
    }
    if (timewarp.exited_rails) {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                //v.physics.POS = v.orbit.POS;
                //v.physics.VEL = v.orbit.VEL;
                
                //Need to do rails to physics
            }
        }

    }

    //Every tick that is rails
    if (!timewarp.is_physics_warp) {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                //v.physics.POS = v.orbit.POS;
                //v.physics.VEL = v.orbit.VEL;
            }
        }   
    } else {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                v.orbit.POS = v.physics.POS;
                v.orbit.VEL = v.physics.VEL;
            }
        }
    }

    //Update focused vessel, in case # of vessels changes. Avoids std::vector fuckery messing with the pointer i think?
    //Is this necessary? if so, i should recalc this ONLY if the vessels.size() changes.
    focused_vessel = nullptr;
    for (Vessel &v : vessels) {
        if (v.is_focused) {
            focused_vessel = &v;    break;
        }
    }
    planetarium.focused_vessel = focused_vessel;

    //Debug to swap texture of planet
    if(isKeyPressed(KEY_NSPIRE_ENTER)) {
        if (planetarium.celestials[2].switch_texture("body/luna.png") != 0) {
            printf("ERROR\n");
        }
    }

    //Step vessel orbits
    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated AND LOADED
        if (v.loaded && timewarp.is_physics_warp) {
            step_physics_orbit_for_v(&v);
            
        } else {
            step_rails_orbit_for_v(&v);
        }
    }



    //Deltatime stuff
    cam.dt = clock.dt;
    universal_time += (clock.dt * timewarp.warp_rate);

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

    planetarium.render_celestials();

    render_nearby_vessels();

    //OUT CAM
    glPopMatrix();
    
}

