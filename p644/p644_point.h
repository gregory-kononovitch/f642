#ifndef P644_POINT_H
#define P644_POINT_H

#include <stdint.h>

namespace p644
{

class Point
{

 public:
  int x;
  int y;
  Point();
  Point(int x, int y);
  ~Point();
  int dist(Point *p);
};

}

#endif
