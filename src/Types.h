/*
 * This file is part of Kugaru.
 *
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


#ifndef KUGARU_TYPES_H
#define KUGARU_TYPES_H


namespace Kugaru
{
    typedef struct
    {
        short v;
        short h;
    } Point;

    typedef enum
    {
        MAIN_MENU_IN_GAME,
        MAIN_MENU_MAIN,
        MAIN_MENU_RESUME,
        MAIN_MENU_OPTIONS,
        MAIN_MENU_KEYS,
        MAIN_MENU_CAMPAIGNS,
        MAIN_MENU_DELETE_USER,
        MAIN_MENU_USER_LISTING,
        MAIN_MENU_DIFFICULTY,
        MAIN_MENU_LEVEL_LISTING,
        MAIN_MENU_CONGRATS,
        MAIN_MENU_CONGRATS_SCORES,
        MAIN_MENU_12,
        MAIN_MENU_CONGRATS_ENTER_NAME,
        MAIN_MENU_LAST,
        MAIN_MENU_17
    } MainMenu;
}

#endif /* KUGARU_TYPES_H */
