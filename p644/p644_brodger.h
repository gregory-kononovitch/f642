/*
 * p644_brodger.h
 *
 * Date Oct 16, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef P644_BRODGER_H
#define P644_BRODGER_H

#include <stdint.h>

#include "p644_point.h"
#include "p644_osc.h"

namespace p644
{

    class Brodger
    {
    public:
        int width;
        int height;
        uint8_t *img;

        int numOsc;
        Osc *oscs;

        Brodger(int width, int height);
        Brodger(int width, int height, int numOsc);
        ~Brodger();
        void brodge();
        void brodge(uint8_t *im);
    };

}

#endif
