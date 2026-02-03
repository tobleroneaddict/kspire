#include "universe.h"
void Universe::render_celestials() {

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

            float v_x = focused_vessel->physics.POS.x;
            float v_y = focused_vessel->physics.POS.y;
            float v_z = focused_vessel->physics.POS.z;
            

            float len = sqrtf(v_x*v_x +
            v_y*v_y +
            v_z*v_z);
            
            // float len = sqrtf(physics.POS.x*physics.POS.x +
            //     physics.POS.y*physics.POS.y +
            //     physics.POS.z*physics.POS.z);
                
            //distance_from_planet *= altitude_multiplier;

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
            float angularRadius = mars_radius / len;    //NTS: MAKE THIS REFERENCE CELESTIAL (but make sure it can do moons too so detached. run this for each visible planet?)
            float renderRadius  = angularRadius * fixed_bubble;

            glScale3f(renderRadius, renderRadius, renderRadius);
            

            //glScale3f(20, 20, 20);
            //would do planet pos here

            auto obj = c.me;
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
void Universe::step_on_rails_orbits() {

}
void Universe::step_physics_orbits() {

}



void Universe::step() {
    clock.tick();

    for (Vessel& v : vessels) {
        //Step vessel orbit after checking if its on rails or simulated
        if (v.loaded) {
            step_physics_orbits();
        } else {
            step_on_rails_orbits();
        }
    }



    cam.dt = clock.dt;

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
    linalg::vec<float,3> out = cam.wrapper();   //Outputs rpy as actual clamped values good for ngl
    nglRotateX(out.x);
    nglRotateY(out.y);
    nglRotateZ(out.z);
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
    

    nglDisplay();
}


int Universe::load_bundles() {
    if (planet_bundle.load_asset_bundle("mars.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }
    return 0;
}
void Universe::free_bundles() {
    planet_bundle.free();
}