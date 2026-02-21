#pragma once
#include <vector>
#include <string>
#include <algorithm>
#ifdef KSPIRE_PLATFORM_NSPIRE //for kspire PC build, always on for demo makefile
#include <libndls.h>
#endif
#include "zlib.h"
#include "../inc/lodepng/lodepng.h"
#include "../inc/nGL/gl.h"
#include "../inc/nGL/texturetools.h"
#include "../inc/nGL/gldrawarray.h"

namespace angel {

    //Texture data object for ngl.
    struct anGL_TEXTURE {
        TEXTURE texture{};    //In here, bitmap is pointed to data. Don't override how nGL does textures for compatbility.
        std::vector<COLOR> data;
    };

    //Material, for parsing the mtl
    struct anGL_MATERIAL {
        std::string name;
        anGL_TEXTURE diffuse_texture;
    };

    


    //Updated ngl_object. Reused from nGL converter script with some tweaks
    struct ngl_object {
        std::string name;   //name of object
        unsigned int count_positions;
        const VECTOR3 *positions;   //For nGL
        GLDrawMode draw_mode;
        unsigned int count_vertices;
        const IndexedVertex *vertices;  //For nGL
        std::vector<IndexedVertex> vertices_list;    //Per-object vertex list
        std::vector<float> u_list;//:   U V
        std::vector<float> v_list;//:   MAP
        const TEXTURE *texture; //ptr to texture in my material
        std::string material; //Material name from usemtl in the obj file.
    };

}
