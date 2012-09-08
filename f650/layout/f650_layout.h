/*
 * file    : f650_layout.h
 * project : f640
 *
 * Created on: Sep 8, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F650_LAYOUT_H_
#define F650_LAYOUT_H_


#include "../f650.h"

//
typedef struct _litem654_       litem654;
typedef struct _znode654_       znode654;
//
typedef struct _level654_       level654;
typedef struct _zone654_        zone654;

/*
 *
 */
struct _litem654_ {
    level654            *up;
    level654            *down;
};

struct _znode654_ {
    zone654             *prev;
    zone654             *next;
    zone654             *head;
    zone654             *items;
};

/*
 *
 */
struct _level654_ {


    // levels
    int                 num;
    int                 any;
    level654            *levels;
    level654            *down;
    level654            *up;

    // zones
    zone654             *first;
    zone654             *last;
};


struct _zone654_ {
    // rect650i
    pos650i_            posr;
    dim650i_            dim;

    // zones
    int                 num;
    int                 any;
    zone654             *zones;
    zone654             *head;
    zone654             *items;
    zone654             *prev;
    zone654             *next;

    // level
    level654            *level;
    zone654             *down;
    zone654             *up;
    void                *res1;

    // work
    pos650i_             _posa;

};




#endif /* F650_LAYOUT_H_ */
