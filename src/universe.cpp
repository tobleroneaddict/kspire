#include "universe.h"

void Universe::render_nearby_vessels() {
    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated
        if (v.loaded) {
            //RENDER CODE HERE

            
        }
        for(Part &p : v.part_tree) //Loop through AnGL scene
        {


            if (parts_master->get_part_by_id(p.shared_id) == nullptr) continue;
            if (parts_master->get_part_by_id(p.shared_id)->models.size() == 0) continue;
            if (parts_master->get_part_by_id(p.shared_id) == nullptr) continue;
            auto part = parts_master->get_part_by_id(p.shared_id);
            if (part->models.empty()) continue;
            
            auto obj = parts_master->get_part_by_id(p.shared_id)->models[0]; //Only first object for now

            glPushMatrix();
            //Standard origin
            glTranslatef(0,0,0);

            //Offset by part pos
            glTranslatef(p.pos.x,
                p.pos.y,
                p.pos.z);
                //will need to do attPos too and in VAB

            glScale3f(10,10,10);
            glBindTexture(obj->texture);
            nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, true);

            glPopMatrix();
            
        }

    }
}
void Universe::step_rails_orbit_for_v(Vessel* v) {
    v->orbit.calculate_state_from_keplers(universal_time);
}
void Universe::step_physics_orbit_for_v(Vessel* v) {
    if ( v == nullptr) return;
    //v->home_body = planetarium.get_attractor(v);

    double calc_mu = planetarium.celestials[v->home_body].mass * 6.6743E-11;
    v->orbit.mu = calc_mu;

    v->orbit.physics_step(clock.dt,timewarp.warp_rate);
    //v->orbit.physics_to_rails(universal_time);
    //v->orbit.calculate_state_from_keplers(universal_time);
}


void Universe::Update() {
    clock.tick();
    cam.camera_controller(Camera::ORBIT);

    timewarp.tick(0);
    timewarp.lerp_rate(clock.dt);

    //Deltatime stuff
    cam.dt = clock.dt;
    universal_time += (clock.dt * timewarp.warp_rate);

    planetarium.update_planet_positions(universal_time);

/*    printf("2POS: (%f,%f,%f)\n", focused_vessel->orbit.POS.x/1000.0f, 
                                 focused_vessel->orbit.POS.y/1000.0f, 
                                 focused_vessel->orbit.POS.z/1000.0f);
    printf("2VEL: (%f,%f,%f)\n", focused_vessel->orbit.VEL.x/1000.0f, 
                                 focused_vessel->orbit.VEL.y/1000.0f, 
                                 focused_vessel->orbit.VEL.z/1000.0f);
    printf("2ECC: %f\n",         focused_vessel->orbit.eccentricity);
*/

    //Rails enter/exit handling
    //Oneshot
    if (timewarp.entered_rails) {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                v.orbit.physics_to_rails(universal_time);
                //v.orbit.calculate_state_from_keplers(universal_time);
            }
        }
        timewarp.entered_rails = false;
    }
    if (timewarp.exited_rails) {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                //Need to do rails to physics (no)
            }
        }
        timewarp.exited_rails = true;
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
    if(isKeyPressed(K_ENTER)) {
        for (Vessel &v : vessels) {
            if (v.loaded) {
                v.orbit.physics_to_rails(universal_time);
                //v.orbit.calculate_state_from_keplers(universal_time);
            }
        }
        //o->calculate_state_from_keplers(universal_time);
        //o->epoch = 0;
        //o->eccentricity = 0.001;
        //o->inclination = 0.01f;
        //o->long_ascending_node = 20;
        //o->mean_anomaly = 15;
        //o->mean_anomaly_at_epoch = 1;
        //o->semi_major_axis = 4250.0f * 1000.0f;
        //o->calculate_state_from_keplers(0);
    }
    //Map view zoom
    if (isKeyPressed(K_EDITOR_DOWN)) {
        map_zoom--;
    }
    if (isKeyPressed(K_EDITOR_UP)) {
        map_zoom++;
    }

    if (isKeyPressed(K_MAP) && map_button_held == false) {
        map_button_held = true;
        in_map_view = !in_map_view;
    }
    if (!isKeyPressed(K_MAP)) map_button_held = false;

    //printf("eph: %f\n",focused_vessel->orbit.epoch);
    //printf("ecc: %f\n",focused_vessel->orbit.eccentricity);
    //printf("incD: %f\n",focused_vessel->orbit.inclination * 57.29);
    //printf("lan: %f\n",focused_vessel->orbit.long_ascending_node);
    //printf("ma : %f\n",focused_vessel->orbit.mean_anomaly);
    //printf("mae : %f\n",focused_vessel->orbit.mean_anomaly_at_epoch);
    //printf("aop : %f\n",focused_vessel->orbit.argument_of_periapsis);
    




    //Step vessel orbits
    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated AND LOADED
        if (v.loaded && timewarp.is_physics_warp) {
            step_physics_orbit_for_v(&v);
            
            
        } else {
            step_physics_orbit_for_v(&v);
            //printf("IN\n");
            //step_rails_orbit_for_v(&v);
            //            printf("OUT\n");

        }
    }

    #ifndef M_PI
    const double M_PI = 3.14159265;
    #endif
    const double TWO_PI = 2.0 * M_PI;
    const double RAD_TO_DEG = 57.29578;

    //Tesitng
    //auto im = linalg::normalize(focused_vessel->orbit.POS);
    //cam.yaw = linalg::atan2(im.x, im.y) * RAD_TO_DEG ;
 
    
    
    for (CelestialBody& c : planetarium.celestials) { 

        double ang = fmod(universal_time / 21549.425 * TWO_PI, TWO_PI);
        c.angle = ang * RAD_TO_DEG;
    }

    //Render
    
    if (in_map_view) {render_map();}
    else {render_flight();}

    
}

//Render Map view
void Universe::render_map() {
    render_skybox();

    glPushMatrix();
    //Move back
    cam.pos.x = 0;
    cam.pos.y = 0;
    cam.pos.z = map_zoom;
    glTranslatef(-cam.pos.x, -cam.pos.y, -cam.pos.z);
    //Camera rotation
    
    //Gonna have to configulate this for orbit mode
    //camera.cpp has kinda a way to do the auto mode using sub modes.
    if (cam.mode == Camera::ORBIT) {
        linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        nglRotateX(out.x);
        nglRotateZ(out.z);
        nglRotateY(out.y);
    }
    if (cam.mode == Camera::FREE) {
        linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        nglRotateX(out.x);
        nglRotateY(out.y);
        nglRotateZ(out.z);
    }

    //Modify the POS stuff to work in map view
    planetarium.render_celestials(100);


    glPopMatrix();
}


//Render flight view
void Universe::render_flight() {
    render_skybox();
    
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
    if (cam.mode == Camera::ORBIT) {
        linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        nglRotateX(out.x);
        nglRotateZ(out.z);
        nglRotateY(out.y);
    }
    if (cam.mode == Camera::FREE) {
        linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        nglRotateX(out.x);
        nglRotateY(out.y);
        nglRotateZ(out.z);
    }


    planetarium.render_celestials(20000);

    render_nearby_vessels();

    //OUT CAM
    glPopMatrix();
    
}

//Pack away the flight scene
void Universe::pack() {
    planetarium.clear_celestial_models();
    skybox.free_group();

    vessels.clear();

}


void Universe::render_skybox() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    //IN SKYBOx
    glPushMatrix();
    
    glTranslatef(0,0,0);
    linalg::vec<float,3> out = cam.wrapper();
    if (cam.mode == Camera::ORBIT) {
        //linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        //if (out.x != NAN && out.y != NAN && out.z != NAN) {
            nglRotateX(out.x);
            nglRotateZ(out.z);
            nglRotateY(out.y);
        //}
    }
    
    if (cam.mode == Camera::FREE) {
        //linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
        //if (out.x != NAN && out.y != NAN && out.z != NAN) {
            nglRotateX(out.x);
            nglRotateY(out.y);
            nglRotateZ(out.z);
        //}
    }
    
    //Horrific camera error
    //if (out.x == NAN && out.y == NAN && out.z == NAN) return;


    glScale3f(500,500,500);
    auto obj = &skybox.objects[0];
    
    if (obj != nullptr) {
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);
    }

    glPopMatrix();
    
    glClear(GL_DEPTH_BUFFER_BIT);
}