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


#ifndef BINIO_H
#define BINIO_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

    /*
     * Notes on format of format strings:
     *  - whitespace is ignored
     *  - each "group" consists of an optional count (defaults to 1),
     *    an optional byte-order marker (defaults to H, "host-native"),
     *    and a  data-type specifier.
     *  - when unpacking, each variable argument is a pointer to the
     *    appropriate number of objects of the appropriate type.
     *  - when packing, each variable argument is an object of the
     *    appropriate type if the count is omitted, or a pointer to the
     *    appropriate number of objects of the appropriate type if the
     *    count is specified.
     *  - the buffer supplied to pack/unpack must be of sufficient length
     *    to hold all the data, or the behavior is unspecified.
     *  - the file provided to the "f" versions of the functions must be
     *    open in the appropriate mode, or the behavior is unspecified.
     *  - the file supplied to funpackf must be of sufficient length to
     *    hold all the data, or the behavior is unspecified.
     *  - the behavior of all functions is unspecified if the format string
     *    is incorrectly-formed.
     *
     * Data-type specifiers:
     *  x skipped byte; no corresponding argument
     *  b byte
     *  s two-byte two's-complement integer
     *  i four-byte two's-complement integer
     *  l eight-byte two's-complement integer
     *  f four-byte IEEE754 float
     *  d eight-byte IEEE754 double
     *
     * Byte-order specifiers:
     *  L little-endian
     *  B big-endian
     *  H host's native byte order
     *  N network byte order
     */


    #define BinIO_TYPE_IGNORE_BYTE         'x'
    #define BinIO_TYPE_BYTE                'b'
    #define BinIO_TYPE_INT16               's'
    #define BinIO_TYPE_INT32               'i'
    #define BinIO_TYPE_INT64               'l'
    #define BinIO_TYPE_FLOAT32             'f'
    #define BinIO_TYPE_FLOAT64             'd'

    #define BinIO_LITTLE_ENDIAN_BYTE_ORDER 'L'
    #define BinIO_BIG_ENDIAN_BYTE_ORDER    'B'
    #define BinIO_HOST_BYTE_ORDER          'H'
    #define BinIO_NETWORK_BYTE_ORDER       'N'

    #define BinIO_BYTE_ORDER BinIO_LITTLE_ENDIAN_BYTE_ORDER

    typedef struct
    {
        double   d;
        uint64_t l;
        int      i;
        float    f;
        uint16_t s;
        uint8_t  b;
    } test_data;

    void packf    (                    const char *format, ...);
    void spackf   (void       *buffer, const char *format, ...);
    void fpackf   (FILE       *file,   const char *format, ...);
    void vspackf  (void       *buffer, const char *format, va_list args);
    void vfpackf  (FILE       *file,   const char *format, va_list args);

    void unpackf  (                    const char *format, ...);
    void sunpackf (const void *buffer, const char *format, ...);
    void funpackf (FILE       *file,   const char *format, ...);
    void vsunpackf(const void *buffer, const char *format, va_list args);
    void vfunpackf(FILE       *file,   const char *format, va_list args);

#if defined(__cplusplus)
}
#endif

#endif /* BINIO_H */

