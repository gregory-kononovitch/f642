#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "p644_point.h"

using namespace p644;

Point::Point() {
  x = 0;
  y = 0;
}

Point::Point(int x, int y) {
  this->x = x;
  this->y = y;
}

Point::~Point() {
}

int Point::dist(Point *p) {
  return sqrt( (p->x - x)*(p->x - x) + (p->y - y)*(p->y - y) );
}
