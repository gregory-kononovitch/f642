/*
 * file    : xgui691_notify654.c
 * project : f640
 *
 * Created on: Sep 12, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f691.h"


    // Key
static int notify_key_pressed691(desk654 *desk, unsigned long key, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */
static int notify_key_released691(desk654 *desk, unsigned long key, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */

    // Mouse
static int notify_mouse_motion691(desk654 *desk, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */
static int notify_button_pressed691(desk654 *desk, int button, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */
static int notify_button_released691(desk654 *desk, int button, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */
static int notify_scroll691(desk654 *desk, int button, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */

    // Drag
static int notify_drag691(desk654 *desk, int button, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */

    // Enter/Leave
static int notify_enter691(desk654 *desk, int button, int x, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */

    // Focus
static int notify_focus691(desk654 *desk, int y, int mask, uint64_t time) {

    return 0;
}

/*
 *
 */

    // Window
static int notify_window691(desk654 *desk, int x, int y, int width, int height, uint64_t time) {

    return 0;
}

/*
 *
 */
static int notify_visibility691(desk654 *desk, int state, uint64_t time) {

    return 0;
}

/*
 *
 */

    // Expose
static int notify_expose691(desk654 *desk, int x, int y, int width, int height, uint64_t time) {

    return 0;
}

/*
 *
 */
static notify654 xgui691_notify654 = {
        // Key
          .notify_key_pressed = notify_key_pressed691
        , .notify_key_released = notify_key_released691

        // Mouse
        , .notify_mouse_motion = notify_mouse_motion691
        , .notify_button_pressed = notify_button_pressed691
        , .notify_button_released = notify_button_released691
        , .notify_scroll = notify_scroll691

        // Drag
        , .notify_drag = notify_drag691

        // Enter/Leave
        , .notify_enter = notify_enter691

        // Focus
        , .notify_focus = notify_focus691

        // Window
        , .notify_window = notify_window691
        , .notify_visibility = notify_visibility691

        // Expose
        , .notify_expose = notify_expose691
};
