#pragma once
#include "../include/angel/angel.h"
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <os.h>
#include "../include/linalg.h"

using namespace::angel;

struct DT {
    float dt = 0;
    float dt_last = 0;

	void init() {
		dt_last = SDL_GetTicks() * 0.001f;
	}
    void tick() {
        float dt_now = SDL_GetTicks() * 0.001f;
        dt = dt_now - dt_last;
        dt_last = dt_now;
    }
};