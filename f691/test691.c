/*
 * file    : test691.c
 * project : f640
 *
 * Created on: Sep 10, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f691.h"


int main() {
    int i, r;

    // Display
    Display *display = XOpenDisplay(":0");
    FOG("Open display : %p", display);

    // Fonts
    int nbf;
    char **fonts = XListFonts(display, "sp", 50, &nbf);
    FOG("List %d fonts like %s :", nbf, "sp");

    // Extensions
    int nbe;
    char **extension = XListExtensions(display, &nbe);
    FOG("List %d extensions :", nbe);
//    for(i = 0 ; i < nbe ; i++) {
//        FOG("\t%s", extension[i]);
//    }

    // Hosts
    int nbh;
    Bool sth;
    XHostAddress *add = XListHosts(display, &nbh, &sth);
    FOG("List %d host address return %d", nbh, sth);
    for(i = 0 ; i < nbh ; i++) {
        FOG("\t%s", add[i].address);
    }

    // Window
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 112, 76, 800, 448, 2, 0xffff00ff, 0xff00ff00);
    FOG("Create window return %lu", window);

    // Window properties
    int nba;
    Atom *atoms = XListProperties(display, window, &nba);
    FOG("List %d properties", nba);
    for(i = 0 ; i < nba ; i++) {
        FOG("\t%s", atoms[i]);
    }

    //
    r = XCloseDisplay(display);
    FOG("Close display return %d", r);
}
