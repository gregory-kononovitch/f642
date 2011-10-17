/*
 * p644_osc.cpp
 *
 * Date Oct 16, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "p644_osc.h"


using namespace p644;

Osc::Osc(int col, int x, int y, int pas, int mas, int dec, int decTyp) {
  this->col = col;
  this->atr.x = x;
  this->atr.y = y;
  this->pas = pas;
  this->mas = mas;
  this->dec = dec;
  this->decTyp = decTyp;
}

Osc::~Osc() {
}


int Osc::coln1(Point *p) {
  int d = atr.dist(p);
  
  switch(decTyp) {
  case NONE:
    d = 256 * (d % pas) / pas;
    break;
  case FIXED:
    if (d / pas < dec) d = 256 * (d % pas) / pas;
    else d = 0;
    break;
  case LINEAR:
    if (d / pas < dec) d = 256 * (dec - d/pas) * (d % pas) / (dec * pas);
    else d = 0;
  case EXPO:
    d = exp(-1. * d / (dec * pas)) * 256 * (d % pas) / pas;
    break;
  case SIN:
    d = cos(2 * M_PI * d / (dec * pas)) * 256 * (d % pas) / pas;
  }
  return d ? d : 0;
}

void Osc::coln3(Point *p, int *colors) {
  int d = atr.dist(p);
  d = 256 * (d % pas) / pas;
  colors[col] += 255;
  switch(col) {
  case 0: colors[1] = 255 -d; colors[2] = 255 -d; break;
  case 1: colors[0] = 255 -d; colors[2] = 255 -d; break;
  case 2: colors[0] = 255 -d; colors[1] = 255 -d; break;
  }
}

