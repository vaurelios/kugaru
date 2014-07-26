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
#include <stdint.h>
#include <assert.h>
#include <sys/time.h>

#include "Utility.h"


/* Foward declarations */
static int QueryPerformanceFrequency (int64_t *liptr);
static int QueryPerformanceCounter   (int64_t *liptr);


class AppTime
{
public:
    AppTime()
    {
        counterRate = 1;
        baseCounter = 0;
        QueryPerformanceFrequency((int64_t *) &counterRate);
        QueryPerformanceCounter((int64_t *) &baseCounter);
    }
    int64_t counterRate;
    int64_t baseCounter;
};
static AppTime g_appTime;


static int QueryPerformanceFrequency(int64_t *liptr)
{
    assert(sizeof (int64_t) == 8);
    *liptr = 1000;

    return 1;
}

static int QueryPerformanceCounter(int64_t *liptr)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    *liptr = ( (((int64_t) tv.tv_sec) * 1000) +
               (((int64_t) tv.tv_usec) / 1000) );

    return 1;
}

AbsoluteTime UpTime()
{
    int64_t counter;
    QueryPerformanceCounter( (int64_t*)&counter);

    counter -= g_appTime.baseCounter;

    AbsoluteTime time;
    time.lo = (unsigned long)counter;
    time.hi = (unsigned long)(counter >> 32);
    
    return time;
}

long AbsoluteDeltaToDuration(AbsoluteTime& a, AbsoluteTime& b)
{
    int64_t value = a.hi;
    int64_t value2 = b.hi;
    value <<= 32;
    value |= a.lo;
    value2 <<= 32;
    value2 |= b.lo;
    value -= value2;

    if (value <= 0)
        return durationImmediate;

    int64_t frac = value % g_appTime.counterRate;
    value /= g_appTime.counterRate;

    long time;

    if (value == 0)
    {
        frac *= -1000000;
        frac /= g_appTime.counterRate;
        time = (long) frac;
    }
    else
    {
        frac *= 1000;
        frac /= g_appTime.counterRate;
        value *= 1000;
        value += frac;
        time = (long) value;
    }

    return time;
}
