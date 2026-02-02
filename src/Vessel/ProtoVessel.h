#pragma once
#include "../globals.h"

class ProtoVessel {
    public:
    uint persistentId;
    bool vesselSpawning;
    bool landed;
    bool splashed;
    std::string vesselName = "Unnamed Vessel";
    int rootIndex;

    //Staging
    int stage;
    int currentStage;

    double distanceTraveled;

    //In scene, rather volatile
    Transform transform;
    

    float altitude;
    float latitude;
    float longitude;

    linalg::vec<float,3> CoM;

    float gForce;

    float launchTime;

    float heightFromSurface;
    
    float obt_speed;

    //Position and Velocity relative to home body
    linalg::vec<float,3> position;

    linalg::vec<float,3> velocity;

    linalg::rotation_quat rotation;

    private:
};