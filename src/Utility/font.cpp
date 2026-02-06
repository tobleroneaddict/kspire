#include "font.h"

int Fonts::init(Bundle* resources) {
    
    //Load font data file
    font_dat = resources->load_raw_data("resources/fonts/font.dat");
    if (font_dat.size() != 273) { printf("Font data size wrong! at %d",font_dat.size()); return 1;}
    //Load texture atlas
    //Eventually add this to angel
    std::vector<unsigned char> image;
    unsigned width, height;

    font_dat.clear();  //make sure it's empty

    std::vector<uint8_t> data = resources->load_raw_data("resources/fonts/font_bmp.png");    

    if (data.size() > 0) {
        unsigned error = lodepng::decode(
            image, width, height, 
            data,   //Load raw PNG data from asset bundle
            LodePNGColorType::LCT_RGBA, //Color mode
            8   //Bit depth
        );
        
        if (!error) {
            atlas_data.resize(width * height);

            //Snatch pixel data and store it
            unsigned int pixel = 0;
            for (unsigned int i = 0; i < width * height; i++) {
                unsigned char r = image[pixel + 0];
                unsigned char g = image[pixel + 1];
                unsigned char b = image[pixel + 2];
        
                atlas_data[i] =
                    ((r & 0b11111000) << 8) |
                    ((g & 0b11111100) << 3) |
                    ((b & 0b11111000) >> 3);
                pixel += 4;
            }

            //Update nGL TEXTURE reference
            atlas.bitmap = atlas_data.data();
            atlas.width  = width;
            atlas.height = height;
        }
        
        
        return error;
    }

    printf("ERROR 32843 FONTS\n");
    return 1;
}

int Fonts::drawChar(char c, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    uint32_t width = font_dat[8];
    uint32_t height = font_dat[12];

    //font_dat[16] is the char at the top left
    const unsigned int pos = c - font_dat[16];
    const unsigned int cols = atlas.width / font_dat[8];
    unsigned int pos_x = pos % cols;
    unsigned int pos_y = pos / cols;
    pos_x *= width;
    pos_y *= height;

    //Each character has its specific width
    width = font_dat[c + 17];

    for(unsigned int x1 = 0; x1 < width; x1++)
        for(unsigned int y1 = 0; y1 < height; y1++)
        {
            if(atlas.bitmap[pos_x + x1 + (pos_y + y1) * atlas.width] == 0xFFFF)
                tex.bitmap[x + x1 + (y + y1) * tex.width] = color;
        }

    return width;
}

void Fonts::drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    unsigned int width = 0;
    const char *ptr = str;
    while(*ptr)
        width += font_dat[17 + *ptr++];

    x -= width / 2;
    drawString(str, color, tex, x, y);
}

void Fonts::drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    const char *ptr = str;
    const unsigned int start_x = x;
    while(*ptr)
    {
        if(*ptr == '\n')
        {
            x = start_x;
            y += fontHeight();
        }
        else if(*ptr == '\t')
        {
            x += 32;
            x -= x % 32;
        }
        else
            x += drawChar(*ptr, color, tex, x, y);

        ++ptr;
    }
}

unsigned int Fonts::fontHeight()
{
    return font_dat[12];
}