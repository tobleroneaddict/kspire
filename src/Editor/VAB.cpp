#include "VAB.h"


int VAB::load_model(Bundle* assets) {
    if (me.load_group(assets,"resources/vab/vab")) return 1;

    //Add objects
    full_scene.push_back(me.get_object("wall"));
    full_scene.push_back(me.get_object("rails"));
    full_scene.push_back(me.get_object("floor"));
    full_scene.push_back(me.get_object("crate"));
    
    return 0;
}

void VAB::render(ProcessedPosition *processed) {
    clock.tick();

    //VAB Controls
    float rot_speed = 160.0f * clock.dt;

    if (isKeyPressed(KEY_NSPIRE_CTRL)) {
        camera_zoom -= rot_speed;
    }
    if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
        camera_zoom += rot_speed;
    }
    if (isKeyPressed(KEY_NSPIRE_Q)) {
        camera_height -= rot_speed;
    }
    if (isKeyPressed(KEY_NSPIRE_E)) {
        camera_height += rot_speed;
    }

    linalg::vec<float,3> out = cam.wrapper(); //Get rotation
    float damper = linalg::sin(out.x * 0.01745329);

   
    camera_zoom = linalg::clamp(camera_zoom,-250,-5);

    //Limit cam from going thru top of VAB
    float CAM_LIMITER = MAX_CAMERA_HEIGHT + abs(camera_zoom) * damper;
    printf("LIM: %f\n",CAM_LIMITER);
    
    float CAM_LIMITER_BOTTOM = linalg::max(abs(camera_zoom) * damper,0);
    
    camera_height = linalg::clamp(camera_height,CAM_LIMITER_BOTTOM,CAM_LIMITER);
    cam.camera_controller(Camera::FREE);
    

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //IN CAM
    glPushMatrix();
    //Move back
    //cam.pos.z = camera_zoom;
    //Camera rotation
    
    
    //Gonna have to configulate this for orbit mode
    //camera.cpp has kinda a way to do the auto mode using sub modes.
    if (cam.mode == Camera::FREE) {
           //Outputs rpy as actual clamped values good for ngl
        nglRotateX(out.x);
        nglRotateY(out.y);
    }
    float rp_x = linalg::sin(out.y * 0.01745329);
    float rp_y = linalg::cos(out.y * 0.01745329);
    glTranslatef(camera_zoom*rp_x,0,-camera_zoom*rp_y);
    glTranslatef(0, -camera_height - 100, 0);
    
    glPushMatrix();
    glColor3f(0.4f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glScale3f(100, 100, 100);

    bool transformed = false;
    for(ngl_object* obj : full_scene) //Loop through AnGL scene
    {
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, !transformed);
        transformed = true;
    }

    glPopMatrix();

    glPopMatrix();

    cam.dt = clock.dt;
}