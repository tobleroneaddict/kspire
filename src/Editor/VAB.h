#pragma once
#include "../globals.h"
#include "../World/Camera/camera.h"
#include "../Utility/GameTexture.h"


class VAB {
public:
    TEXTURE *screen;
    
    ProcessedPosition *processed;

    //Reduce pallete to just side buttons
    bool show_pallete = false;
    bool hide_vab = false;

    //Load ANGEL model
    int Start(Bundle* assets, Bundle* parts);
    void destroy_model();
    
    void Update();
private:
    std::vector<ngl_object*> full_scene;
    Camera cam;
    DT clock;
    ModelGroup me;
    ModelGroup part_group;

    float camera_height = 30;
    float camera_angle = 30;
    float camera_zoom = -20;

    const float MAX_CAMERA_HEIGHT = 740; //Max cam height

    GameTexture side_panel;
    GameTexture page_selector;
    
    //Which part group is selected on pallete
    int page_index = 0;
    bool page_key_held = false;
    int part_sel_index = 0;
    bool part_sel_key_held = false;

  
    void render();
    void editor_controls();

    //Touchpad
    float tp_h;
    float tp_w;
};