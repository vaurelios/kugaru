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


#ifndef KUGARU_INPUT_H
#define KUGARU_INPUT_H

#include <stdint.h>
#include <SDL.h>


namespace Kugaru
{
    const char   *GetButtonName(uint8_t button);
    SDL_Scancode  GetMouseOrKbd(const char *which);

    bool IsKeyDown(SDL_Scancode key);
    bool IsKeyRangeDown(SDL_Scancode keyf, SDL_Scancode keyl);
    bool IsKeyPress(SDL_Scancode Key);
    bool IsButtonDown(uint8_t btn);
    bool IsButtonPress(uint8_t btn);
}

#endif /* KUGARU_INPUT_H */
