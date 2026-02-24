#pragma once
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#ifdef KSPIRE_PLATFORM_NSPIRE
#include <os.h>
#endif

#ifdef KSPIRE_PLATFORM_NSPIRE

#define K_ESC               KEY_NSPIRE_ESC
#define K_PAD_N             KEY_NSPIRE_UP
#define K_PAD_S             KEY_NSPIRE_DOWN
#define K_PAD_E             KEY_NSPIRE_RIGHT
#define K_PAD_W             KEY_NSPIRE_LEFT
#define K_PAD_NW            KEY_NSPIRE_LEFTUP
#define K_PAD_SW            KEY_NSPIRE_DOWNLEFT
#define K_PAD_NE            KEY_NSPIRE_UPRIGHT
#define K_PAD_SE            KEY_NSPIRE_RIGHTDOWN
#define K_WARP_UP           KEY_NSPIRE_Z
#define K_WARP_DOWN         KEY_NSPIRE_X
#define K_ENTER             KEY_NSPIRE_ENTER
#define K_EDITOR_DOWN       KEY_NSPIRE_TAB
#define K_EDITOR_UP         KEY_NSPIRE_SCRATCHPAD
#define K_CTRL              KEY_NSPIRE_CTRL
#define K_SHIFT             KEY_NSPIRE_SHIFT
#define K_EDITOR_IN         K_SHIFT
#define K_EDITOR_OUT        K_CTRL
#define K_DEBUG_SCENE_1     KEY_NSPIRE_1
#define K_DEBUG_SCENE_2     KEY_NSPIRE_2
#define K_MAP               KEY_NSPIRE_M

#endif

//Other System, only PC for now
#ifndef KSPIRE_PLATFORM_NSPIRE

#define K_ESC               SDLK_ESCAPE
#define K_PAD_N             SDLK_w
#define K_PAD_S             SDLK_s
#define K_PAD_E             SDLK_d
#define K_PAD_W             SDLK_a
#define K_PAD_NW            SDLK_h
#define K_PAD_SW            SDLK_h
#define K_PAD_NE            SDLK_h
#define K_PAD_SE            SDLK_h
#define K_ENTER             SDLK_RETURN
#define K_EDITOR_DOWN       SDLK_q
#define K_EDITOR_UP         SDLK_e
#define K_WARP_UP           SDLK_z
#define K_WARP_DOWN         SDLK_x
#define K_EDITOR_IN         SDLK_LSHIFT
#define K_EDITOR_OUT        SDLK_LCTRL
#define K_CTRL              SDLK_LCTRL
#define K_SHIFT             SDLK_LSHIFT
#define K_DEBUG_SCENE_1     SDLK_1
#define K_DEBUG_SCENE_2     SDLK_2
#define K_MAP               SDLK_m

#endif