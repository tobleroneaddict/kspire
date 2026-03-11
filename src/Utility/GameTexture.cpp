#include "GameTexture.h"

void GameTexture::draw(int x, int y)
{
    drawTexture(tex, *screen, 0, 0, tex.width, tex.height, x, y, tex.width, tex.height);
}
void GameTexture::draw(int x, int y, int w, int h)
{
    drawTexture(tex, *screen, 0, 0, tex.width, tex.height, x, y, w, h);
}
void GameTexture::draw(int x, int y, int w, int h, int s_w, int s_h)
{
    drawTexture(tex, *screen, 0, 0, s_w, s_h, x, y, w, h);
}
int GameTexture::init(Bundle *resources, std::string path, TEXTURE *_screen)
{
    screen = _screen;

    std::vector<unsigned char> image;
    unsigned width, height;

    tex_data.clear(); // make sure it's empty

    // Set this
    tex.has_transparency = true;

    std::vector<uint8_t> data = resources->load_raw_data(path.c_str());

    if (data.size() > 0)
    {
        unsigned error = lodepng::decode(image, width, height,
                                         data,                       // Load raw PNG data from asset bundle
                                         LodePNGColorType::LCT_RGBA, // Color mode
                                         8                           // Bit depth
        );

        if (!error)
        {
            tex_data.resize(width * height);

            // Snatch pixel data and store it
            unsigned int pixel = 0;
            for (unsigned int i = 0; i < width * height; i++)
            {
                unsigned char r = image[pixel + 0];
                unsigned char g = image[pixel + 1];
                unsigned char b = image[pixel + 2];

                tex_data[i] = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | ((b & 0b11111000) >> 3);
                pixel += 4;
            }

            // Update nGL TEXTURE reference
            tex.bitmap = tex_data.data();
            tex.width = width;
            tex.height = height;
        }

        return error;
    }

    printf("ERROR 32843: GameTexture\n");
    return 1;
}