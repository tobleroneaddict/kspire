    #include "title.h"

//To avoid memory leaks it uses the nglobject from the loaded planets.
//It was orignally just gonna load it on it's own, but apparently that crashes
//the game after a few retries despite having used the same setup as planetarium.celestials...
void Title::load_title(Bundle* resources,ngl_object* _obj,ngl_object* _moon) {
    obj = _obj;
    moon = _moon;
    angle = 0.0f;

    //Move this to a new delete
    texture_set.init(resources,"resources/ui/title.png",screen);
}

int Title::Update() {
    clock.tick();
    if (isKeyPressed(K_PAD_S) && buttons_wiped)
        {select_index++; buttons_wiped = false;}
    if (isKeyPressed(K_PAD_N) && buttons_wiped)
        {select_index--; buttons_wiped = false;}
    if (!isKeyPressed(K_PAD_N) && !isKeyPressed(K_PAD_S)) buttons_wiped = true;

    if (select_index < 0) select_index = 4 + select_index; //Loop
    else if (select_index > 3) select_index -= 4;

    //Click events
    if (select_index == 3 && isKeyPressed(K_ENTER)) {
        return -1;
    }


    //Start game. In the release, this will not return 600 just yet. There will be a save loading menu 
    if (select_index == 0 && isKeyPressed(K_ENTER)) {
        return 600;
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Render
    if (obj != nullptr) {

        
        glPushMatrix();

        glTranslatef(30,-23,100);

        glScale3f(60,60,60);

        float a = fmod(fmod(angle, 360.0f) + 360.0f, 360.0f);
        nglRotateZ(20);
        nglRotateY(360.0f - a);
        
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);
        glPopMatrix();

        
        
    }
    //Title screen
    texture_set.draw(0,0,229,81,229,81);
    //Orbit line
    drawTextureOverlay(texture_set.tex,0,82,*screen,0,0,229,92);
    //Main Buttons
    drawTexture(texture_set.tex,*screen,0,174,149,75,SCREEN_WIDTH-149,SCREEN_HEIGHT-75,149,75);

    //Menu buttons
    switch (select_index) {
        case 0: drawTexture(texture_set.tex,*screen,0,249,149,23,SCREEN_WIDTH-149,SCREEN_HEIGHT-75,149,23);
        break;
        case 1: drawTexture(texture_set.tex,*screen,0,249,149,23,SCREEN_WIDTH-149,SCREEN_HEIGHT-49,149,23);
        break;
        case 2: drawTexture(texture_set.tex,*screen,0,275
            ,89,23,SCREEN_WIDTH-149,SCREEN_HEIGHT-23,89,23);
        break;
        case 3: drawTexture(texture_set.tex,*screen,92,275
            ,57,23,SCREEN_WIDTH-58,SCREEN_HEIGHT-23,57,23);
        break;
        default:
        break;
    }
        
    angle += 20.0f * clock.dt;

    return 0;
}


void Title::pack_title() {
    texture_set.tex.bitmap = nullptr;
    texture_set.tex_data.clear();
}
