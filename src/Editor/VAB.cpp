#include "VAB.h"

void VAB::destroy_model() {
    me.free_group();
    node_g.free_group();
    full_scene.clear();
    part_tree.clear();

    test_pids.clear();
}

//Init script
int VAB::Start(Bundle* assets) {
    destroy_model();
    part_tree.clear();

    //DEBUG STUFF
    Part part;
    part = parts_master->get_part_by_id(5817571)->default_data;
    part.unique_id = (unsigned int)&part % 10000000;
    for (unsigned int i = 0; i < part.nodes.size(); i++) {
        part.nodes[i].unique_id = i + (part.unique_id * 10);
    }

    part_tree.push_back(std::move(part));
    Part part2;
    part2 = parts_master->get_part_by_id(5817571)->default_data;
    part2.unique_id = (unsigned int)&part2 % 10000000;

    for (unsigned int i = 0; i < part2.nodes.size(); i++) {
        part2.nodes[i].unique_id = i + (part2.unique_id * 10);
    }
    part2.pos.x += 5;
    part_tree.push_back(std::move(part2));
    //END DEBUG STUFF


    //Render VAB scene For release
    if (!hide_vab) {
        if (me.load_group(assets,"resources/vab/vab")) return 1;
        full_scene.push_back(me.get_object("wall"));
        full_scene.push_back(me.get_object("rails"));
        full_scene.push_back(me.get_object("floor"));
        full_scene.push_back(me.get_object("crate"));
    }

    side_panel.init(assets,"resources/ui/pallete.png",screen);
    page_selector.init(assets,"resources/ui/selector.png",screen);
    page_selector.tex.transparent_color = 0x00;


    //Node
    if (node_g.load_group(assets,"resources/vab/node")) return 1;
    node = node_g.get_object("Sphere");


    //Read part categories
    test_pids = parts_master->get_parts_of_category("Command");


    return 0;
}

//Inputs for editor
void VAB::editor_controls() {
    float rot_speed = 160.0f * clock.dt;

    //Editor specific camera controls
    if (isKeyPressed(KEY_NSPIRE_CTRL)) camera_zoom -= rot_speed;
    if (isKeyPressed(KEY_NSPIRE_SHIFT)) camera_zoom += rot_speed;

    if (isKeyPressed(KEY_NSPIRE_Q) || isKeyPressed(KEY_NSPIRE_TAB)) camera_height -= rot_speed;
    if (isKeyPressed(KEY_NSPIRE_E) || isKeyPressed(KEY_NSPIRE_SCRATCHPAD)) camera_height += rot_speed;

    //Keyboard pallete show hide
    // if (isKeyPressed(KEY_NSPIRE_G)) show_pallete = false;
    // if (isKeyPressed(KEY_NSPIRE_H)) show_pallete = true;


    //Keyboard Paging (PC)
    if (isKeyPressed(KEY_NSPIRE_F) && page_index < 11 && !page_key_held) {
        page_index++;
        page_key_held = true;
    }
    if (isKeyPressed(KEY_NSPIRE_R) && page_index > 0 && !page_key_held) {
        page_index--;
        page_key_held = true;
    }
    if (!isKeyPressed(KEY_NSPIRE_F) && !isKeyPressed(KEY_NSPIRE_R)) page_key_held = false;

    //Keyboard Part selector
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

//Actions for mouse click, happens once on click
void VAB::onClick_oneshot() {

    //Palette action
    if (show_pallete && kspire_pad.x_screen > 20) {
        printf("Grabbed new part.\n");
        if (has_grabbed_part) { //Holding part?
            for (size_t i = 0; i < part_tree.size(); ++i) {
                if (&part_tree[i] == grabbed_part) {
                    part_tree.erase(part_tree.begin() + i);
                    has_grabbed_part = false;
                    break;
                }
            }
        } else { //Create new part
            Part part;
            part = parts_master->get_part_by_id(5817571)->default_data;
            part.unique_id = (unsigned int)&part % 10000000;
            for (unsigned int i = 0; i < part.nodes.size(); i++) {
                part.nodes[i].unique_id = i + (part.unique_id * 10);
            }
            part.pos = {0,-500,0 };
            //Shove it in tree
            part_tree.push_back(std::move(part));
            //Put it in the hand
            has_grabbed_part = true;
            grabbed_part = &part_tree.back();
        }
    }

    //Check for part grab with raycast (Uses stale camera rotation but its okay)
    else if (!has_grabbed_part) {
        int found = -1;
        auto point = raycast_camera(current_cam_rotation);
        
        for (size_t i = 0; i < part_tree.size(); ++i) {
            float off = linalg::length(part_tree[i].pos - point);
            if (off < part_raycast_threshold) {
                found = i;
                has_grabbed_part = true;
                grabbed_part = &part_tree[i];
                break;
            }
        }

        if (found != -1)
            part_tree[found].pos = point;
    } else {
        has_grabbed_part = false;
        stopped_grabbing = true;
        grabbed_part = nullptr;
    }


}

//VAB
void VAB::Update() {
    clock.tick();

    editor_controls();
    
    //VAB main code
    tsx_o = tsx; tsy_o = tsy;

    //READ TOUCHPAD XY

    tsx = kspire_pad.x_screen;
    tsy = kspire_pad.y_screen;

    //Mouse moved
    bool m_moved = (tsx != tsx_o || tsy != tsy_o);

    //Pallete::mouse to pallete
    if (tsx < 16 && !show_pallete) show_pallete = true;
    if (tsx > 115 && show_pallete) show_pallete = false;
    if (m_moved && tsx < 16)  {
        int num = tsy - 9;
        num /= 16;
        page_index = num;
        page_index = 11-linalg::clamp(page_index,0,11);
    }
    if (m_moved && tsx > 16 && tsx < 115 && tsy < 200) {
        int col = (tsx - 20) / 32;
        int row = ((210-tsy) - 12) / 32;
        if (row > 5) row = 5;
        part_sel_index = row * 3 + col;
    }


    //Translate grabbed part
    if (has_grabbed_part) {
        for(Part &p : part_tree)
        {
            if (&p == grabbed_part)
                p.pos = raycast_camera(current_cam_rotation);
        }
    }


    //TODO: make this node::size dependent
    float snap_thresh = 6.0f;
    float mult = 22.0f;
    //Snapping
    if (has_grabbed_part) {
        for(Part &p : part_tree)
        {
            if (&p == grabbed_part) continue; //Skip self

            for (Node &n : p.nodes) { //Client nodes
                auto client_pos = (mult*n.position) + p.pos;
                for (Node &n_2 : grabbed_part->nodes) { //Host node
                    auto host_pos = (mult*n_2.position) + grabbed_part->pos;
                    float len = linalg::length(host_pos - client_pos);
                    if (len < snap_thresh) {
                        grabbed_part->pos = client_pos - (mult*n_2.position);
                        // n.attached_node = n_2.unique_id;    //Link
                        // n_2.attached_node = n.unique_id;    //Link
                        break;
                    }
                }

            }
        }
    }






    //Click oneshot events
    if (kspire_pad.pressed && !pad_held) {
        pad_held = true;
        onClick_oneshot();
    }
    
    if (!has_grabbed_part && !stopped_grabbing) cam.camera_controller(Camera::FREE);
    if (!kspire_pad.pressed) {pad_held = false; stopped_grabbing = false;}//release


    render();
    cam.dt = clock.dt;
    pallete_r += clock.dt * 50;
}




//VAB Render call
void VAB::render() {

    current_cam_rotation = cam.wrapper(); //Get rotation
    camera_zoom = linalg::clamp(camera_zoom,-250,-5);

    //Limit cam from going thru top of VAB
    float damper = linalg::sin(current_cam_rotation.x * 0.01745329);
    float CAM_LIMITER = MAX_CAMERA_HEIGHT + abs(camera_zoom) * damper;
    float CAM_LIMITER_BOTTOM = linalg::max(abs(camera_zoom) * damper,0);

    camera_height = linalg::clamp(camera_height,CAM_LIMITER_BOTTOM,CAM_LIMITER);



    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //IN CAM
    glPushMatrix();



    //Gonna have to configulate this for orbit mode
    //camera.cpp has kinda a way to do the auto mode using sub modes.

    glTranslatef(0,0,-camera_zoom);
    //glTranslatef(camera_zoom*rp_x,0,-camera_zoom*rp_y);
    nglRotateX(current_cam_rotation.x); //Look up and down
    nglRotateY(current_cam_rotation.y);

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

    ngl_object* obj;




    //TESTING HIGHLIGHT

    Part* found = nullptr;
    auto point = raycast_camera(current_cam_rotation);
        
    for (size_t i = 0; i < part_tree.size(); ++i) {
        float off = linalg::length(part_tree[i].pos - point);
        if (off < part_raycast_threshold) {
            found = &part_tree[i];
            break;
        }
    }




    //Part tree also contains detached parts. ( No parent )if there are any theyre nuked.
    for(Part &p : part_tree) //Loop through AnGL scene
    {


        if (parts_master->get_part_by_id(p.shared_id) == nullptr) continue;
        if (parts_master->get_part_by_id(p.shared_id)->models.size() == 0) continue;


        obj = parts_master->get_part_by_id(p.shared_id)->models[0]; //Only first object for now
        auto highlit = parts_master->get_part_by_id(p.shared_id)->models.back(); //Highlit
        glPushMatrix();
        //Standard origin
        glTranslatef(0,130,0);

        //Offset by part pos
        glTranslatef(p.pos.x,
            p.pos.y,
            p.pos.z);

        glScale3f(10,10,10);
        if (&p == found && highlit != nullptr && !has_grabbed_part) { //Highlight (unless grabbed)
          glPushMatrix();
          glBindTexture(highlit->texture);
          nglDrawArray(highlit->vertices, highlit->count_vertices, highlit->positions, highlit->count_positions, processed, highlit->draw_mode, true);
          glPopMatrix();
        } else {
            glBindTexture(obj->texture);
            nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, true);
        }

        //Show nodes on grab
         if (has_grabbed_part /* && grabbed_part == &p*/) {
            for (Node &n : p.nodes) {
                glPushMatrix();

                //idk why its *2 but yeah it works
                glTranslatef(n.position.x * 2.2f
                            ,n.position.y * 2.2f
                            ,n.position.z * 2.2f
                );

                //Scale nodes as per Node::size
                float node_scale = 0.45f;
                node_scale += 0.3f * n.size;

                glScale3f(node_scale,node_scale,node_scale);

                glBindTexture(node->texture);

                nglDrawArray(node->vertices, node->count_vertices, node->positions, node->count_positions, processed,
                    node->draw_mode, true);
                glPopMatrix();
            }
        }

        glPopMatrix();
    }


    glPopMatrix(); //End Camera

    //Show pallete texture at 4x X-res
    if (show_pallete) {
        side_panel.draw(0,0,120,240);
    } else {
        side_panel.draw(0,0,20,240,5,60);
    }

    //Draw selector for page
    page_selector.draw(0,12+(page_index*16));


    //Clear depth buffer before 3D rendering pallete
    glClear(GL_DEPTH_BUFFER_BIT);

    //Display pallete parts
    if (show_pallete) {
        for (int psi = 0; psi <= 17; psi++) {
            int calc_col = psi % 3;
            int calc_row = psi / 3;
            calc_col = 20+(37*calc_col); calc_row = 12+(calc_row*37);
            calc_row = 20 - calc_row;


            glPushMatrix();

            obj = parts_master->get_part_by_id(test_pids[0])->models[0]; //Only first object for now

            glTranslatef(
                 calc_col - 163
                ,calc_row + 98
                ,300
            );
            nglRotateY(fmod(pallete_r,360.0f));

            glScale3f(10,10,10);
            glBindTexture(obj->texture);
            nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode, true);

            glPopMatrix();

        }
    }

    //Draw selector for parts
    if (show_pallete) {
        int calc_col = part_sel_index % 3;
        int calc_row = part_sel_index / 3;
        page_selector.draw(20+(32*calc_col),12+(calc_row*32),32,32);
    }



}


//Raycast point from camera to editor space, for part selection.
//This doesnt actually raycast, it's plane changes with camera rotation about the Y. this sucks, but will suffice
//until I can get a true raycaster working.
linalg::vec<float,3> VAB::raycast_camera(linalg::vec<float,3> out /*camera out*/) {

    float rp_x = linalg::sin(out.y * 0.01745329);
    float rp_y = linalg::cos(out.y * 0.01745329);
    //Temporary solution rn
    float t_off_x = kspire_pad.x_screen_normalized;
    float t_off_y = kspire_pad.y_screen_normalized;

    t_off_x -= 0.5f;  t_off_y -= 0.5f;

    float mult = 1.4f * abs(camera_zoom);

    float c_x = t_off_x * mult * rp_y;
    float c_z = t_off_x * mult * rp_x;
    float c_y = t_off_y * mult * linalg::cos(out.x * 0.01745329) + camera_height;

    return {c_x,c_y,c_z};
}