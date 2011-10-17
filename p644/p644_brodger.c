/*
 * p644_brodger.c
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

typedef struct _point_ {
  int x;
  int y;
} point;

typedef struct _osc_ {
  int   col;
  point atr;
  int   pas;
  int   mas;
  int   dec;
  int   dec_typ;
} osc;

typedef struct _brodge_ {
  int     width;
  int     height;
  int     num_osc;

  uint8_t *img;
  osc     *oscs;
} brodge;


//
int dist(point *p1, point *p2);
void coln3(osc *os, point *p, int *colors);
int  coln1(osc *os, point *p);
void osc_init(osc *os, int col, int x, int y, int pas, int mas, int dec, int dec_typ);
void brodger(brodge *b, uint8_t *img);
void brodger_init(brodge *b, int w, int h, int num_osc);

/*
 *
 */
void brodger_init(brodge *b, int w, int h, int num_osc) {
  int o;
  b->width = w;
  b->height = h;
  b->num_osc = num_osc;

  b->img  = NULL;
  b->oscs = (osc*) malloc(num_osc * sizeof(osc));

  for(o = 0 ; o < num_osc ; o++) {
    osc_init(&b->oscs[o],  o % 3
		  , 1L * b->width - 1L * b->width * rand() / RAND_MAX
		  , 1L * b->height- 1L * b->height *rand() / RAND_MAX
		    , 50//20L+ 1L*width * rand() / RAND_MAX
		  , 1
		  , 5
		  , 0     //4L * rand()/RAND_MAX
    );
  }
}

void brodger_exit(brodge *b) {
  if (b->img) free(b->img);
  free(b->oscs);
}

void brodger(brodge *b, uint8_t *img) {
  int x, y, o, pix[3], mtot[3];
  point p;
  uint8_t *im = img;

  for(o = 0 ; o < 3 ; o++) mtot[o] = 0;
  for(o = 0 ; o < b->num_osc ; o++) {
    mtot[b->oscs[o].col] += b->oscs[o].mas;
  }
  for(o = 0 ; o < 3 ; o++) {
    if (mtot[o] == 0) mtot[o] = 1;
    pix[o] = 0;
  }

  for(y = 0 ; y < b->height ; y++) {
    for(x = 0 ; x < b->width ; x++) {
      p.x = x;      p.y = y;
      for(o = 0 ; o < b->num_osc ; o++) {
	if (b->oscs[o].mas > 0)
	  pix[b->oscs[o].col] += b->oscs[o].mas * coln1(&b->oscs[o], &p);
      }
      for(o = 0 ; o < 3 ; o++) {
	pix[o] /= mtot[o];
	*(im++) = pix[o];
	pix[o] = 0;
      }
    }
  }
}

void init_osc(osc *os, int col, int x, int y, int pas, int mas, int dec, int dec_typ) {
  os->col = col;
  os->atr.x = x;
  os->atr.y = y;
  os->pas = pas;
  os->mas = mas;
  os->dec = dec;
  os->dec_typ = dec_typ;
}


int coln1(osc *os, point *p) {
  int d = dist(&os->atr, p);
  switch(os->dec_typ) {
  case 0:
    d = 256 * (d % os->pas) / os->pas;
    break;
  case 1:
    if (d / os->pas < os->dec) d = 256 * (d % os->pas) / os->pas;
    else d = 0;
    break;
  case 2:
    if (d / os->pas < os->dec) d = 256 * (os->dec - d / os->pas) * (d % os->pas) / (os->dec * os->pas);
    else d = 0;
  case 3:
    d = exp(-1. * d / (os->dec * os->pas)) * 256 * (d % os->pas) / os->pas;
    break;
  case 4:
    d = cos(2 * M_PI * d / (os->dec * os->pas)) * 256 * (d % os->pas) / os->pas;
  }
  return d ? d : 0;
}

void coln3(osc *os, point *p, int *colors) {
  int d = dist(&os->atr, p);
  d = 256 * (d % os->pas) / os->pas;
  colors[os->col] += 255;
  switch(os->col) {
  case 0: colors[1] = 255 -d; colors[2] = 255 -d; break;
  case 1: colors[0] = 255 -d; colors[2] = 255 -d; break;
  case 2: colors[0] = 255 -d; colors[1] = 255 -d; break;
  }
}

int dist(point *p1, point *p2) {
  return sqrt( (p2->x - p1->x)*(p2->x - p1->x) + (p2->y - p1->y)*(p2->y - p1->y) );
}
