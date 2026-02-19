#include "model.h"

namespace angel {

    ngl_object* ModelGroup::get_object(std::string name) {
        rtrim(name);
        ngl_object* found = nullptr;
        for (ngl_object& n : objects) {
            rtrim(n.name);
            if (n.name == name) {
                found = &n;break;
            }
        }
        return found;
    }

    anGL_MATERIAL* ModelGroup::get_material_by_name(std::string name) {
        rtrim(name);
        anGL_MATERIAL* found = nullptr;
        for (anGL_MATERIAL& n : group_materials) {
            rtrim(n.name);
            if (n.name == name) {
                found = &n;break;
            }
        }
        return found;
    }

    void ModelGroup::debug_list_objects() {
        for (ngl_object& n : objects) {
            printf(n.name.c_str()); printf("\n");
        }
    }

    //From auto loader might not be needed, still working on it

    int ModelGroup::load_texture_into_material(Bundle* asset_bundle,std::string name,anGL_MATERIAL *mat,bool from_auto_loader) {
        mat->diffuse_texture.data.clear();  //make sure it's empty

        //Explode trailing chars
        name.erase(std::find_if(name.rbegin(), name.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), name.end());
        if (from_auto_loader) {
            //name = path + name;
        }

        std::vector<uint8_t> data = asset_bundle->load_raw_data(name.c_str());    

        std::vector<unsigned char> image;
        unsigned width, height;

        if (data.size() > 0) {
            unsigned error = lodepng::decode(
                image, width, height, 
                data,   //Load raw PNG data from asset bundle
                LodePNGColorType::LCT_RGBA, //Color mode
                8   //Bit depth
            );
            
            if (!error) {
                mat->diffuse_texture.data.resize(width * height);

                //Snatch pixel data and store it
                unsigned int pixel = 0;
                for (unsigned int i = 0; i < width * height; i++) {
                    unsigned char r = image[pixel + 0];
                    unsigned char g = image[pixel + 1];
                    unsigned char b = image[pixel + 2];
            
                    mat->diffuse_texture.data[i] =
                        ((r & 0b11111000) << 8) |
                        ((g & 0b11111100) << 3) |
                        ((b & 0b11111000) >> 3);
                    pixel += 4;
                }

                //Update nGL TEXTURE reference
                mat->diffuse_texture.texture.bitmap = mat->diffuse_texture.data.data();
                mat->diffuse_texture.texture.width  = width;
                mat->diffuse_texture.texture.height = height;
            }
            

            return error;
        }

        return 1;
    }

    int ModelGroup::swap_texture(Bundle* asset_bundle, std::string model_name,std::string texture_name) {
        ngl_object* obj = get_object(model_name);
 
        if (obj == nullptr) { printf("58581: swap error\n"); return 1;}

        //Gonna have to add uv prescaling to affix new textures that might differ from the originally loaded textures, do later.

        printf("Using texture: %s\n",texture_name.c_str());          
        int error = load_texture_into_material(asset_bundle,texture_name,get_material_by_name(obj->material),false);
        if (error) {printf("Error loading texture %s into material.\n",texture_name.c_str());}
        //printf("Applied to %s\n",group_materials.back().name.c_str());

        return 0;
    }

    void ModelGroup::free_group() {
        //Clear objects thouroughly
        for (ngl_object& n : objects) {
            n.vertices_list.clear();
            n.u_list.clear();   //marked to clear
            n.v_list.clear();   //marked to clear
            n.count_vertices = 0;
            n.count_positions = 0;
            n.material.clear();
        }

        //Clear material/textures
        for (anGL_MATERIAL &t : group_materials) {
            t.diffuse_texture.data.clear();
            t.name.clear();
        }
        //Finish cleaning
        group_materials.clear();
        objects.clear();
        u_list.clear();
        v_list.clear();
        positions.clear();
        
        //Should textures be outside of model?
        //make a texture atlas module
    }

    ModelGroup::~ModelGroup() {
        free_group();
    }

    int ModelGroup::load_group(Bundle* asset_bundle,const char* name) {
        this->free_group(); //Clear data
        //Load object file
        std::string obj_name = std::string(name) + ".obj";
        std::string mat_name;
        std::string usemat_name;
        std::vector<uint8_t> obj = asset_bundle->load_raw_data(obj_name.c_str());

        //Get path, useful later for materials which dont define absolute paths.
        size_t found = obj_name.find_last_of("/");
        std::string substr = obj_name.substr(0,found);
        substr.append("/");
        
        //Assign materials path if were over a layer deep
        if (found != (size_t)-1) {
            path = substr;
        }   //This appears to work beautifully
        //Fixes models at the root

        //Sanity check
        if (obj.size() == 0)
            {printf("Model::load_model(2) : Object size is zero!\nCheck your file name.\n");
                printf("%s doesnt exist!\n",obj_name.c_str());
                return 1;}

        //Parse OBJ

        std::string line;
        unsigned int lineidx = 0;

        //The current object
        std::string current_object_name;
        unsigned int object_index = -1;  //Object index in the vector of ngl_objects


        //temproary color
        COLOR temporary_color = 0x0;


        printf("Parsing model...\n");
    
        


        while (lineidx < obj.size()) {  //Line by line parse the OBJ

            VECTOR3 out;    //Used for vector3 outputs


            //Comments
            if (obj[lineidx] == '#') {
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }

            //Grab Material file here. Materials might not be [objectname].mtl.
            //Mtllib
            if (lineidx + 3 < obj.size() &&
                obj[lineidx] == 'm' && obj[lineidx+1] == 't' && obj[lineidx+2] == 'l' && obj[lineidx+3] == 'l') {
                //sto mtl filename
                lineidx += 7;

                //Parse mtl name (heyyy please do this better, this sucks!!)
                while (lineidx < obj.size() && obj[lineidx] != '\n') {
                    mat_name += obj[lineidx++];
                }


                //Add the path 
                std::string full_path = path + mat_name;
                mat_name = full_path;

                //Go to NL
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }
            //CURRENTLY NOT SUPPPORTED YET
            //Newmtl
            if (lineidx + 3 < obj.size() &&
                obj[lineidx] == 'n' && obj[lineidx+1] == 'e' && obj[lineidx+2] == 'w' && obj[lineidx+3] == 'm') {
                printf("Newmtl in OBJ not supported!\n");

    //            lineidx += 7;

                //Go to NL
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }
            //Use material for the current object
            //usemtl
            if (lineidx + 3 < obj.size() &&
                obj[lineidx] == 'u' && obj[lineidx+1] == 's' && obj[lineidx+2] == 'e' && obj[lineidx+3] == 'm') {
                usemat_name.clear();
                lineidx += 7;
                //Parse mtl name STO to object
                while (lineidx < obj.size() && obj[lineidx] != '\n') {
                    usemat_name += obj[lineidx++];
                }
                objects[object_index].material = usemat_name;

                //Go to NL
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }

            //Create Object
            if ((lineidx + 2 < obj.size()) &&
                (obj[lineidx] == 'o' && obj[lineidx+1] == ' ')
                ) {
                //Parse object name
                lineidx += 2;
                current_object_name.clear();
                while (lineidx < obj.size() && obj[lineidx] != '\n') {
                    current_object_name += obj[lineidx++];

                }
                //Create new object and assign name
                ngl_object new_thing;
                new_thing.name = current_object_name;
                objects.push_back(new_thing); object_index++; //Advance which object were modifying
                printf("Created new object: %s\t",current_object_name.c_str());
                printf("index: %d \n",object_index);
                //Go to NL
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }
            //Vertex
            if ((lineidx + 2 < obj.size()) &&
                (obj[lineidx] == 'v' && obj[lineidx+1] == ' ')) {
                //Parse Vertex
                lineidx += 2;

                std::vector<char> x, y, z;
                float          _x, _y, _z;
                uint8_t component = 0;  //Current x-y-z writing to

                //Byte Iterator
                while (lineidx < obj.size() && obj[lineidx] != '\n') {
                    if (obj[lineidx] == 0x20) {
                        component++; lineidx++; continue;
                    }

                    switch (component) {
                        case 0:
                        x.push_back(obj[lineidx]);
                        break;
                        case 1:
                        y.push_back(obj[lineidx]);
                        break;
                        case 2:
                        z.push_back(obj[lineidx]);
                        break;
                    }

                    lineidx++;
                }

                //Convert ASCII to Float
                _x = std::stof(std::string(x.begin(), x.end()));
                _y = std::stof(std::string(y.begin(), y.end()));
                _z = std::stof(std::string(z.begin(), z.end()));

                //Then to VECTOR3 STO to pos
                out = {_x, _y, _z};
                positions.push_back(out);

                //Go to NL
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }

            //UV Mapping
            if (lineidx +2 < obj.size() &&
                (obj[lineidx] == 'v' && obj[lineidx + 1] == 't' && obj[lineidx + 2] == ' ')) {
                lineidx += 3;
                //Parse UV

                std::vector<char> u, v;
                float          _u, _v;
                uint8_t component = 0;  // vt 0.500 1 [0]

                //Byte Iterator
                while (lineidx < obj.size() && obj[lineidx] != '\n') {
                    if (obj[lineidx] == ' ') {
                        component++; lineidx++; continue;
                    }
                    switch (component) {
                        case 0:
                        u.push_back(obj[lineidx]);
                        break;
                        case 1:
                        v.push_back(obj[lineidx]);
                        break;
                    }
                    lineidx++;
                }

                //Floatify and push uvs to global UV list
                _u = std::stof(std::string(u.begin(), u.end()));
                _v = std::stof(std::string(v.begin(), v.end()));
                _v = 1- _v; //flip v because ????
                u_list.push_back(_u);
                v_list.push_back(_v);

                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                if (lineidx < obj.size()) lineidx++;
                continue;

            }

            //Face
            if (lineidx+2 < obj.size() &&
                (obj[lineidx] == 'f' && obj[lineidx + 1] == ' ')) {
                lineidx += 2;

                std::vector<int> v_indices;
                std::vector<int> vt_indices;
                std::string token;
                int component = 0; // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
                //VN is UNUSED!!!!!

                //Parse v/vt/vn
                while (lineidx < obj.size() && obj[lineidx] != '\n') {
                    char c = obj[lineidx++];

                    if (c == ' ') {
                        if (!token.empty()) {
                            if (component == 0) v_indices.push_back(std::stoi(token));
                            else if (component == 1) vt_indices.push_back(std::stoi(token));
                            token.clear();
                    }
                        component = 0;
                    } else if (c == '/') {
                        if (component == 0) { v_indices.push_back(std::stoi(token)); token.clear(); }
                        else if (component == 1) { vt_indices.push_back(std::stoi(token)); token.clear(); }
                        component++;
                    } else {
                        token += c;
                    }
                }
                if (!token.empty()) {
                    if (component == 0) v_indices.push_back(std::stoi(token));
                    else if (component == 1) vt_indices.push_back(std::stoi(token));
                }


                //Set draw mode
                switch (v_indices.size()) {
                    case 3:
                    objects[object_index].draw_mode = GL_TRIANGLES;
                    break;
                    case 4:
                    objects[object_index].draw_mode = GL_QUADS;
                    break;
                    default:
                        objects[object_index].draw_mode = GL_TRIANGLES;
                        printf("Unsupported draw mode! Make sure your model consists of all triangles or all quads.\n");
                    break;

                }

                //Build indexed vertex
                for (size_t i = 0; i < v_indices.size(); ++i) {
                    IndexedVertex vtx;
                    vtx.index = v_indices[i] - 1;    //OBJ are extremely silly..they start at 1
                    //Pulled from curr obj uvmap (vt) after this loops.
                    vtx.u = vt_indices[i] - 1;  //Store uv index, to be used in UV_AFFIX
                    vtx.v = 0; //Nothing
                    vtx.c = 0xFF;

                    objects[object_index].vertices_list.push_back(vtx);

                }

                //Go to NL
                while (lineidx < obj.size() && obj[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }


            lineidx++;
        }

        //printf("Created %d new objects.\n",object_index+1);



        printf("Model loading complete! Parsing material...\n");
        printf("name:%s\n",mat_name.c_str());
        //Load material
        std::vector<uint8_t> mat = asset_bundle->load_raw_data(mat_name.c_str());
        if (mat.size() == 0)
            printf("Model::load_model(2) : Material size is zero!\n");

        //Material loading code here

        lineidx = 0;
        while (lineidx < mat.size()) {  //Line by line parse the mat
            VECTOR3 out;    //Used for vector3 outputs

            //For some reason mtl couldnt be read the same way as the vector
            //Kx [Handler for Ka Kd Ks Ke]
            if ((lineidx + 2 < mat.size()) &&
            (mat[lineidx] == 'K' && mat[lineidx+1] == 'd')) {
                //Parse K value
                lineidx += 3;

                char buf[32];
                int bufidx = 0;
                float _x = 0.0f, _y = 0.0f, _z = 0.0f;
                uint8_t component = 0;
                while (lineidx < mat.size() && mat[lineidx] != '\n') {
                    uint8_t c = mat[lineidx];
                    //RG
                    if (c == ' ') {
                        if (bufidx > 0) {
                            buf[bufidx] = 0;
                            switch (component) {
                                case 0: _x = atof(buf); break;
                                case 1: _y = atof(buf); break;
                                case 2: _z = atof(buf); break;
                            }
                            bufidx = 0;
                            component++;
                        }
                        lineidx++;
                        continue;
                    }

                    if (bufidx < 31) {
                        buf[bufidx++] = c;
                    }
                    lineidx++;
                }
                //B
                if (bufidx > 0 && component < 3) {
                    buf[bufidx] = 0;
                    switch (component) {
                        case 0: _x = atof(buf); break;
                        case 1: _y = atof(buf); break;
                        case 2: _z = atof(buf); break;
                    }
                }

                out = { _x, _y, _z };


                temporary_color =
                ((int)(_x*255) & 0b11111000) << 8 |
                ((int)(_y*255) & 0b11111100) << 3 |
                ((int)(_z*255) & 0b11111000) >> 3 ;

                //Go to NL
                while (lineidx < mat.size() && mat[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }

            //Newmtl
            if (lineidx + 3 < obj.size() &&
                mat[lineidx] == 'n' && mat[lineidx+1] == 'e' && mat[lineidx+2] == 'w' && mat[lineidx+3] == 'm') {

                usemat_name.clear();
                lineidx += 7;
                

                while (lineidx < mat.size() && mat[lineidx] != '\n') {
                    usemat_name += mat[lineidx++];
                }

                //New mat and tex
                anGL_MATERIAL new_mat; anGL_TEXTURE new_tex;
                new_mat.name = usemat_name;
                new_mat.diffuse_texture = new_tex;
                group_materials.push_back(new_mat);
                printf("Made new mat %s\n",usemat_name.c_str());

                //Go to NL
                while (lineidx < mat.size() && mat[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }

            //map_Kx
            if (lineidx + 3 < mat.size() &&
                mat[lineidx] == 'm' && mat[lineidx+1] == 'a' && mat[lineidx+2] == 'p' && mat[lineidx+3] == '_') {

                //Map diffuse textures
                if (mat[lineidx+5] == 'd') {
                    usemat_name.clear(); //Reuse this name ig
                    lineidx += 7;
                    while (lineidx < mat.size() && mat[lineidx] != '\n') { //Rip name
                        usemat_name += mat[lineidx++];
                    }      
                    printf("Using texture: %s\n",usemat_name.c_str());          
                    
                    //Combine path obtained in the first steps of load_group, to allow for semi relative pathing in mats.
                    std::string full_path = path + usemat_name;
                    int error = load_texture_into_material(asset_bundle,full_path,&group_materials.back(),true);
                    printf("Size of mats is : %d\n",group_materials.size());
                    if (error) {printf("Error loading texture %s into material.\n",usemat_name.c_str());}
                    printf("Applied to %s\n",group_materials.back().name.c_str());
                }



                //Go to NL
                while (lineidx < mat.size() && mat[lineidx] != '\n') lineidx++;
                lineidx++;
                continue;
            }

            lineidx++;
        }


        //Store model data to the object
        for (ngl_object& n : objects) {
            printf("Writing model %s data...\n",n.name.c_str());
            n.positions = positions.data();    //PTR to my positions
            n.count_positions = positions.size();
            n.count_vertices = n.vertices_list.size();
            n.vertices = n.vertices_list.data();

            //Find material and set it as the object's texture
            anGL_MATERIAL* curr_mat = get_material_by_name(n.material);
            if (!curr_mat) {
                printf("ERROR: Material '%s' missing!\n", n.material.c_str());
                continue;
            }
            curr_mat->diffuse_texture.texture.has_transparency = true;
            curr_mat->diffuse_texture.texture.transparent_color = 0;
            n.texture = &curr_mat->diffuse_texture.texture;

            int i = 0;
            //Store UV data to IV (UV_AFFIX)
            uint8_t errors = 0;
            for (IndexedVertex& iv : n.vertices_list) {
                unsigned int ptr = (unsigned int)iv.u;    //U contains ptr to a UV, gets overwritten
                if (ptr < v_list.size()) {

                    //Fetch UVs from global UV list in Model
                    float _u = u_list[ptr];
                    float _v = v_list[ptr];

                    //Map normalized UVs to texture plane (this is super cool)
                    float t_h = get_material_by_name(n.material)->diffuse_texture.texture.height;
                    float t_w = get_material_by_name(n.material)->diffuse_texture.texture.width;
                    iv.u = _u*t_h;
                    iv.v = _v*t_w;
                    
                    iv.c = temporary_color;
                } else {
                    errors++;
                }
                i++;
            }
            if (errors) { printf("UVs out of range! (%d errors)\n",errors);}
        }



        return 0;
    }
}
