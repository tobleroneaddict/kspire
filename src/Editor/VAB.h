#pragma once
#include "../globals.h"
#include "../World/Camera/camera.h"


class GameTexture {
public:
    std::vector<uint16_t> tex_data;
    TEXTURE tex;
    int x, y;
    int init(Bundle* resources,std::string path);
};

class VAB {
public:
    TEXTURE *screen;
    ModelGroup me;
    //Load ANGEL model
    int load_model(Bundle* assets);
    void render(ProcessedPosition *processed);
private:
    std::vector<ngl_object*> full_scene;
    Camera cam;
    DT clock;
    
    float camera_height = 30;
    float camera_angle = 30;
    float camera_zoom = -20;

    const float MAX_CAMERA_HEIGHT = 740; //Max cam height

    GameTexture side_panel;

    
};