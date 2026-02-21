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

//Camera controller for each mode. Also sets mode.
//0: AUTO 1: FREE 2: ORBIT 3: LOCKED 4: VAB
void Camera::camera_controller(Camera::Mode set_mode) {
    this->mode = set_mode;
    Camera::Mode sub_mode = mode;
    if (mode == Camera::AUTO) { //AUTO handler
        if (1 == 1) {
            sub_mode = Camera::FREE;
        } else {
            sub_mode = Camera::ORBIT;
        }
    }

    float rot_speed = 160.0f * dt;
    linalg::vec<float,2> dpad_vector = { 0,0 };
    

    //Diagonal
        if (isKeyPressed(K_PAD_NE)) {
        dpad_vector = { rot_speed, -rot_speed };
    } else if (isKeyPressed(K_PAD_SW)) {
        dpad_vector = { -rot_speed, rot_speed };
    } else if (isKeyPressed(K_PAD_SE)) {
        dpad_vector = { rot_speed, rot_speed };
    } else if (isKeyPressed(K_PAD_NW)) {
        dpad_vector = { -rot_speed, -rot_speed };
        
    }   else { //Cardinal
        #if  defined(_FIREBIRD) && defined(_TINSPIRE)
        if (isKeyPressed(KEY_NSPIRE_W) || isKeyPressed(K_PAD_N)) {
            dpad_vector.y -= rot_speed;
        }
        if (isKeyPressed(KEY_NSPIRE_S) || isKeyPressed(K_PAD_S)) {
            dpad_vector.y += rot_speed;
        }
        if (isKeyPressed(KEY_NSPIRE_A) || isKeyPressed(K_PAD_W)) {
            dpad_vector.x -= rot_speed;
        }
        if (isKeyPressed(KEY_NSPIRE_D) || isKeyPressed(K_PAD_E)) {
            dpad_vector.x += rot_speed;
        }
        #else
        if (isKeyPressed(K_PAD_N)) {
            dpad_vector.y -= rot_speed;
        }
        if (isKeyPressed(K_PAD_S)) {
            dpad_vector.y += rot_speed;
        }
        if (isKeyPressed(K_PAD_W)) {
            dpad_vector.x -= rot_speed;
        }
        if (isKeyPressed(K_PAD_E)) {
            dpad_vector.x += rot_speed;
        }
        #endif
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

    //USE THE ENUM
    //Unused ATM
    switch (sub_mode) {
        case Camera::FREE: //Free

        break;
        case Camera::ORBIT: //Orbit

        break;

        case Camera::LOCKED: //Locked

        break;

        case Camera::VAB: //VAB

        break;



        default:
        printf("Improper mode!\n");
        break;
    }
}