#pragma once
#include "../globals.h"

class Part {
    public:
    bool attached;
    float aerodynamicArea;
    linalg::vec<float,3> attPos
    linalg::rotation_quat attRotation;

    std::vector<Part> children;    
    
    float mass;
    
    
};
