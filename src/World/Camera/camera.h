#pragma once
#include "../../globals.h"

struct Camera {
    float pitch,roll,yaw = 0;
    linalg::vec<float,3> pos;
    linalg::vec<float,3>  wrapper();

    enum Mode {
        AUTO,
        FREE,
        ORBIT,
        LOCKED,
        VAB
    };

    Mode mode;

    float dt;

    void camera_controller(Camera::Mode mode);

    private:
    float clamp_rotation_helper(float in);
};
