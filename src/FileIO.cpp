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

#include <map>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace Kugaru
{
    std::map<const char *, const char *> FileNameCache;

    const char* ConvertFileName( const char *name )
    {
        if ( FileNameCache.count( name ) )
            return FileNameCache[ name ];

        bool data_found = false;
        bool rel_found = false;
        fs::path datap(std::string(DATADIR) + std::string(name));
        fs::path relp(name);

        if ( fs::exists( datap ) )
            data_found = true;

        if ( fs::exists( relp ) )
            data_found = true;

        if ( !(data_found || rel_found) )
            std::cout << "Warning: File '" << relp.generic_string() << "' Not Found..." << std::endl;

        if ( rel_found )
        {
            FileNameCache[name] = relp.generic_string().c_str();

            return relp.generic_string().c_str();
        }

        if ( data_found )
        {
            FileNameCache[name] = datap.generic_string().c_str();

            return datap.generic_string().c_str();
        }

        return name;
    }
}
