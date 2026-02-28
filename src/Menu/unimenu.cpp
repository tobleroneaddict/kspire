#include "unimenu.h"



void Menu::init(Bundle* resources,TEXTURE* _screen, int _x, int _y, int _h, int _w,Fonts* _fonts) 
{
    h = _h;    w = _w;
    x = _x;    y = _y;
    this->screen = _screen;
    texture_set.init(resources,"resources/ui/unimenu.png",screen);
    fonts = _fonts;
    scroll = 0;
}

void Menu::add_item(std::string label, std::function<void(Menu_Item*)> callback,
int x_offset, int id) 
{
    Menu_Item item;
    item.label = label; item.id = id;
    item.on_click = callback;
    item.x_offset = x_offset;
    items.push_back(item);
}

void Menu::Update() 
{
    if (texture_set.tex.height == 0) return;
    if (h <= 0 || w <= 0) return;

    render_frame();
 
    //Need to add Scroll

    int text_cutoff_y = 0;
    if (centered_to_screen) {
        text_cutoff_y = ((SCREEN_HEIGHT + h) / 2)-16;
    } else {
        text_cutoff_y = y+h;
    }

    //Render items
    int calc_x = 0;
    if (centered_to_screen) {
        calc_x = (SCREEN_WIDTH - w) / 2;
        calc_x += 15;
    } else {
        calc_x = x + 10;
    }
    int val_calc_x = 0;
    if (centered_to_screen) {
        val_calc_x = (SCREEN_WIDTH + w) / 2;
        val_calc_x -= 60;
    } else {
        val_calc_x = x + w - 60;
    }
    
    //Show all items
    for (unsigned int i = 0; i < items.size(); i++) {
        int calc_y = 0;
        if (centered_to_screen) {
            calc_y = (SCREEN_HEIGHT - h) / 2;
            calc_y += 21;
        } else {
            calc_y = y;
        }
        calc_y += 16*i;
        if (calc_y >= text_cutoff_y) continue;
        if (i+scroll >= items.size()) continue;
        
        uint16_t calc_color = 0xFFFF;
        if ((int)i == select_base) calc_color = colorRGB(0.3f,1,0.3f);

        fonts->drawString(items[i+scroll].label.c_str(),calc_color,*screen,calc_x,calc_y);
        fonts->drawString(items[i+scroll].value.c_str(),calc_color,*screen,val_calc_x+items[i+scroll].x_offset,calc_y);
    }

    //Controls
    if (!lock_inputs) {
        //Scroll
        if (isKeyPressed(K_PAD_S) && buttons_wiped)
            {scroll++; buttons_wiped = false;}
        if (isKeyPressed(K_PAD_N) && buttons_wiped)
            {scroll--; buttons_wiped = false;}


        if (allow_scroll_jump) {
            //Find, and jump to the next or last category '['
            if (isKeyPressed(K_PAD_E) && buttons_wiped)
            {
                for (int i = scroll; i < (int)items.size(); i++) {
                    if (i >= (int)items.size()) continue;
                    if (items[i].label.contains('[') && i != (int)scroll) {
                        scroll = i;break;                }
                }
                buttons_wiped = false;
            }
            if (isKeyPressed(K_PAD_W) && buttons_wiped)
            {
                for (int i = scroll; i >= 0; i--) {
                    if (i < 0) continue;
                    if (items[i].label.contains('[') && i != (int)scroll) {
                        scroll = i;break;                }
                }
                buttons_wiped = false;
            }

        }
        if (!isKeyPressed(K_ENTER) && !isKeyPressed(K_PAD_N) && !isKeyPressed(K_PAD_S) && !isKeyPressed(K_PAD_W) && !isKeyPressed(K_PAD_E)) buttons_wiped = true;
    
        //if (scroll < 0) scroll = 0;
        if (scroll < -select_base) scroll = -select_base;
        if (scroll > ((int)items.size()-select_base-1)) scroll = ((int)items.size()-select_base-1);
        

        //On click updates

        if (isKeyPressed(K_ENTER) && (unsigned int)scroll < items.size() && scroll >= 0 && buttons_wiped) {
            if (items[scroll].on_click)
                items[scroll].on_click(&items[scroll]);
            buttons_wiped = false;
        }
    }   
    // for (unsigned int i = 0; i < items.size(); i++) {
    //     if (items[i].on_click)
    //         items[i].on_click();
    // }
}

void Menu::render_frame() {
    //Main window
    if (centered_to_screen) {
        if (show_title_bar) {
            drawTexture(texture_set.tex,
                *screen,
                0, 0, 200, 150,
                (SCREEN_WIDTH  - w) / 2,
                (SCREEN_HEIGHT - h+24) / 2,
                w, h-8);
            //Titlebar
            drawTexture(texture_set.tex,
                *screen,
                0, 0, 200, 3,
                (SCREEN_WIDTH  - w) / 2,
                (SCREEN_HEIGHT - h) / 2,
                w, 16);
        } else {
            drawTexture(texture_set.tex,
                *screen,
                0, 0, 200, 150,
                (SCREEN_WIDTH  - w) / 2,
                (SCREEN_HEIGHT - h) / 2,
                w, h);
        }
    } else {
        if (show_title_bar) {
            drawTexture(texture_set.tex,*screen,0,0,200,150,x,y+16,w,h-16);
            drawTexture(texture_set.tex,*screen,0,0,200,3,x,y,w,16);
            
        } else {
            drawTexture(texture_set.tex,*screen,0,0,200,150,x,y,w,h);
        }

    } 

    if (show_title_bar) {
        if (centered_to_screen) {
            if (titlebar_centered) {
                fonts->drawStringCenter(titlebar.c_str(),0xFFFF,*screen,
            ((SCREEN_WIDTH  - w) / 2)+(w/2),(SCREEN_HEIGHT - h+1) / 2);
            } else {
            fonts->drawString(titlebar.c_str(),0xFFFF,*screen,
            (SCREEN_WIDTH  - w+16) / 2,(SCREEN_HEIGHT - h+1) / 2);}
        } else {
            if (titlebar_centered) {
                fonts->drawStringCenter(titlebar.c_str(),0xFFFF,*screen,
            x+(w/2),y+3);
            } else {
            fonts->drawString(titlebar.c_str(),0xFFFF,*screen,
            x+8,y+3);}
        }
    }
}