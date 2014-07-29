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


#ifndef KUGARU_CONF_H
#define KUGARU_CONF_H


#include <glib.h>

#include "Conf.h"


namespace Kugaru
{
    class Conf
    {
        public:
            static Conf& getInstance()
            {
                static Conf instance;
                return instance;
            }

            void save();

            gint     getGameInt (const gchar *key);
            void     setGameInt (const gchar *key, gint value);
            gboolean getGameBool(const gchar *key);
            void     setGameBool(const gchar *key, gboolean value);

            gint     getDisplayInt (const gchar *key);
            void     setDisplayInt (const gchar *key, gboolean value);
            gboolean getDisplayBool(const gchar *key);
            void     setDisplayBool(const gchar *key, gboolean value);

            gint     getSoundInt (const gchar *key);
            void     setSoundInt (const gchar *key, gint value);
            gboolean getSoundBool(const gchar *key);
            void     setSoundBool(const gchar *key, gboolean value);
            
            gint     getMouseInt (const gchar *key);
            void     setMouseInt (const gchar *key, gint value);
            gboolean getMouseBool(const gchar *key);
            void     setMouseBool(const gchar *key, gboolean value);
        protected:
            GKeyFile *keys;
        private:
            Conf();
            Conf(Conf const&);

            void operator=(Conf const&);
    };
}

#endif /* KUGARU_CONF_H */
