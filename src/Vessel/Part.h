#pragma once
#include "../globals.h"
#include <memory>
#include <vector>


//Staging icons
enum Staging_Icons {
    CAPSULE,
    ENGINE,
    BOOSTER,
    NONE
};

enum Attach_Mode {
    Stack,
    Surface
};

struct Node {
    linalg::vec<float,3> position;
    linalg::vec<float,3> up;
    int size;
    int crossfeed;
    int rigid_attach;

    //Comes from part unique ID, but *10, then index of this node is added.
    //Example: part ID is 5817571
    //As long as you dont 
    //Node 0: 58175710
    //Node 1: 58175712
    //Node 2: 58175713
    //Formula: (p:unique_id * 10) + Node:index

    unsigned int unique_id;
    //Same, but for the node it's attached to.
    unsigned int attached_node;
};


struct Resource {
    std::string name;
    float rate = 0.0f;  //ONLY USE FOR MODULES
    float amount = 0.0f;    //ONLY USE AS TANKS
    float amount_max = 0.0f; //ONLY USE AS TANKS
};

//Part module types
enum ModTypes {
    MOD_Engine,
    MOD_Command,
    MOD_Tank,
    MOD_Decoupler,
    MOD_Strut,
    MOD_Torque
};

//I WAS gonna have multiple MOD_* parts, and inherit from Module here, but, doing so would require some
//Horrifying std::move and conditional deep copying shit that doesnt make sense in a game of this scope
struct Module {
    std::string type;
    ModTypes module_type;
    float thrust = 0.0f;

    float torque_p = 0.0f;
    float torque_y = 0.0f;
    float torque_r = 0.0f;
    int crew_max = 0;
    int crew_min = 0;
    std::vector<Resource> resources; //Input
};


//Avoid using pointers to other parts in here. Instead, use unique ID's, and have a dict or something idfk lol
//also nodes are uID * 10 + node INDEX
//Pretty easy huh
class Part {
    public:
    //Attachment
    bool attached;
    linalg::vec<float,3> pos;
    linalg::vec<float,3> attPos;
    linalg::vec<float,4> attRotation;

    //Nodes
    std::vector<Node> nodes;
    //Modules
    std::vector<Module> modules;
    //Resources
    std::vector<Resource> resources;

    std::vector<int> attach_rules;


    //Comes from assets, identifies what part this is.
    unsigned int shared_id = 0;

    //Which instance of the part is this? pseudorandomly generated per part, stored in save.
    unsigned int unique_id = 0;

    //SYMM
    std::vector<unsigned int> symmetry_tochters_uids;
    int symmetry_mode;
    

    //Family
    std::vector<unsigned int> children;    
    unsigned int parent = -1;

    //Other
    int stage;
    Staging_Icons icon = Staging_Icons::NONE;

    //Physical Properties
    float mass;
    float aerodynamicArea;
    float buoyancy;
    
    enum DragModel
	{
		DEFAULT,
		CONIC,
		CYLINDRICAL
	};

    float drag_max = 0.19f;
    float drag_min = 0.1f;
    float drag_ang = 0.1f;

    //Pascals
    float static_pressure = 100.0f;
    float dynamic_pressure = 0.0f;

    //K
    float temp_max = 2400.0f;
    float temp     = 300.0f;

    //Offsets
    std::vector<float> CoPOffset;
    std::vector<float> CoLOffset;
    std::vector<float> CoBOffset;
};
