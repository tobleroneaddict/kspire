#pragma once
#include "../globals.h"

// Vogtinator https://github.com/Vogtinator/crafti/blob/master/font.h
// Adapted for use in this game

struct Fonts
{
    int init(Bundle *resources); // Load font data
    int drawChar(char c, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
    void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
    void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
    unsigned int fontHeight();

    std::vector<uint8_t> font_dat;
    std::vector<uint16_t> atlas_data;
    TEXTURE atlas;
};
