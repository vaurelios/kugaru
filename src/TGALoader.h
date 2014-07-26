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


#ifndef TGA_LOADER_H
#define TGA_LOADER_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define Polygon WinPolygon
#include <windows.h>
#undef Polygon
#include "GL/gl.h"
#else
#include "gamegl.h"
#endif

typedef struct TGAImageRec
{
    GLubyte *data;  // Image Data (Up To 32 Bits)
    GLuint   bpp;   // Image Color Depth In Bits Per Pixel.
    GLuint   sizeX;
    GLuint   sizeY;
} TGAImageRec;

bool upload_image(const char* filePath, bool hasalpha);

/**> FUNCTION PROTOTYPES <**/
TGAImageRec* LoadTGA( char *filename );


#endif

