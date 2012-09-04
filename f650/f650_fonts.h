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
    const char const            name[16];
    const int                   size;
    const int                   first;
    const int const             *index;
    const unsigned char const   *glyphs;
} font650;


/*
 *
 */
extern font650 monospaced650;


#endif /* FONTS_H_ */
