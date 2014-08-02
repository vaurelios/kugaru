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


#include <stdio.h>
#include <string.h>
#include <SDL.h>

#include "Conf.h"
#include "Input.h"
#include "FileIO.h"
#include "Globals.h"


namespace Kugaru
{
    Conf::Conf()
    {
        gchar *file = GetConfigFilePath();
        GError *err = NULL;

        keys = g_key_file_new();

        g_key_file_load_from_file(keys, file, G_KEY_FILE_KEEP_COMMENTS, &err);

        if (err != NULL)
            fprintf(stderr, "Error while loading config file: %d: %s", err->code, err->message);

        g_free(file);

        /* set defualt values */
        #define GK_SET_DEFAULT_INT(g, k, d) if (!g_key_file_has_key(keys, g, k, NULL)) g_key_file_set_integer(keys, g, k, d)
        #define GK_SET_DEFAULT_STR(g, k, d) if (!g_key_file_has_key(keys, g, k, NULL)) g_key_file_set_string(keys, g, k, d)
        #define GK_SET_DEFAULT_BOOL(g, k, d) if (!g_key_file_has_key(keys, g, k, NULL)) g_key_file_set_boolean(keys, g, k, d)
        
        GK_SET_DEFAULT_INT("Game", "game-speed", 1);
        GK_SET_DEFAULT_INT("Game", "difficulty", 1);

        GK_SET_DEFAULT_BOOL("Game", "floating-jump",  false);
        GK_SET_DEFAULT_BOOL("Game", "show-text",      false);
        GK_SET_DEFAULT_BOOL("Game", "show-points",    false);
        GK_SET_DEFAULT_BOOL("Game", "immediate-mode", false);
        GK_SET_DEFAULT_BOOL("Game", "debug",          false);

        GK_SET_DEFAULT_INT("Display", "width",          800);
        GK_SET_DEFAULT_INT("Display", "height",         600);
        GK_SET_DEFAULT_INT("Display", "detail",         1  );
        GK_SET_DEFAULT_INT("Display", "decals",         0  );
        GK_SET_DEFAULT_INT("Display", "blood",          0  );
        GK_SET_DEFAULT_INT("Display", "damage-effects", 0  );
        GK_SET_DEFAULT_INT("Display", "velocity-blur",  0  );

        GK_SET_DEFAULT_BOOL("Display", "auto-slow-motion", true );
        GK_SET_DEFAULT_BOOL("Display", "trilinear",        false);
        GK_SET_DEFAULT_BOOL("Display", "vsync",            true );
        GK_SET_DEFAULT_BOOL("Display", "show-foliage",     true);
        GK_SET_DEFAULT_BOOL("Display", "always-blur",      false);
        GK_SET_DEFAULT_BOOL("Display", "motion-blur",      false);

        GK_SET_DEFAULT_INT("Sound", "volume", 80);

        GK_SET_DEFAULT_BOOL("Sound", "music",   true);
        GK_SET_DEFAULT_BOOL("Sound", "ambient", true);

        GK_SET_DEFAULT_INT("Mouse", "mouse-speed", 5);
        
        GK_SET_DEFAULT_BOOL("Mouse", "mouse-invert", false);
        GK_SET_DEFAULT_BOOL("Mouse", "mouse-jump",   false);

        GK_SET_DEFAULT_STR("Keys", "key-forward", SDL_GetScancodeName(SDL_SCANCODE_W)     );
        GK_SET_DEFAULT_STR("Keys", "key-back",    SDL_GetScancodeName(SDL_SCANCODE_S)     );
        GK_SET_DEFAULT_STR("Keys", "key-left",    SDL_GetScancodeName(SDL_SCANCODE_A)     );
        GK_SET_DEFAULT_STR("Keys", "key-right",   SDL_GetScancodeName(SDL_SCANCODE_D)     );
        GK_SET_DEFAULT_STR("Keys", "key-jump",    SDL_GetScancodeName(SDL_SCANCODE_SPACE ));
        GK_SET_DEFAULT_STR("Keys", "key-crouch",  SDL_GetScancodeName(SDL_SCANCODE_LSHIFT));
        GK_SET_DEFAULT_STR("Keys", "key-draw",    SDL_GetScancodeName(SDL_SCANCODE_E)     );
        GK_SET_DEFAULT_STR("Keys", "key-throw",   SDL_GetScancodeName(SDL_SCANCODE_Q)     );
        GK_SET_DEFAULT_STR("Keys", "key-attack",  GetButtonName(SDL_BUTTON_LEFT)          );
        GK_SET_DEFAULT_STR("Keys", "key-chat",    SDL_GetScancodeName(SDL_SCANCODE_T)     );
    }

    void Conf::dump()
    {
        setGameInt ("game-speed",     oldgamespeed);
        setGameInt ("difficulty",     difficulty);
        setGameInt ("debug",          debugmode);
        setGameBool("floating-jump",  floatjump);
        setGameBool("show-text",      texttoggle);
        setGameBool("show-points",    showpoints);
        setGameBool("immediate-mode", immediate);

        setDisplayInt ("width",            screenwidth);
        setDisplayInt ("height",           screenheight);
        setDisplayInt ("detail",           detail);
        setDisplayInt ("decals",           decals);
        setDisplayInt ("blood",            blooddetail);
        setDisplayInt ("damage-effects",   damageeffects);
        setDisplayInt ("velocity-blur",    velocityblur);
        setDisplayBool("auto-slow-motion", autoslomo);
        setDisplayBool("trilinear",        trilinear);
        setDisplayBool("vsync",            vsync);
        setDisplayBool("show-foliage",     foliage);
        setDisplayBool("always-blur",      alwaysblur);
        setDisplayBool("motion-blur",      ismotionblur);

        setSoundInt ("volume",  volume); 
        setSoundBool("ambient", ambientsound);
        setSoundBool("music",   musictoggle);

        setMouseInt ("mouse-speed",  usermousesensitivity);
        setMouseBool("mouse-invert", invertmouse);
        setMouseBool("mouse-jump",   mousejump); 
    }

    void Conf::save()
    {
        gchar *file = GetConfigFilePath();
        GError *err = NULL;

        g_key_file_save_to_file(keys, file, &err);

        if (err != NULL)
            fprintf(stderr, "Error while saving config file: %d: %s\n", err->code, err->message);

        g_free(file);
    }

    gint Conf::getGameInt(const gchar *key)
    {
        return g_key_file_get_integer(keys, "Game", key, NULL);
    }

    void Conf::setGameInt(const gchar *key, gint value)
    {
        g_key_file_set_integer(keys, "Game", key, value);
    }

    gboolean Conf::getGameBool(const gchar *key)
    {
        return g_key_file_get_boolean(keys, "Game", key, NULL);
    }

    void Conf::setGameBool(const gchar *key, gboolean value)
    {
        g_key_file_set_boolean(keys, "Game", key, value);
    }

    gboolean Conf::getDisplayInt(const gchar *key)
    {
        return g_key_file_get_integer(keys, "Display", key, NULL);
    }

    void Conf::setDisplayInt(const gchar *key, gint value)
    {
        g_key_file_set_integer(keys, "Display", key, value);
    }

    gboolean Conf::getDisplayBool(const gchar *key)
    {
        return g_key_file_get_boolean(keys, "Display", key, NULL);
    }

    void Conf::setDisplayBool(const gchar *key, gboolean value)
    {
        g_key_file_set_boolean(keys, "Display", key, value);
    }

    gint Conf::getSoundInt(const gchar *key)
    {
        return g_key_file_get_integer(keys, "Sound", key, NULL);
    }

    void Conf::setSoundInt(const gchar *key, gint value)
    {
        g_key_file_set_integer(keys, "Sound", key, value);
    }

    gboolean Conf::getSoundBool(const gchar *key)
    {
        return g_key_file_get_boolean(keys, "Sound", key, NULL);
    }

    void Conf::setSoundBool(const gchar *key, gboolean value)
    {
        g_key_file_set_boolean(keys, "Sound", key, value);
    }

    gint Conf::getMouseInt(const gchar *key)
    {
        return g_key_file_get_integer(keys, "Mouse", key, NULL);
    }

    void Conf::setMouseInt(const gchar *key, gint value)
    {
        g_key_file_set_integer(keys, "Mouse", key, value);
    }

    gboolean Conf::getMouseBool(const gchar *key)
    {
        return g_key_file_get_boolean(keys, "Mouse", key, NULL);
    }

    void Conf::setMouseBool(const gchar *key, gboolean value)
    {
        g_key_file_set_boolean(keys, "Mouse", key, value);
    }

    gchar *Conf::getKeysStr(const gchar *key)
    {
        return g_key_file_get_string(keys, "Keys", key, NULL);
    }

    void Conf::setKeysStr(const gchar *key, gchar *value)
    {
        g_key_file_set_string(keys, "Keys", key, value);
    }
}

