/*
 * p644_brodger.cpp
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
#include <time.h>
#include <sys/time.h>

#include "p644_brodger.h"


using namespace p644;
using namespace std;

Brodger::Brodger(int width, int height) {
  int o;
  this->width  = width;
  this->height = height;
  this->numOsc = 3;
  this->img    = NULL;

  oscs = (Osc*) malloc(numOsc * sizeof(Osc));
  for(o = 0 ; o < numOsc ; o++) {
    Osc osc = Osc(  o % 3
		  , 1L * width - 1L * width * rand() / RAND_MAX
		  , 1L * height- 1L * height *rand() / RAND_MAX
		  , 50      //20L+ 1L*width * rand() / RAND_MAX
		  , 1
		  , 5
		  , NONE     //4L * rand()/RAND_MAX
    );
    __builtin_memcpy(&oscs[o], &osc, sizeof(Osc));
  }
}

Brodger::Brodger(int width, int height, int numOsc) {
  int o;
  this->width = width;
  this->height = height;
  this->numOsc = numOsc;

  this->img = (uint8_t*) malloc(3 * width * height);

  oscs = (Osc*) malloc(numOsc * sizeof(Osc));
  for(o = 0 ; o < numOsc ; o++) {
    Osc osc = Osc(  o % 3
		  , 1L * width - 1L * width * rand() / RAND_MAX
		  , 1L * height- 1L * height *rand() / RAND_MAX
		    , 50//20L+ 1L*width * rand() / RAND_MAX
		  , 1
		  , 5
		  , SIN     //4L * rand()/RAND_MAX
    );
    __builtin_memcpy(&oscs[o], &osc, sizeof(Osc)); 
  }
}

Brodger::~Brodger() {
  if (this->img) free(this->img);
  free(this->oscs);
}

void Brodger::brodge() {
  struct timeval tv1, tv2;
  int x, y, o, pix[3], mtot[3];
  Point p;
  uint8_t *im = img;

  gettimeofday(&tv1, NULL);
  for(o = 0 ; o < 3 ; o++) mtot[o] = 0;
  for(o = 0 ; o < numOsc ; o++) {
    mtot[oscs[o].col] += oscs[o].mas;
  }
  for(o = 0 ; o < 3 ; o++) {
    if (mtot[o] == 0) mtot[o] = 1;
    pix[o] = 0;
  }

  for(y = 0 ; y < height ; y++) {
    for(x = 0 ; x < width ; x++) {
      p.x = x;      p.y = y;
      for(o = 0 ; o < numOsc ; o++) {
	if (oscs[o].mas > 0)
	  pix[oscs[o].col] += oscs[o].mas * oscs[o].coln1(&p);
      }
      for(o = 0 ; o < 3 ; o++) {
	pix[o] /= mtot[o];
	*(im++) = pix[o];
	pix[o] = 0;
      }
    }
  }
  gettimeofday(&tv2, NULL);
  if (tv2.tv_usec - tv1.tv_usec < 0)
      printf("CPPBrodger done in %ld.%ld\n", tv2.tv_sec - tv1.tv_sec - 1, (1000000 + tv2.tv_usec - tv1.tv_usec)/1000);
  else
      printf("CPPBrodger done in %ld.%ld\n", tv2.tv_sec - tv1.tv_sec, (tv2.tv_usec - tv1.tv_usec)/1000);
}

void Brodger::brodge(uint8_t *im) {
  this->img = im;
  brodge();
  this->img = NULL;
}
