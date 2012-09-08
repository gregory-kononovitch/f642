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

    // Zones
    desk->zones = calloc(512, sizeof(zone654));
    desk->nb_max_zones = 512;
    desk->nb_zones = 1;
    desk->root = desk->zones;
    // root
    desk->root->posr.x = 0;
    desk->root->posr.y = 0;
    desk->root->dim.width  = width;
    desk->root->dim.height = height;
    desk->root->num = 0;
    desk->root->any = 1;
    desk->root->zones = desk->zones;
    desk->root->head  = NULL;
    desk->root->items = NULL;
    desk->root->prev  = NULL;
    desk->root->next  = NULL;
    desk->root->level = desk->level0;
    desk->root->down  = NULL;
    desk->root->up    = NULL;
    //
    desk->level0->first = desk->root;
    desk->level0->last  = desk->root;

    //
    return desk;
}


void desk_free654(desk654 **desk) {
    free((*desk)->zones);
    free((*desk)->levels);
    free(*desk);
}


/*
 * Assuming zone is updated
 */
static void desk_compute_iter(desk654 *desk, zone654 *zone) {
    zone = zone->items;
    while(zone) {
        zone->_posa.x = zone->posr.x + zone->head->_posa.x;
        zone->_posa.y = zone->posr.y + zone->head->_posa.y;
        //
        desk_compute_iter(desk, zone);
        //
        zone = zone->next;
    }
}

/*
 * Assuming no other zone above has been changed
 */
void desk_compute_zone(desk654 *desk, zone654 *zone) {
    zone->_posa.x = zone->posr.x;
    zone->_posa.y = zone->posr.y;
    if (zone->head) {
        zone->_posa.x += zone->head->_posa.x;
        zone->_posa.y += zone->head->_posa.y;
    }
    //
    desk_compute_iter(desk, zone);
}

/*
 *
 */
void desk_compute(desk654 *desk) {
    desk_compute_zone(desk, desk->root);
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
    //
    zone->num = desk->nb_zones;
    zone->any = 0;
    zone->zones = desk->zones;
    //
    zone->posr.x = 0;
    zone->posr.y = 0;
    zone->dim.width  = 0;
    zone->dim.height = 0;
    //
    zone->head  = head;
    zone->items = NULL;
    zone->prev  = NULL;
    zone->next  = head->items;
    if (head->items) head->items->prev = zone;
    head->items = zone;
    //
    zone->level = &desk->levels[level];
    zone->down  = zone->level->last;
    if (zone->level->last) zone->level->last->up = zone;
    zone->up    = NULL;
    //
    desk->nb_zones++;

    //
    return zone;
}

/*
 * Without recomputing
 */
void zone_set_location(zone654 *zone, int x, int y) {
    zone->posr.x = x;
    zone->posr.y = y;
}

/*
 * Without recomputing
 */
void zone_set_dimension(zone654 *zone, int w, int h) {
    zone->dim.width  = w;
    zone->dim.height = h;
}
