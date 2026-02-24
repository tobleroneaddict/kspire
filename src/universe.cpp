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
            glTranslatef(
                p.pos.x+p.attPos.x,
                p.pos.y+p.attPos.y,
                p.pos.z+p.attPos.z);
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
        timewarp.exited_rails = false; //Good? ig
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

    //Map view zoom and also flight ig
    if (isKeyPressed(K_EDITOR_DOWN)) {
        map_zoom-=1000 * clock.dt;
    }
    if (isKeyPressed(K_EDITOR_UP)) {
        map_zoom+=1000 * clock.dt;
    }

    map_zoom = linalg::clamp(map_zoom,-20000,150);
    //printf("z:%f\n",map_zoom);


    if (isKeyPressed(K_MAP) && map_button_held == false) {
        map_button_held = true;
        in_map_view = !in_map_view;
        printf("Map view key\n");
    }
    if (!isKeyPressed(K_MAP)) map_button_held = false;

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

    for (CelestialBody& c : planetarium.celestials) { 

        double ang = fmod(universal_time / 21549.425 * TWO_PI, TWO_PI);
        c.angle = ang * RAD_TO_DEG;
    }

    //Update vessel states
    for (Vessel &v : vessels) {
        //All vessels
        v.Update();
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
    rotate_camera();

    linalg::vec<double,3> pos_d(
        cam.pos.x,
        cam.pos.y,
        cam.pos.z
    );

    //Modify the POS stuff to work in map view
    planetarium.render_celestials(100,true,pos_d);


    glPopMatrix();
}


//Render flight view
void Universe::render_flight() {
    
    CelestialBody* e = &planetarium.celestials[planetarium.find_body_by_name("Earth")];
    float alt = linalg::length(focused_vessel->orbit.POS);
    alt -= e->radius*2.0f;
    float intensity = e->get_atm_intensity(alt);

    glColor3f(intensity*((float)0x87/(float)0xFF)
             ,intensity*((float)0xCE/(float)0xFF)
             ,intensity*((float)0xEB/(float)0xFF)
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Render skybox if in space
    if (intensity < 1E-8)
        render_skybox();
    

    //IN CAM: PLANET
    glPushMatrix();
    //Move back
    cam.pos.x = 0;
    cam.pos.y = 0;
    cam.pos.z = 0;
    glTranslatef(-cam.pos.x, -cam.pos.y, -cam.pos.z);
    rotate_camera();

    //You should make this operate from vessel pos + camera
    //planetarium.render_celestials(20000,false,{0,0,0});


    //zero btw
    linalg::vec<double,3> pos_d(
        cam.pos.x,
        cam.pos.y,
        cam.pos.z
    );
    planetarium.render_celestials(20000,false,pos_d);



    //OUT CAM : PLANET
    glPopMatrix();
    //IN CAM: VESSEL
    glPushMatrix();
    //Move back
    cam.pos.x = 0;
    cam.pos.y = 0;
    cam.pos.z = -200+map_zoom;
    
    
    glTranslatef(-cam.pos.x, -cam.pos.y, -cam.pos.z);
    rotate_camera();
    
    glTranslatef(focused_vessel->protoVessel.CoM.x,
    focused_vessel->protoVessel.CoM.y,
    focused_vessel->protoVessel.CoM.z);

    render_nearby_vessels();
    

    //OUT CAM : VESSEL
    glPopMatrix();

    
}

void Universe::rotate_camera() {
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
}



void Universe::render_skybox() {
    
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


//Pack away the flight scene
void Universe::pack() {
    planetarium.clear_celestial_models();
    skybox.free_group();

    vessels.clear();

}