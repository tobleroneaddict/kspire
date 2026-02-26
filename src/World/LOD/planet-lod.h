#pragma once
#include "../../globals.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

struct ChunkModel 
{
    unsigned int count_positions = 0;
    std::vector<VECTOR3> *positions = nullptr;   //For nGL
    GLDrawMode draw_mode = GL_QUADS;
    unsigned int count_vertices = 0;
    std::vector<IndexedVertex> *vertices = nullptr;  //For nGL
    std::vector<IndexedVertex> vertices_list{};    //Per-object vertex list
    std::vector<float> u_list{};//:   U V
    std::vector<float> v_list{};//:   MAP
    const TEXTURE *texture; //ptr to texture in my material
};

struct Chunk 
{
    ChunkModel data{}; //model data
    linalg::vec<double,2> coordinate = {0.0,0.0}; //lat,long
    linalg::vec<double,3> center = {0,0,0} ;
    bool visible = true;
    bool dirty = true;
    int lod_level = 8192;
    bool delete_next = false;
};

class Terrain 
{
public:
    std::vector<std::vector<COLOR>> texture_data;
    std::vector<TEXTURE> textures;
    
    //All chunks
    std::vector<Chunk> chunks;
    //How big is a chunk?
    double scale = 1024.0f;

    
    double radius = 600000;

    void calculate_chunk(Chunk* c);

    void recalculate_chunks(linalg::vec<double,2> coordinate,double altitude);

    void render_terrain(linalg::vec<double,2> coordinate,double altitude);

private:

    linalg::vec<double,3> map_to_pos(linalg::vec<double,2> coordinate);
};