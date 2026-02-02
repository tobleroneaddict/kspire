#pragma once
#include "globals.h"
#include "bundle.h"

/*
*   ModelGroup contains all the found objects in a given obj file. 
*   Models can be accessed either by name, or by ID. Model IDs always follow the order of appearance in an obj, be cautious when adding more objects in your program of choice.
*   The best way to get an object is by name using get_object(name).
*/
struct ModelGroup {

    /*
    *   Container holding all the objects in this group. Uses a modified ngl_object container, notably with a std::string material field, for use with get_material_by_name(name) to access the object's material.
    */
    std::vector<ngl_object> objects;

    /*
    *   Find an object in the ModelGroup by name. If you name your object "Suzanne" in blender,
    *   You should pass "Suzanne" into this function to retrieve your monkey. Alternatively, access with objects[n]
    *   Usage: ngl_object* obj_player = group.get_object("Tux");
    */
    ngl_object* get_object(const char* name);
    //Print all the objects in the group.
    void debug_list_objects();
    //Frees all data in the group from memory.
    void free_group();
    /*
    *   Loads a .obj file into the ModelGroup.
    *   asset_bundle: pointer to the Bundle
    *   name: Target Wavefront Object File (excluding file extension)
    *   Usage: my_group.load_group(&assets,"niko_oneshot");
    */
    int load_group(Bundle* asset_bundle,const char* name);

    /*
    *   Find an material in the ModelGroup by name.
    *   Usage: anGL_MATERIAL* my_mat = group.get_object("Mdl_0_Mtl_S013_all");
    */
    anGL_MATERIAL* get_material_by_name(std::string name);

    private:
    std::vector<VECTOR3> positions; //All v objects
    std::vector<float> u_list;//:   U V
    std::vector<float> v_list;//:   MAP
    std::vector<anGL_MATERIAL> group_materials; //All materials
    int load_texture_into_material(Bundle* asset_bundle,std::string name,anGL_MATERIAL *mat);

};
