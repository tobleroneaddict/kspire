#pragma once
#include "../../globals.h"

struct Camera {
    float pitch,roll,yaw = 0;
    linalg::vec<float,3> pos;
    linalg::vec<float,3>  wrapper();

    float dt;

    void camera_controller(int mode);

    private:
    float clamp_rotation_helper(float in);
};
