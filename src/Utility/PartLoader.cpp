#include "PartLoader.h"





ModTypes PartLoader::string_to_mod(std::string s) {
    if (s == "MOD_Engine")    return ModTypes::MOD_Engine;
    if (s == "MOD_Command")   return ModTypes::MOD_Command;
    if (s == "MOD_Tank")      return ModTypes::MOD_Tank;
    if (s == "MOD_Decoupler") return ModTypes::MOD_Decoupler;
    if (s == "MOD_Strut")     return ModTypes::MOD_Strut;
    if (s == "MOD_Torque")    return ModTypes::MOD_Torque;
    return MOD_Command;
};

//Get part->json.
//Has two uses: For initial load from Assets, and from save. d is the { data }, writes into part.
void PartLoader::config_part(Part* part, const rapidjson::Value& d) {
    std::string ICON;
    if (d.HasMember("staging_icon") && d["staging_icon"].IsString()) ICON = d["staging_icon"].GetString();


    //WIP
    part->icon = Staging_Icons::CAPSULE;

    //COMMON
    if (d.HasMember("id") && d["id"].IsInt()) part->shared_id = d["id"].GetInt();
    if (d.HasMember("name") && d["name"].IsString()) part->name = d["name"].GetString();
    if (d.HasMember("description") && d["description"].IsString()) part->description = d["description"].GetString();
    if (d.HasMember("category") && d["category"].IsString()) part->category = d["category"].GetString();
    if (d.HasMember("model") && d["model"].IsString()) part->objname = d["model"].GetString();


    if (d.HasMember("mass") && d["mass"].IsNumber()) part->mass = d["mass"].GetFloat();
    if (d.HasMember("drag_min") && d["drag_min"].IsNumber()) part->drag_min = d["drag_min"].GetFloat();
    if (d.HasMember("drag_max") && d["drag_max"].IsNumber()) part->drag_max = d["drag_max"].GetFloat();
    if (d.HasMember("drag_ang") && d["drag_ang"].IsNumber()) part->drag_ang = d["drag_ang"].GetFloat();
    if (d.HasMember("temp_max") && d["temp_max"].IsNumber()) part->temp_max = d["temp_max"].GetFloat();
    if (d.HasMember("buoyancy") && d["buoyancy"].IsNumber()) part->buoyancy = d["buoyancy"].GetFloat();

    
    if (d.HasMember("Nodes") && d["Nodes"].IsArray()) {
        const rapidjson::Value& nodes = d["Nodes"];
        for (rapidjson::SizeType i = 0; i < nodes.Size(); i++) {
            const rapidjson::Value& this_node = nodes[i];
            if (!this_node.IsObject()) continue;
            Node new_node;

            //PLEASE CONDENSE THIS!!!!
            if (this_node.HasMember("position") && this_node["position"].IsArray()) {
                const rapidjson::Value& pos = this_node["position"];
                if (pos[0].IsNumber())
                        new_node.position.x = (pos[0].GetFloat());
                if (pos[1].IsNumber())
                        new_node.position.y = (pos[1].GetFloat());
                if (pos[2].IsNumber())
                        new_node.position.z = (pos[2].GetFloat());
            }
            if (this_node.HasMember("up") && this_node["up"].IsArray()) {
                const rapidjson::Value& pos = this_node["up"];
                if (pos[0].IsNumber())
                        new_node.up.x = (pos[0].GetFloat());
                if (pos[1].IsNumber())
                        new_node.up.y = (pos[1].GetFloat());
                if (pos[2].IsNumber())
                        new_node.up.z = (pos[2].GetFloat());
            }
            if (this_node.HasMember("size") && this_node["size"].IsNumber()) new_node.size = this_node["size"].GetInt();
            if (this_node.HasMember("crossfeed") && this_node["crossfeed"].IsNumber()) new_node.crossfeed = this_node["crossfeed"].GetInt();
            if (this_node.HasMember("rigid_attach") && this_node["rigid_attach"].IsNumber()) new_node.rigid_attach = this_node["rigid_attach"].GetInt();

            part->nodes.emplace_back(new_node);
        }
    }

    if (d.HasMember("Modules") && d["Modules"].IsArray()) {
        const rapidjson::Value& modules = d["Modules"];
        for (rapidjson::SizeType i = 0; i < modules.Size(); i++) {
            const rapidjson::Value& mod_one = modules[i];
            if (!mod_one.IsObject()) continue;
            

            Module new_mod;


            if (mod_one.HasMember("type") && mod_one["type"].IsString()) {
                new_mod.type = mod_one["type"].GetString();
                new_mod.module_type = string_to_mod(new_mod.type);
                printf("Found module of type: %s\n",new_mod.type.c_str());
            }

            if (mod_one.HasMember("thrust") && mod_one["thrust"].IsNumber()) new_mod.thrust = mod_one["thrust"].GetFloat();
            if (mod_one.HasMember("torque_p") && mod_one["torque_p"].IsNumber()) new_mod.torque_p = mod_one["torque_p"].GetFloat();
            if (mod_one.HasMember("torque_y") && mod_one["torque_y"].IsNumber()) new_mod.torque_y = mod_one["torque_y"].GetFloat();
            if (mod_one.HasMember("torque_r") && mod_one["torque_r"].IsNumber()) new_mod.torque_r = mod_one["torque_r"].GetFloat();
            if (mod_one.HasMember("crew_max") && mod_one["crew_max"].IsInt()) new_mod.crew_max = mod_one["crew_max"].GetInt();
            if (mod_one.HasMember("crew_min") && mod_one["crew_min"].IsInt()) new_mod.crew_min = mod_one["crew_min"].GetInt();

            //Resource sinks / generators (use negative)
            if (mod_one.HasMember("Resources") && mod_one["Resources"].IsArray()) {
                const rapidjson::Value& mod_res = mod_one["Resources"];
                for (rapidjson::SizeType j = 0; j < mod_res.Size(); j++) {
                    const rapidjson::Value& res = mod_res[j];
                    if (!res.IsObject()) continue;
                    Resource new_res;

                    if (res.HasMember("name") && res["name"].IsString()) new_res.name = res["name"].GetString();
                    if (res.HasMember("rate") && res["rate"].IsNumber()) new_res.rate = res["rate"].GetFloat();
                    new_mod.resources.emplace_back(new_res);
                }
            }

            part->modules.emplace_back(new_mod);

        }
    }

    if (d.HasMember("Resources") && d["Resources"].IsArray()) {
        const rapidjson::Value& resources = d["Resources"];
        for (rapidjson::SizeType i = 0; i < resources.Size(); i++) {
            const rapidjson::Value& res = resources[i];
            if (!res.IsObject()) continue;
            Resource new_res;
            if (res.HasMember("name") && res["name"].IsString()) new_res.name = res["name"].GetString();
            if (res.HasMember("amount") && res["amount"].IsNumber()) new_res.rate = res["amount"].GetFloat();
            if (res.HasMember("amount_max") && res["amount_max"].IsNumber()) new_res.rate = res["amount_max"].GetFloat();
            
            part->resources.emplace_back(new_res);
        }
    }
}

int PartLoader::load_parts(Bundle* parts) {
    _parts = parts;
    //Get part paths
    if (push_raw(parts) != 0) return 4;

    //Read config
    for (ProtoPart &p : raw_parts) {

        
        //Test for a parts.json in the folder
        auto full_list = _parts->ls(p.path);

        if (!(std::find(full_list.begin(),full_list.end(),"part.json") != full_list.end()))
            continue;
        //Grab parts.json
        std::vector<uint8_t> raw = _parts->load_raw_data((p.path + "/part.json").c_str());
        std::string json(raw.begin(),raw.end());
        Document d;
        d.Parse(json.c_str(),json.size());
        

        //Read static part mode
        config_part(&p.default_data,d); //Can also be used for loading from save.
        
        //Copy values that can't be transferred
        p.objname   = p.default_data.objname;
        p.shared_id = p.default_data.shared_id;

        //And finally load the nGL model
        
        std::string obj_locator_string = p.path +"/" + p.objname;
        auto obj_locator = obj_locator_string.c_str();

        p.group.load_group(parts,obj_locator);
        

        //printf("WARNING::::: LIST ALL OBJECTS IN THE SCENE USING ANGEL SHIT\n");
        //Or just do part for now ig
        //EXPAND THIS TO A SEARCH!!
        p.models.emplace_back(p.group.get_object("part"));

    }



    //Load model

    //Track parts
    printf("WARNIGN:::: STILL NEED TO  TRACK PARTS!!\n");

    for (size_t i = 0; i < raw_parts.size(); ++i) {
        tracked_parts.emplace(raw_parts[i].shared_id, i);
    }

    printf("PartLoader: Added %d parts.\n",tracked_parts.size());

    return 0;
}

//Grab protopart by a shared ID
ProtoPart* PartLoader::get_part_by_id(unsigned int id) {
    auto proto = tracked_parts.find(id);
    if (proto == tracked_parts.end())
        return nullptr;

    return &raw_parts[proto->second];
}

//Returns Shared IDs of parts of a certain category
std::vector<int> PartLoader::get_parts_of_category(std::string category) {
    std::vector<int> res;

    for (ProtoPart &p : raw_parts) {
        if (p.default_data.category == category) res.emplace_back(p.shared_id);
    }
    return res;
}

//Get part data paths
int PartLoader::push_raw(Bundle* parts) {
    std::vector<uint8_t> raw = parts->load_raw_data("parts/database.json");
    std::string json(raw.begin(),raw.end());

    Document d;
    d.Parse(json.c_str(),json.size());

    if (!d.HasMember("Banks") || !d["Banks"].IsArray()) {
        printf("E 13418: Missing part banks!\n");
        return 2;
    }

    const rapidjson::Value& banks = d["Banks"];

    for (rapidjson::SizeType i = 0; i < banks.Size(); i++) {
        const rapidjson::Value& bank = banks[i];

        folders.emplace_back(std::string(bank.GetString()));
    }

    //Find all parts, push them to raw
    for (std::string &s : folders) {
        std::string folder_path = "parts/" + s;
        std::vector<std::string> ls = parts->ls(folder_path);
        //S2 is each part folder
        for (std::string &s2 : ls) {
            //Skip anything  that isnt a part
            auto full_list = _parts->ls(folder_path + "/" + s2);
            if (!(std::find(full_list.begin(),full_list.end(),"part.json") != full_list.end()))
                continue;

            //protoparts
            ProtoPart part;
            
            if (s2.find_first_of('.') != std::string::npos) continue; //Skip stray files
            std::string part_path = folder_path + "/" + s2;
            part.path = part_path;
            raw_parts.emplace_back(std::move(part));

            
        }

    }
    return 0;
}