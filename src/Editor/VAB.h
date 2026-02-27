#pragma once
#include "../globals.h"
#include "../World/Camera/camera.h"
#include "../Utility/GameTexture.h"
#include "../Vessel/Part.h"
#include "../Utility/PartLoader.h"


class VAB {
public:
    TEXTURE *screen;
    
    ProcessedPosition *processed;
    PartLoader* parts_master;

    //Reduce pallete to just side buttons
    bool show_pallete = false;
    bool hide_vab = false;

    std::vector<Part> part_tree;

    //Load ANGEL model
    int Start(Bundle* assets);
    void destroy_model();
    
    void Update();

    void highlight_part(ngl_object* obj,COLOR color); //Move this to Parts?


    //Stuff for the pallete menus
    int page_index = 0;
    bool page_key_held = false;
    int part_sel_index = 0;
    bool part_sel_key_held = false;

private:
    std::vector<ngl_object*> full_scene;
    Camera cam;
    DT clock;
    ModelGroup me;
    ModelGroup part_group;
    ModelGroup node_g;
    ngl_object* node;


    float camera_height = 30;
    float camera_angle = 30;
    float camera_zoom = -20;

    const float MAX_CAMERA_HEIGHT = 740; //Max cam height

    GameTexture side_panel;
    GameTexture page_selector;
    

    //Magic value to denote a detached node
    const int DETACHED_NODE = -1;

    //Holding part?
    bool  has_grabbed_part = false;
    //magic stuff that makes the camera not move
    //when you place down a part
    bool stopped_grabbing = false;
    bool pad_held = false;  //Holding click
    int grabbed_part = -1; //Index
    float part_raycast_threshold = 30.0f;

    void render();
    void editor_controls();
    void onClick_oneshot();
   
    linalg::vec<float,3> raycast_camera(linalg::vec<float,3> out);

    int tsx, tsy, tsx_o,tsy_o;

    linalg::vec<float,3> current_cam_rotation;


    /*    PALLETE RENDERER    */
    //Replace this with a better data type
    std::vector<int> test_pids;
    float pallete_r = 0.0f; //Rotator


    
};