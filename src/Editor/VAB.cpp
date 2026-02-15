#include "VAB.h"

int GameTexture::init(Bundle* resources,std::string path) {
    

    std::vector<unsigned char> image;
    unsigned width, height;

    tex_data.clear();  //make sure it's empty

    std::vector<uint8_t> data = resources->load_raw_data(path.c_str()    );

    printf("sz %d\n",data.size());
    
    if (data.size() > 0) {
        unsigned error = lodepng::decode(
            image, width, height, 
            data,   //Load raw PNG data from asset bundle
            LodePNGColorType::LCT_RGBA, //Color mode
            8   //Bit depth
        );
        
        if (!error) {
            tex_data.resize(width * height);

            //Snatch pixel data and store it
            unsigned int pixel = 0;
            for (unsigned int i = 0; i < width * height; i++) {
                unsigned char r = image[pixel + 0];
                unsigned char g = image[pixel + 1];
                unsigned char b = image[pixel + 2];
        
                tex_data[i] =
                    ((r & 0b11111000) << 8) |
                    ((g & 0b11111100) << 3) |
                    ((b & 0b11111000) >> 3);
                pixel += 4;
            }

            //Update nGL TEXTURE reference
            tex.bitmap = tex_data.data();
            tex.width  = width;
            tex.height = height;
        }
        
        
        return error;
    }

    printf("ERROR 32843 VAVBB\n");
    return 1;
}



int VAB::load_model(Bundle* assets) {
    if (me.load_group(assets,"resources/vab/vab")) return 1;

    //Add objects
    full_scene.push_back(me.get_object("wall"));
    full_scene.push_back(me.get_object("rails"));
    full_scene.push_back(me.get_object("floor"));
    full_scene.push_back(me.get_object("crate"));
    
    side_panel.init(assets,"resources/fonts/font_bmp.png");
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
    //printf("LIM: %f\n",CAM_LIMITER);
    
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


    drawTexture(side_panel.tex,*screen,0,0,128,128,50,50,320,240);
    

    cam.dt = clock.dt;
}