#pragma once
#include "../globals.h"
#include <unordered_map>
#include "../Vessel/Part.h"
#include "../../include/rapidjson/document.h"
#include "../../include/rapidjson/writer.h"
#include "../../include/rapidjson/stringbuffer.h"


using namespace rapidjson;



//Contains the actual textures for a part.
//DO NOT instantiate this into a vessel,
//Ask the partloader for it by id.
struct ProtoPart {
    ModelGroup group;
    std::vector<ngl_object*> models; //Supports multiple objects in an obj, such as flags on capsules, handrails and such
    std::string path;   //Path of part data
    std::string objname;
    unsigned int shared_id;

    std::string editor_name;
    std::string editor_description;
    std::string editor_category;

    //Doesnt use parent, children, etc.
    //Only hold data from the cfg into here
    Part default_data;

};

class PartLoader {
public:

    //Default categories, expandable.
    std::vector<std::string> list_categories = {
        "Command",
        "Control",
        "Storage",
        "Engine",
        "Coupling",
        "Aerodynamics",
        "Power",
        "Utility"
    };

    int load_parts(Bundle* _parts);

    ProtoPart* get_part_by_id(unsigned int id);
    std::vector<int> get_parts_of_category(std::string category);

private:
    
    int push_raw(Bundle* parts);

    void config_part(Part* part, const rapidjson::Value& d);
    ModTypes string_to_mod(std::string s);

    Bundle* _parts;
    std::vector<std::string> folders;
    //All found parts + TEXTURE DATA LIVE HERE
    std::vector<ProtoPart> raw_parts;
    std::unordered_map<int, size_t> tracked_parts;
};
    