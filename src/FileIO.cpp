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
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace Kugaru
{
    std::map<const char *, std::string *> FileNameCache;

    const char* ConvertFileName( const char *name )
    {
        LOGFUNC;

        if ( FileNameCache.count( name ) )
            return FileNameCache[ name ]->c_str();

        std::string *path = NULL;

        fs::path datap(std::string(DATADIR) + std::string(name)); 
        if ( fs::exists( datap ) )
        {
            path = new std::string(datap.c_str());
        }

        fs::path relp(name); 
        if ( fs::exists( relp ) )
        {
            path = new std::string(relp.c_str());
        }

        fs::path *envp;
        char *env_data;
        if ( (env_data = getenv("KUGARU_DATA_DIR")) != NULL )
        {
            LOG("Picking up KUGARU_DATA_DIR...");

            envp = new fs::path(std::string(env_data) + "/" + name);

            if ( fs::exists( *envp ) )
                path = new std::string(envp->c_str());
        }

        if ( path == NULL )
            std::cout << "Warning: File '" << relp.generic_string() << "' Not Found..." << std::endl;

        if ( path == NULL )
            path = new std::string();

        FileNameCache[name] = path;

        return path->c_str();
    }
}
