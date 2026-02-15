#include "VAB.h"




void VAB::destroy_model() {
    me.free_group();
    full_scene.clear();
}

int VAB::Start(Bundle* assets, Bundle* parts) {
    
    auto tp_i = touchpad_getinfo();
    tp_h = (float)tp_i->height;
    tp_w = (float)tp_i->width;
    

    if (!hide_vab) {
        if (me.load_group(assets,"resources/vab/vab")) return 1;

        //Load VAB
        //Add objects
        full_scene.push_back(me.get_object("wall"));
        full_scene.push_back(me.get_object("rails"));
        full_scene.push_back(me.get_object("floor"));
        full_scene.push_back(me.get_object("crate"));
    }
    side_panel.init(assets,"resources/ui/pallete.png",screen);
    page_selector.init(assets,"resources/ui/selector.png",screen);
    page_selector.tex.transparent_color = 0x00;

    //Load parts
    if (part_group.load_group(parts,"parts/capsule")) return 1;


    return 0;
}

void VAB::editor_controls() {
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

    if (isKeyPressed(KEY_NSPIRE_G)) {
        show_pallete = false;
    }
    if (isKeyPressed(KEY_NSPIRE_H)) {
        show_pallete = true;
    }


    if (isKeyPressed(KEY_NSPIRE_F) && page_index < 11 && !page_key_held) {
        page_index++;
        page_key_held = true;
    }
    if (isKeyPressed(KEY_NSPIRE_R) && page_index > 0 && !page_key_held) {
        page_index--;
        page_key_held = true;
    }
    if (!isKeyPressed(KEY_NSPIRE_F) && !isKeyPressed(KEY_NSPIRE_R)) page_key_held = false; 

    if (isKeyPressed(KEY_NSPIRE_X) && part_sel_index < 17 && !part_sel_key_held) {
        part_sel_index++;
        part_sel_key_held = true;
    }
    if (isKeyPressed(KEY_NSPIRE_Z) && part_sel_index > 0 && !part_sel_key_held) {
        part_sel_index--;
        part_sel_key_held = true;
    }
    if (!isKeyPressed(KEY_NSPIRE_Z) && !isKeyPressed(KEY_NSPIRE_X)) part_sel_key_held = false; 
}

void VAB::Update() {
    clock.tick();
    editor_controls();
    
    //VAB main code


    
    render();
    cam.dt = clock.dt;
}

void VAB::render() {
    
    linalg::vec<float,3> out = cam.wrapper(); //Get rotation
    camera_zoom = linalg::clamp(camera_zoom,-250,-5);
    
    //Limit cam from going thru top of VAB
    float damper = linalg::sin(out.x * 0.01745329);
    float CAM_LIMITER = MAX_CAMERA_HEIGHT + abs(camera_zoom) * damper;
    float CAM_LIMITER_BOTTOM = linalg::max(abs(camera_zoom) * damper,0);
    
    camera_height = linalg::clamp(camera_height,CAM_LIMITER_BOTTOM,CAM_LIMITER);
    cam.camera_controller(Camera::FREE);
    

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //IN CAM
    glPushMatrix();
    

    float rp_x = linalg::sin(out.y * 0.01745329);
    float rp_y = linalg::cos(out.y * 0.01745329);
    //Gonna have to configulate this for orbit mode
    //camera.cpp has kinda a way to do the auto mode using sub modes.

    glTranslatef(0,0,-camera_zoom);
    //glTranslatef(camera_zoom*rp_x,0,-camera_zoom*rp_y);
    nglRotateX(out.x); //Look up and down
    nglRotateY(out.y);

    glTranslatef(0, -camera_height - 100, 0);
    
    
    
    //VAB
    glPushMatrix();
    glColor3f(0.4f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Offset VAB to center the floor
    glTranslatef(60,0,3);
    glScale3f(100, 100, 100);

    //VAB OFFSET
    
    bool transformed = false;
    for(ngl_object* obj : full_scene) //Loop through AnGL scene
    {
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, !transformed);
        transformed = true;
    }

    glPopMatrix();//OUT VAB

    //Sample part
    glPushMatrix();

    touchpad_report_t touchpad;
    touchpad_scan(&touchpad);
    

    //Default origin
    glTranslatef(0,100,0);

    //Touchpad offsets
    float t_off_x = touchpad.x / tp_w;
    float t_off_y = touchpad.y / tp_h;
    t_off_x -= 0.5f;  t_off_y -= 0.5f;
    
    
    float mult = 1.4f * abs(camera_zoom);

    float c_x = t_off_x * mult * rp_y;
    float c_z = t_off_x * mult * rp_x;
    float c_y = t_off_y * mult * linalg::cos(out.x * 0.01745329) + camera_height;

    //Later: fix this for looking down/up


    glTranslatef(c_x,c_y,c_z);

    glScale3f(10,10,10);

    transformed = false;
    ngl_object* obj = part_group.get_object("MK1");
    glBindTexture(obj->texture);
    nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, !transformed);    



    glPopMatrix();
    //End parts

    glPopMatrix();

    //Show pallete texture at 4x X-res
    if (show_pallete) {
        side_panel.draw(0,0,120,240);
    } else {
        side_panel.draw(0,0,20,240,5,60);
    }

    //Draw selector for page
    page_selector.draw(0,12+(page_index*16));

    //Draw selector for parts
    if (show_pallete) {
        int calc_col = part_sel_index % 3;
        int calc_row = part_sel_index / 3;
        page_selector.draw(20+(32*calc_col),12+(calc_row*32),32,32);
    }


    
}