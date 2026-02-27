#pragma once
#include "../globals.h"
#include "../Utility/GameTexture.h"
#include "../Utility/font.h"
//Universal menu

struct Menu_Item {
    std::string label = "";
    std::string value = "null";
    int x_offset = 0;
    void (*on_click)() = nullptr;

};

class Menu {
public:
    std::string titlebar = "";

    //Position
    int x = 0; int y = 0;
    int h = 0; int w = 0;
    //Overrides position
    bool centered_to_screen = false;
    //Titlebar is an extra 16 pixels
    bool show_title_bar = true;
    bool titlebar_centered = true;
    void init(Bundle* resources,TEXTURE* _screen,
        int _x, int _y, int _h, int _w,
    Fonts* _fonts);

    void add_item(std::string label, void(*callback)(),int x_offset);
    std::vector<Menu_Item> items;

    void Update();
    
    //Which one is green (selected)
    int select_base = 4;

    //Allow 5pt jumping with L/R keys?
    bool allow_scroll_jump = true;

private:
    GameTexture texture_set;
    TEXTURE* screen;
    
    Fonts* fonts;
    int scroll = 0;
    int buttons_wiped = false;
    void render_frame();

    void setup_settings();
    void setup_manage_saves();
};