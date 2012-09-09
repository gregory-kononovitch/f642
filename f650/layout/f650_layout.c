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
    desk->root->flags = 1;
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
 * Assuming zone position+dimension / root is set
 * + @@@ everything is >= 0, no pb hors champ
 */
static void zone_compute_heap64(desk654 *desk, zone654 *zone) {
    // _obytes0;       // offset
    zone->_obytes0 = (zone->_posa.x + zone->_posa.y * desk->width) * desk->col_bepth;
    // _udword1;
    zone->_udword1 = 4 - (zone->_posa.x & 0x03);
    zone->_udword1 &= 0x03;
    // _udword3;
    zone->_udword3 = (zone->_posa.x + zone->dim.width) & 0x03;
    // _aoword2;
    zone->_aoword2 = zone->dim.width - zone->_udword1 - zone->_udword3;
    if (zone->_aoword2 < 40) {
        zone->_udword1 = zone->dim.width;
        zone->_udword3 = 0;
        zone->_aoword2 = 0;
    } else {
        zone->_aoword2 >>= 2;
    }
    // _obytes4;
    zone->_obytes4 = (desk->width - zone->dim.width) * desk->col_bepth;
    // _height
    zone->_height = zone->dim.height;

    // ### test : all in udword1 (opt later)
    zone->_udword1 = zone->dim.width;
    zone->_aoword2 = 0;
    zone->_udword3 = 0;
}

/*
 * Assuming zone is updated
 */
static void desk_layout_iter(desk654 *desk, zone654 *zone) {
    zone = zone->items;
    while(zone) {
        zone->_posa.x = zone->posr.x + zone->head->_posa.x;
        zone->_posa.y = zone->posr.y + zone->head->_posa.y;
        zone_compute_heap64(desk, zone);
        //
        desk_layout_iter(desk, zone);
        //
        zone = zone->next;
    }
}

/*
 * Assuming no other zone above has been changed
 */
void desk_layout_zone(desk654 *desk, zone654 *zone) {
    zone->_posa.x = zone->posr.x;
    zone->_posa.y = zone->posr.y;
    if (zone->head) {
        zone->_posa.x += zone->head->_posa.x;
        zone->_posa.y += zone->head->_posa.y;
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
    //
    zone->num = desk->nb_zones;
    zone->flags = 0;
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
 * Without laying out
 */
void zone_set_location(zone654 *zone, int x, int y) {
    zone->posr.x = x;
    zone->posr.y = y;
}

/*
 * Without laying out
 */
void zone_set_dimension(zone654 *zone, int w, int h) {
    zone->dim.width  = w;
    zone->dim.height = h;
}

static void zone_dump_iter(zone654 *zone, char *tab) {
    char tabi[33];
    zone = zone->items;
    snprintf(tabi, 33, "\t%s", tab);
    while(zone) {
        LOG("%sZone %d : %dx%d at (%d, %d)", tab, zone->num, zone->dim.width, zone->dim.height, zone->_posa.x, zone->_posa.y);
        LOG("%s + ob0 = %d ; ud1 = %d ; ao2 = %d ; ud3 = %d ; ob4 = %d ; h = %d", tab, zone->_obytes0, zone->_udword1, zone->_aoword2, zone->_udword3, zone->_obytes4, zone->_height);
        //
        zone_dump_iter(zone, tabi);
        //
        zone = zone->next;
    }
}

void zone_dump(zone654 *zone, int tree) {
    //static char *tab = "\t";
    LOG("Zone %d : %dx%d at (%d, %d)", zone->num, zone->dim.width, zone->dim.height, zone->_posa.x, zone->_posa.y);
    LOG(" - ob0 = %d ; ud1 = %d ; ao2 = %d ; ud3 = %d ; h = %d", zone->_obytes0, zone->_udword1, zone->_aoword2, zone->_udword3, zone->_height);
    if (tree) zone_dump_iter(zone, "\t");
}

void desk_dump(desk654 *desk) {
    int i;
    LOG("Desk :");
    zone_dump(desk->root, 1);
}
