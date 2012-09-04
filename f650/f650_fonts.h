/*
 * file    : fonts.h
 * project : f640
 *
 * Created on: Sep 4, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef FONTS_H_
#define FONTS_H_

typedef struct {
    const char const            name[16];       // 0
    const int                   size;           // 16
    const int                   first;          // 20
    const short                 baseline;       // 24
    const short                 width;          // 26
    const short                 height;         // 28
    const short                 res;            // 30
    const int const             *index;         // 32
    const unsigned char const   *glyphs;        // 48
} font650;


/*
 *
 */
extern font650 monospaced650;


#endif /* FONTS_H_ */
