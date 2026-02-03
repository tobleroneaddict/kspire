#include "camera.h"

float Camera::clamp_rotation_helper(float in) {
    in = fmod(in, 360.0f);
    if (in < 0) in += 360.0f;
    return in;
}
//Fix under/over rotation
linalg::vec<float,3> Camera::wrapper() {
    return linalg::vec<float,3> {clamp_rotation_helper(-pitch), clamp_rotation_helper(-yaw), clamp_rotation_helper(-roll)};
}

//Camera controller for each mode.
//0: AUTO 1: FREE 2: ORBIT 3: LOCKED 4: VAB
void Camera::camera_controller(int mode) {
    int sub_mode = mode;
    if (mode == 0) { //AUTO handler
        if (1 == 1) {
            sub_mode = 1;
        } else {
            sub_mode = 2;
        }
    }

    float rot_speed = 160.0f * dt;
    linalg::vec<float,2> dpad_vector = { 0,0 };
    if (isKeyPressed(KEY_NSPIRE_W)) {
        dpad_vector.y -= rot_speed;
    }
    if (isKeyPressed(KEY_NSPIRE_S)) {
        dpad_vector.y += rot_speed;
    }
    if (isKeyPressed(KEY_NSPIRE_A)) {
        dpad_vector.x -= rot_speed;
    }
    if (isKeyPressed(KEY_NSPIRE_D)) {
        dpad_vector.x += rot_speed;
    }
    //Make it move at a consistent speed
    float dpad_norm = sqrt((dpad_vector.x * dpad_vector.x) + (dpad_vector.y * dpad_vector.y));
    if (dpad_norm > 0.0001f) {
        dpad_vector /= dpad_norm;
        dpad_vector *= rot_speed;
    }
    
    //Apply
    yaw += dpad_vector.x;
    pitch += dpad_vector.y;
    //clamp pitch
    if (pitch > 89) pitch = 89;
    if (pitch < -89) pitch = -89;
        

    //Unused ATM
    switch (sub_mode) {
        case 1: //Free

        break;
        case 2: //Orbit

        break;

        case 3: //Locked

        break;

        case 4: //VAB

        break;



        default:
        printf("Improper mode!\n");
        break;
    }
}