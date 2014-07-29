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


#include "config.h"

#include "gamegl.h"

#include <stdio.h>
#include <map>
#include <glib.h>
#include <glib/gstdio.h>


namespace Kugaru
{
    std::map<const char *, GString *> FileNameCache;

    const char* ConvertFileName( const char *name )
    {
        gchar **names = g_strsplit(name, "/", -1);
        gchar *rpath = g_build_filenamev(names);

        g_free(names);

        LOGFUNC;

        if ( FileNameCache.count(name) )
            return FileNameCache[name]->str;

        GString *path = NULL;

        gchar *datap = g_build_filename(DATADIR, rpath, NULL);
        if ( g_file_test(datap, G_FILE_TEST_IS_REGULAR) )
        {
            path = g_string_new(datap);
        }
        g_free(datap);
 
        if ( g_file_test(rpath, G_FILE_TEST_IS_REGULAR) )
        {
            if (path != NULL)
                g_string_free(path, true);

            path = g_string_new(rpath);
        }

        char *env_data;
        if ( (env_data = getenv("KUGARU_DATA_DIR")) != NULL )
        {
            gchar *envp;

            LOG("Picking up KUGARU_DATA_DIR...");

            envp = g_build_filename(env_data, rpath, NULL);

            if ( g_file_test(envp, G_FILE_TEST_IS_REGULAR) )
                path = g_string_new(envp);

            g_free(envp);
        }

        if ( path == NULL )
        {
            std::cout << "Warning: File '" << rpath << "' Not Found..." << std::endl;

            path = g_string_new("");
        }

        FileNameCache[name] = path;

        return path->str;
    }

    gchar *GetConfigFilePath()
    {
        gchar *config_path = g_build_filename(g_get_user_config_dir(), "kugaru", NULL);
        gchar *config_file = g_build_filename(g_get_user_config_dir(), "kugaru", "config.ini", NULL);

        if (!g_file_test(config_path, G_FILE_TEST_IS_DIR))
            mkdir(config_path, 0700);
        g_free(config_path);

        if (!g_file_test(config_file, G_FILE_TEST_IS_REGULAR))
        {
            FILE *fp;

            fp = fopen(config_file, "w");
            fclose(fp);
        }

        return config_file;
    }

    FILE *LoadConfigFile()
    {
        gchar *file = GetConfigFilePath();
        FILE *fp;

        fp = fopen(file, "w+");
        
        g_free(file);

        return fp;
    }
}
