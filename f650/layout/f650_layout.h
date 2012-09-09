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

#define __USE_XOPEN2K 1
#include <pthread.h>

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
%define     obytes0     16
%define     udword1     20
%define     aoword2     22
%define     udword3     24
%define     obytes4     26
%define     rows        28
 */
struct zone_pties {
    uint32_t            flags;          // 0
    pos650s_            posr;           // 4
    dim650s_            dim;            // 8
    pos650s_            posa;           // 12
    // outter rect
    int32_t             obytes0;        // 16 offset nw
    int16_t             udword1;        // 20
    int16_t             aoword2;        // 22
    int16_t             udword3;        // 24
    int16_t             obytes4;        // 26 next seg
    int16_t             rows;           // 28

    //
    char pad[36];
                                        // 64
};

struct zone_ptr {
    // zone
    int                 key;
    int                 flags;
    zone654             *header;

    // zones
    zone654             *head;          // 16
    zone654             *items;
    zone654             *prev;
    zone654             *next;

    // level
    level654            *level;         // 48
    zone654             *down;
    zone654             *up;

    // pool
    zone654             *pool_prev;
    zone654             *pool_next;      // 72

    // repaint
    zone654             *repaint_prev;
    zone654             *repaint_next;      // 88

    // 96
};

//
struct _zone654_ {
    // pties
    struct zone_pties   pties;          // 0

    // ptr
    struct zone_ptr     links;

};

/*
 * pool or reference?
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
    int                 width;
    int                 height;
    bgra650             bgra;
    int                 col_bepth;          // temp
    color650            background;
    color650            foreground;
    int                 pad1;

    // used/free zones pool
    volatile int        spool_spin;         // pthread_spinlock_t, volatile int
    zone654             *free_first;
    zone654             *used_first;

    // repaint pool
    volatile int        spin_paint;         // pthread_spinlock_t, volatile int
    zone654             *paint_first;

};




/*
 *
 */
// Desk
desk654 *desk_create654(int width, int height);
void desk_free654(desk654 **desk);

//
void desk_layout(desk654 *desk);
void desk_layout_zone(desk654 *desk, zone654 *zone);
zone654 *desk_get_zone(desk654 *desk);
void desk_put_zone(desk654 *desk, zone654 *zone);

// Zone
zone654 *zone_add_item(desk654 *desk, zone654 *head, int level);
void zone_set_location(zone654 * zone, int x, int y);
void zone_set_dimension(zone654 * zone, int w, int h);

// Utils
void zone_dump(zone654 *zone, int tree);
void desk_dump(desk654 *desk);


// ASM
long imgfill1a650(bgra650 *dest, uint32_t color, void *pties);
long imgfill2a650(bgra650 *dest, uint32_t color, void *pties);
long imgfill12a650(bgra650 *dest, uint32_t color, void *pties);
long imgfill123a650(bgra650 *dest, uint32_t color, void *pties);




#endif /* F650_LAYOUT_H_ */
