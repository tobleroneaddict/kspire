#include "universe.h"
#include "../include/rapidjson/document.h"
#include "../include/rapidjson/writer.h"
#include "../include/rapidjson/stringbuffer.h"

using namespace rapidjson;


void Universe::render_celestials() {
    
    if (focused_vessel == nullptr) { printf("E 218754: Cannot render planets!\n");return;}
    if (celestials.size() == 0) { printf("E 128585: Cannot render planets!\n");return;}

    //Should i do distance from CAMERA instead??????

    for (CelestialBody& c : celestials) {
        glPushMatrix();
        int mode = 0;
        //Check if vessel is low enough to render in nearby mode
        if (1 == 5) {
            mode = 1;
        }

        if (mode) {
            //Mode 1 PQS
            printf("Mode 1 not implemented!\n");
        } else {
            auto obj = c.me;
            if (obj != nullptr)
            {
                //Mode 0 Distant
                //Planet renderer works by scaling the glscale3f, and keeping the planet at a fixed distance

                //Vessel coordinate in planet space, per planet
                float v_x = focused_vessel->physics.POS.x - c.POS.x;
                float v_y = focused_vessel->physics.POS.y - c.POS.y;
                float v_z = focused_vessel->physics.POS.z - c.POS.z;
                
                float len = linalg::length(focused_vessel->physics.POS - c.POS);
                 
                //3000 meter bubble
                float fixed_bubble = 3000;
                glTranslatef(
                    -(v_x  / len)* fixed_bubble        * 1,
                    -(v_y  / len)* fixed_bubble        * 1,
                    -(v_z   / len)* fixed_bubble       * 1
                );

                float angular_diameter = 2.0f * (c.radius / len);
                float render_radius = angular_diameter * fixed_bubble;
                render_radius /=5;

                glScale3f(render_radius, render_radius, render_radius);
                glBindTexture(obj->texture);
                nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);
                
                //reformat
                if (focused_vessel != nullptr) {
                    if (c.name == "Earth") {
                        focused_vessel->protoVessel.altitude = len;
                        
                    }
                    
                }


            }
        }
        glPopMatrix();
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
void Universe::step_rails_orbit_for_v(Vessel* v) {
    if ( v == nullptr) return;  //Compiler chloroform
    //When you do this: dont forget to apply the phys warp rate to on rails, 
    //when in phys warp.
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

                //Need to do rails to physics
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


    //Debug to swap texture of planet
    if(isKeyPressed(KEY_NSPIRE_ENTER)) {
        if (celestials[2].switch_texture("body/luna.png") != 0) {
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

    render_celestials();

    render_nearby_vessels();

    //OUT CAM
    glPopMatrix();
    
}

//Move this to celestialbody?
int Universe::load_celestial_bodies(std::vector<CelestialBody> *celestials, Bundle* resources) {
    printf("LOADING BODIES FROM JSON\n");
    std::vector<uint8_t> raw = resources->load_raw_data("resources/system/system.json");
    std::string json(raw.begin(),raw.end());

    Document d;
    d.Parse(json.c_str(),json.size());

    if (!d.HasMember("Bodies") || !d["Bodies"].IsArray()) {
        printf("E 13418: Missing celestials!\n");
        return 2;
    }

    const rapidjson::Value& bodies = d["Bodies"];

    //Iterate bodies
    for (rapidjson::SizeType i = 0; i < bodies.Size(); i++) {
        const rapidjson::Value& body = bodies[i];
        if (!body.IsObject()) continue;

        if (!body.HasMember("Orbit")) {
            printf("E 13418: Missing orbit!\n");
            return 3;
        }
        //Read Orbit data
        const rapidjson::Value& orbit = body["Orbit"];

        CelestialBody cb;

        //Grab values
        const char* s_name      = body["name"].GetString();
        const char* s_parent    = body["parent"].GetString();
        const char* s_radius    = body["radius"].GetString();
        const char* s_mass      = body["mass"].GetString();
        const char* s_rr        = body["rotation_rate"].GetString();
        const char* s_ha        = body["has_atmosphere"].GetString();
        const char* s_atmh      = body["atmosphere_height"].GetString();
        const char* s_slp       = body["sea_level_pressure"].GetString();
        const char* s_mu        = orbit["mu"].GetString();
        const char* s_sma       = orbit["semi_major_axis"].GetString();
        const char* s_ecc       = orbit["eccentricity"].GetString();
        const char* s_inc       = orbit["inclination"].GetString();
        const char* s_lan       = orbit["long_ascending_node"].GetString();
        const char* s_ap        = orbit["argument_of_periapsis"].GetString();
        const char* s_ma        = orbit["mean_anomaly_at_epoch"].GetString();
        const char* s_ep        = orbit["epoch"].GetString();
        
        //Check
        if (
            s_name   == nullptr ||
            s_parent == nullptr ||
            s_radius == nullptr ||
            s_mass   == nullptr ||
            s_rr     == nullptr ||
            s_ha     == nullptr ||
            s_atmh   == nullptr ||
            s_slp    == nullptr
        ) { printf("E 15123: Error parsing object!\n"); return 1; }
        if (
            s_mu     == nullptr ||
            s_sma    == nullptr ||
            s_ecc    == nullptr ||
            s_inc    == nullptr ||
            s_lan    == nullptr ||
            s_ap     == nullptr ||
            s_ma     == nullptr ||
            s_ep     == nullptr
        ) { printf("E 15123: Error parsing orbit!\n"); return 1; }

        if (sizeof(s_name) < 64)
            cb.name = std::string(s_name);
        if (sizeof(s_parent) < 64)
            cb.parent = std::string(s_parent);
        if (sizeof(s_radius) < 64)
            cb.radius = std::stod(s_radius);
        if (sizeof(s_mass) < 64)
            cb.mass = std::stod(s_mass);
        if (sizeof(s_rr) < 64)
            cb.rotation_rate = std::stof(s_rr);
        if (sizeof(s_ha) < 64)
            cb.atmosphere = std::stoi(s_ha);
        if (sizeof(s_atmh) < 64)
            cb.atmosphere_height = std::stoi(s_atmh);
        if (sizeof(s_slp) < 64)
            cb.sea_level_pressure = std::stof(s_slp);
        if (sizeof(s_mu) < 64)
            cb.orbit.mu = std::stod(s_mu);
        if (sizeof(s_sma) < 64)
            cb.orbit.semi_major_axis = std::stod(s_sma);
        if (sizeof(s_ecc) < 64)
            cb.orbit.eccentricity = std::stod(s_ecc);
        if (sizeof(s_inc) < 64)
            cb.orbit.inclination = std::stod(s_inc);
        if (sizeof(s_lan) < 64)
            cb.orbit.long_ascending_node = std::stod(s_lan);
        if (sizeof(s_ap) < 64)
            cb.orbit.argument_of_periapsis = std::stod(s_ap);
        if (sizeof(s_ma) < 64)
            cb.orbit.mean_anomaly = std::stod(s_ma);
        if (sizeof(s_ep) < 64)
            cb.orbit.epoch = std::stod(s_ep);

        celestials->push_back(cb);
        printf("Added body %s.\n",cb.name.c_str());

    }

    return 0;
}