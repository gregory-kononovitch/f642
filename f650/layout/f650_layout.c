/*
 * file    : f650_layout.c
 * project : f640
 *
 * Created on: Sep 8, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f650_layout.h"


desk654 *desk_create654(int width, int height) {
    int i;
    //
    desk654 *desk = calloc(1, sizeof(desk654));

    // Levels
    desk->levels = calloc(16, sizeof(level654));
    desk->nb_max_level = 16;
    desk->nb_levels = 16;
    // level0
    desk->level0 = desk->levels;
    desk->level0->num = 0;
    desk->level0->any = 16;
    desk->level0->levels = desk->levels;
    desk->level0->down = NULL;
    desk->level0->up   = NULL;
    desk->level0->first = NULL;
    desk->level0->last  = NULL;
    // levels
    level654 *l = desk->level0;
    for(i = 1 ; i < desk->nb_levels ; i++) {
        desk->levels[i].num = i;
        desk->levels[i].any = 0;
        desk->levels[i].levels = desk->levels;
        desk->levels[i].down = l;
        l->up = &desk->levels[i];
        desk->levels[i].up   = NULL;
        desk->levels[i].first = NULL;
        desk->levels[i].last  = NULL;
        //
        l = &desk->levels[i];
    }
    //
    pthread_spin_init(&desk->spin_spool, 1);
    pthread_mutex_init(&desk->mutex_paint, NULL);

    // Zones
    desk->nb_max_zones = 512;
    desk->zones = calloc(desk->nb_max_zones, sizeof(zone654));
    for(i = 1 ; i < desk->nb_max_zones - 1 ; i++) {
        desk->zones[i].links.pool_prev = &desk->zones[i-1];
        desk->zones[i].links.pool_next = &desk->zones[i+1];
    }
    desk->zones[0].links.pool_next = NULL;
    desk->zones[desk->nb_max_zones - 1].links.pool_next = &desk->zones[desk->nb_max_zones - 2];
    // root
    desk->root = desk->zones;
    desk->nb_zones = 1;
    desk->root->pties.posr.x = 0;
    desk->root->pties.posr.y = 0;
    desk->root->pties.dim.width  = width;
    desk->root->pties.dim.height = height;
    desk->root->pties.hborder.left  = 8;
    desk->root->pties.hborder.right = 8;
    desk->root->pties.vborder.above = 8;
    desk->root->pties.vborder.below = 8;
    desk->root->pties.out.flags = 0;

    //
    desk->root->links.key = 0;
    desk->root->links.flags = 1;
    //
    desk->root->links.head  = NULL;
    desk->root->links.items = NULL;
    desk->root->links.prev  = NULL;
    desk->root->links.next  = NULL;
    desk->root->links.level = desk->level0;
    desk->root->links.down  = NULL;
    desk->root->links.up    = NULL;
    //
    desk->level0->first = desk->root;
    desk->level0->last  = desk->root;

    //
    desk->width  = width;
    desk->height = height;
    bgra_alloc650(&desk->bgra, width, height);
    bgra_fill650(&desk->bgra, BLACK650);
    desk->col_bepth = 4;
    desk->background.value = BLACK650;
    desk->foreground.value = 0xff606060;

    //
    return desk;
}


void desk_free654(desk654 **desk) {
    free((*desk)->zones);
    free((*desk)->levels);
    free(*desk);
}

/*
 * before adding lock
 */
zone654 *desk_get_zone_unsync(desk654 *desk) {
    if (desk->free_first) {
        zone654 *z = desk->free_first;
        if (z->links.pool_next) {
            z->links.pool_next->links.pool_prev = NULL;
            desk->free_first = z;
        } else {
            desk->free_first = NULL;
        }
        //
        if (desk->used_first) {
            desk->used_first->links.pool_prev = z;
            z->links.pool_prev = NULL;
            z->links.pool_next = desk->used_first;
            desk->used_first = z;
        } else {
            z->links.pool_prev = NULL;
            z->links.pool_next = NULL;
            desk->used_first = z;
        }
        desk->nb_used++;
        return z;
    }
    return NULL;
}

void desk_put_zone_unsync(desk654 *desk, zone654 *zone) {
    if (zone->links.pool_prev) {
        zone->links.pool_prev->links.pool_next = zone->links.pool_next;
        if (zone->links.pool_next) {
            zone->links.pool_next->links.pool_prev = zone->links.pool_prev;
        }
    } else if (zone->links.pool_next) {
        zone->links.pool_next->links.pool_prev = zone->links.pool_prev;
    } else {
        desk->used_first = NULL;
    }
    //
    if (desk->free_first) {
        zone->links.pool_prev = NULL;
        zone->links.pool_next = desk->free_first;
        desk->free_first->links.pool_prev = zone;
        desk->free_first;
    } else {
        zone->links.pool_prev = zone->links.pool_next = NULL;
        desk->free_first;
    }
    //
    desk->nb_used--;
}



/*
 * Assuming zone position+dimension / root is set
 * + @@@ everything is >= 0, no pb hors champ
 */
static void zone_compute_heap64(desk654 *desk, zone654 *zone) {
    // _obytes0;       // offset
    zone->pties.out.obytes0 = (zone->pties.posa.x + zone->pties.posa.y * desk->width) * desk->col_bepth;
    // pties.udword1;
    zone->pties.out.udword1 = 4 - (zone->pties.posa.x & 0x03);
    zone->pties.out.udword1 &= 0x03;
    // pties.udword3;
    zone->pties.out.udword3 = (zone->pties.posa.x + zone->pties.dim.width) & 0x03;
    // pties.aoword2;
    zone->pties.out.aoword2 = zone->pties.dim.width - zone->pties.out.udword1 - zone->pties.out.udword3;
    if (zone->pties.out.aoword2 < 40) {
        zone->pties.out.udword1 = zone->pties.dim.width;
        zone->pties.out.udword3 = 0;
        zone->pties.out.aoword2 = 0;
    } else {
        zone->pties.out.aoword2 >>= 2;
    }
    // pties.obytes4;
    zone->pties.out.obytes4 = (desk->width - zone->pties.dim.width) * desk->col_bepth;
    // pties.height
    zone->pties.out.rows = zone->pties.dim.height;

    // ### test : all in udword1 (opt later)
    zone->pties.out.udword1 = zone->pties.dim.width;
    zone->pties.out.aoword2 = 0;
    zone->pties.out.udword3 = 0;

    // ### test : all in udword1 (opt later)
    zone->pties.in.obytes0 = (zone->pties.posa.x + zone->pties.hborder.left + (zone->pties.posa.y + zone->pties.vborder.above) * desk->width) * desk->col_bepth;
    zone->pties.in.udword1 = zone->pties.dim.width - zone->pties.hborder.left - zone->pties.hborder.right;
    zone->pties.in.aoword2 = 0;
    zone->pties.in.udword3 = 0;
    zone->pties.in.obytes4 = (desk->width - zone->pties.dim.width + zone->pties.hborder.left + zone->pties.hborder.right) * desk->col_bepth;
    zone->pties.in.rows    = zone->pties.dim.height - zone->pties.vborder.above - zone->pties.vborder.below;
}

/*
 * Assuming zone is updated
 */
static void desk_layout_iter(desk654 *desk, zone654 *zone) {
    zone = zone->links.items;
    while(zone) {
        zone->pties.posa.x = zone->pties.posr.x + zone->links.head->pties.posa.x;
        zone->pties.posa.y = zone->pties.posr.y + zone->links.head->pties.posa.y;
        zone_compute_heap64(desk, zone);
        //
        desk_layout_iter(desk, zone);
        //
        zone = zone->links.next;
    }
}

/*
 * Assuming no other zone above has been changed
 */
void desk_layout_zone(desk654 *desk, zone654 *zone) {
    zone->pties.posa.x = zone->pties.posr.x;
    zone->pties.posa.y = zone->pties.posr.y;
    if (zone->links.head) {
        zone->pties.posa.x += zone->links.head->pties.posa.x;
        zone->pties.posa.y += zone->links.head->pties.posa.y;
        zone_compute_heap64(desk, zone);
    }
    //
    desk_layout_iter(desk, zone);
}

/*
 *
 */
void desk_layout(desk654 *desk) {
    desk_layout_zone(desk, desk->root);
}

/*
 * geometry of zone needs to be set after
 */
zone654 *zone_add_item(desk654 *desk, zone654 *head, int level) {
    if (!desk || !head || desk->nb_max_zones <= desk->nb_zones || desk->nb_max_level <= level) {
        FOG("Add zone failed, bad parameters");
        return NULL;
    }
    //
    zone654 *zone = &desk->zones[desk->nb_zones];     // @@@
    zone->pties.flags = PLAIN_MASK650;
    //
    zone->links.key = desk->nb_zones;
    zone->links.flags = 0;
    zone->links.header = NULL;      // @@@
    //
    zone->pties.posr.x = 0;
    zone->pties.posr.y = 0;
    zone->pties.dim.width  = 0;
    zone->pties.dim.height = 0;
    //
    zone->links.head  = head;
    zone->links.items = NULL;
    zone->links.prev  = NULL;
    zone->links.next  = head->links.items;
    if (head->links.items) head->links.items->links.prev = zone;
    head->links.items = zone;
    //
    zone->links.level = &desk->levels[level];
    zone->links.down  = zone->links.level->last;
    if (zone->links.level->last) zone->links.level->last->links.up = zone;
    zone->links.up    = NULL;
    //
    desk->nb_zones++;

    //
    return zone;
}

/*
 * Without laying out
 */
void zone_set_location(zone654 *zone, int x, int y) {
    zone->pties.posr.x = x;
    zone->pties.posr.y = y;
}

/*
 * Without laying out
 */
void zone_set_dimension(zone654 *zone, int w, int h) {
    zone->pties.dim.width  = w;
    zone->pties.dim.height = h;
}

/*
 * Without laying out
 */
void zone_set_borders(zone654 *zone, int a, int r, int b, int l) {
    zone->pties.hborder.above = a;
    zone->pties.hborder.below = b;
    zone->pties.vborder.left  = l;
    zone->pties.vborder.right = r;
}

static void zone_dump_iter(zone654 *zone, char *tab) {
    char tabi[33];
    zone = zone->links.items;
    snprintf(tabi, 33, "\t%s", tab);
    while(zone) {
        LOG("%sZone %d : %dx%d at (%d, %d)", tab, zone->links.key, zone->pties.dim.width, zone->pties.dim.height, zone->pties.posa.x, zone->pties.posa.y);
        LOG("%s + ob0 = %d ; ud1 = %d ; ao2 = %d ; ud3 = %d ; ob4 = %d ; h = %d", tab
                , zone->pties.out.obytes0, zone->pties.out.udword1, zone->pties.out.aoword2, zone->pties.out.udword3, zone->pties.out.obytes4, zone->pties.out.rows);
        LOG("%s + ob0 = %d ; ud1 = %d ; ao2 = %d ; ud3 = %d ; ob4 = %d ; h = %d", tab
                , zone->pties.in.obytes0, zone->pties.in.udword1, zone->pties.in.aoword2, zone->pties.in.udword3, zone->pties.in.obytes4, zone->pties.in.rows);
        //
        zone_dump_iter(zone, tabi);
        //
        zone = zone->links.next;
    }
}

void zone_dump(zone654 *zone, int tree) {
    //static char *tab = "\t";
    LOG("Zone %d : %dx%d at (%d, %d)", zone->links.key, zone->pties.dim.width, zone->pties.dim.height, zone->pties.posa.x, zone->pties.posa.y);
    LOG(" - ob0 = %d ; ud1 = %d ; ao2 = %d ; ud3 = %d ; h = %d"
            , zone->pties.out.obytes0, zone->pties.out.udword1, zone->pties.out.aoword2, zone->pties.out.udword3, zone->pties.out.rows);
    LOG(" - ob0 = %d ; ud1 = %d ; ao2 = %d ; ud3 = %d ; h = %d"
            , zone->pties.in.obytes0, zone->pties.in.udword1, zone->pties.in.aoword2, zone->pties.in.udword3, zone->pties.in.rows);
    if (tree) zone_dump_iter(zone, "\t");
}

void desk_dump(desk654 *desk) {
    int i;
    LOG("Desk :");
    zone_dump(desk->root, 1);
}
