/*
 * p644_osc.h
 *
 * Date Oct 16, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef P644_OSC_H
#define P644_OSC_H

#include <stdint.h>

#include "p644_point.h"

namespace p644
{
  enum decayType {
      NONE     = 0
    , FIXED    = 1
    , LINEAR   = 2
    , EXPO     = 3
    , SIN      = 4
    , NB_TYPES = 5
  };

class Osc
{
  friend class Brodger;
  int   col;
  Point atr;
  int   pas;
  int   mas;
  int   dec;
  int   decTyp;

 public:
  Osc(int col, int x, int y, int pas, int mas, int dec, int decTyp);
  ~Osc();
  int  coln1(Point *p);
  void coln3(Point *p, int *color);
};

}


#endif
