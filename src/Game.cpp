/*
 * This file is part of Kugaru.
 *
 * Copyright (C) 2015 Victor A. Santos
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


#include "Game.h"


bool Game::isHoverRect(int sx, int sy, int ex, int ey) {
    bool ishover = true;
    int mousex = mousecoordh / screenwidth * 640;
    int mousey = 480 - (mousecoordv / screenheight * 480);

    if (mousex < sx || mousex > ex)
        ishover = false;
    if (mousey < sy || mousey > ey)
        ishover = false;

    return ishover;
}
