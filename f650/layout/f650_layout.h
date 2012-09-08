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
typedef struct _level654_       level654;
typedef struct _zone654_        zone654;
typedef struct _desk654_        desk654;


/*
 *
 */
struct _level654_ {
    //

    // levels
    int                 num;
    int                 any;
    level654            *levels;
    level654            *down;              // 16
    level654            *up;

    // zones
    zone654             *first;             // 32
    zone654             *last;
    //

    void                *pad1;               // 48
    void                *pad2;
};

/*
 *
 */
struct _zone654_ {
    // rect650i
    pos650i_            posr;
    dim650i_            dim;

    // zones
    int                 num;            // 16
    int                 any;            // (nb_zones for root)
    zone654             *zones;
    zone654             *head;          // 32
    zone654             *items;
    zone654             *prev;          // 48
    zone654             *next;

    // level
    level654            *level;         // 64
    zone654             *down;
    zone654             *up;            // 80

    // work
    pos650i_            _posa;
    int                 _visible;       // 96
    int                 _pad1;
    int                 _obytes0;       // offset
    int                 _ubytes1;
    int                 _abytes2;       // 112
    int                 _ubytes3;
    int                 _obytes4;       // next line
    int                 _res1;
                                        // 128
};

/*
 *
 */
struct _desk654_ {
    //
    level654            *level0;
    zone654             *root;
    //
    level654            *levels;            // 16
    int                 nb_levels;
    int                 nb_max_level;       // (tab of levels?)
    //
    zone654             *zones;             // 32
    int                 nb_zones;
    int                 nb_max_zones;       // (tab of zones?)

    //

};







#endif /* F650_LAYOUT_H_ */
