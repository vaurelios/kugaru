/*
 * This file is part of Kugaru.
 *
 * Copyright (C) 2003, 2010 - Wolfire Games
 * Copyright (C) 2014 Victor A. Santos
 *
 * Kugaru is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kugaru is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kugaru.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <string.h>
#include <SDL.h>

#include "Input.h"
#include "Globals.h"


namespace Kugaru {
    const Uint8 *keystate;

    const char *GetButtonName(uint8_t button)
    {
        switch (button)
        {
            case SDL_BUTTON_LEFT:
                return "button1";
            case SDL_BUTTON_MIDDLE:
                return "button2";
            case SDL_BUTTON_RIGHT:
                return "button3";
            default:
                return "unknown";
        }
    }

    SDL_Scancode GetMouseOrKbd(const char *which)
    {
        if (which == NULL) return SDL_SCANCODE_UNKNOWN;

        if (strncmp(which, "button", 6) == 0)
        {
            if (strcmp(which, "button1") == 0) return (SDL_Scancode) ((int) SDL_NUM_SCANCODES + (int) SDL_BUTTON_LEFT);
            if (strcmp(which, "button2") == 0) return (SDL_Scancode) ((int) SDL_NUM_SCANCODES + (int) SDL_BUTTON_MIDDLE);
            if (strcmp(which, "button3") == 0) return (SDL_Scancode) ((int) SDL_NUM_SCANCODES + (int) SDL_BUTTON_RIGHT);
        }

        return SDL_GetScancodeFromName(which);
    }

    bool IsKeyDown(SDL_Scancode key)
    {
        // is it a mouse button ?
        if ((int) key > SDL_NUM_SCANCODES) return IsButtonDown((int) key - SDL_NUM_SCANCODES);

        keystate = SDL_GetKeyboardState(NULL);
        if (keystate[key]) return true;

        return false;
    }

    bool IsKeyRangeDown(SDL_Scancode keyf, SDL_Scancode keyl)
    {
        if (keysym == NULL) return false;

        if (keysym->scancode >= keyf && keysym->scancode <= keyl) return true;

        return false;
    }

    bool IsKeyPress(SDL_Scancode key)
    {
        if (keysym == NULL) return false;
        if (oldscancode == key) return false;

        if (keysym->scancode == key)
        {
            oldscancode = keysym->scancode;

            return true;
        }

        return false;
    }

    bool IsButtonDown(uint8_t btn)
    {
        if (button == NULL) return false;
        if (button->button == btn) return true;

        return false;
    }

    bool IsButtonPress(uint8_t btn)
    {
        if (button == NULL) return false;
        if (oldbutton == btn) return false;
        if (button->button != btn) return false;

        oldbutton = button->button;

        return true;
    }
}
