/*
 * file    : monospace.c
 * project : f640
 *
 * Created on: Sep 4, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f650_fonts.h"

/*
 * monospaced plain 12
 */

static const int const monospaced_index[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, 0, 2, 14, 22, 50, 77, 100, 126, 132, 147, 162, 175, 190, 198, 205, 211
, 224, 244, 261, 279, 298, 319, 340, 365, 380, 406, 431, 441, 453, 465, 479, 491
, 507, 536, 558, 584, 599, 621, 643, 661, 679, 702, 721, 736, 756, 771, 799, 826
, 848, 868, 892, 915, 933, 950, 971, 990, 1018, 1037, 1052, 1071, 1090, 1103, 1122, 1136
, 1145, 1149, 1170, 1192, 1207, 1229, 1249, 1267, 1292, 1312, 1324, 1341, 1360, 1373, 1396, 1414
, 1432, 1454, 1476, 1489, 1506, 1523, 1541, 1556, 1579, 1594, 1611, 1628, 1644, 1657, 1673, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
, 1681, 1683, 1695, 1715, 1735, 1757, 1779, 1789, 1813, 1817, 1844, 1859, 1871, 1881, 1888, 1905
, 1914, 1920, 1940, 1954, 1966, 1970, 1991, 2022, 2025, 2031, 2041, 2055, 2067, 2093, 2118, 2147
, 2163, 2187, 2211, 2236, 2264, 2288, 2318, 2343, 2362, 2386, 2410, 2435, 2459, 2480, 2501, 2523
, 2544, 2568, 2601, 2625, 2649, 2674, 2702, 2726, 2737, 2766, 2789, 2812, 2836, 2859, 2876, 2896
, 2917, 2940, 2963, 2987, 3014, 3037, 3066, 3086, 3105, 3127, 3149, 3172, 3194, 3207, 3220, 3234
, 3247, 3270, 3294, 3314, 3334, 3355, 3379, 3399, 3415, 3438, 3458, 3478, 3499, 3519, 3538, 3562
};


static const unsigned char const monospaced_glyphs[] = {
 32,0       /*      */
,33,10,0xb3,0xa3,0x93,0x83,0x73,0x63,0x43,0x44,0x33,0x34        /*  !   */
,34,6,0xb2,0xb4,0xa2,0xa4,0x92,0x94     /*  "   */
,35,26,0xb3,0xb5,0xa3,0xa5,0x93,0x95,0x81,0x82,0x83,0x84,0x85,0x86,0x72,0x74,0x62,0x64,0x50,0x51,0x52,0x53,0x54,0x55,0x41,0x43,0x31,0x33        /*  #   */
,36,25,0xc3,0xb2,0xb3,0xb4,0xb5,0xa1,0xa3,0x91,0x93,0x81,0x83,0x72,0x73,0x74,0x63,0x65,0x53,0x55,0x43,0x45,0x31,0x32,0x33,0x34,0x23     /*  $   */
,37,21,0xb1,0xb6,0xa0,0xa2,0xa5,0x90,0x92,0x94,0x81,0x84,0x73,0x62,0x65,0x52,0x54,0x56,0x41,0x44,0x46,0x30,0x35     /*  %   */
,38,24,0xb2,0xb3,0xa1,0xa4,0x91,0x94,0x82,0x83,0x71,0x72,0x76,0x60,0x63,0x66,0x50,0x54,0x55,0x40,0x45,0x31,0x32,0x33,0x34,0x36      /*  &   */
,39,4,0xb3,0xa3,0x93,0x83       /*  '   */
,40,13,0xb4,0xb5,0xa3,0x93,0x82,0x72,0x62,0x52,0x42,0x33,0x23,0x14,0x15     /*  (   */
,41,13,0xb1,0xb2,0xa3,0x93,0x84,0x74,0x64,0x54,0x44,0x33,0x23,0x11,0x12     /*  )   */
,42,11,0xb3,0xa1,0xa3,0xa5,0x92,0x93,0x94,0x82,0x84,0x72,0x74       /*  *   */
,43,13,0x93,0x83,0x73,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x53,0x43,0x33     /*  +   */
,44,6,0x42,0x43,0x32,0x33,0x23,0x12     /*  ,   */
,45,5,0x61,0x62,0x63,0x64,0x65      /*  -   */
,46,4,0x42,0x43,0x32,0x33       /*  .   */
,47,11,0xb6,0xa5,0x95,0x84,0x74,0x63,0x52,0x42,0x31,0x21,0x10       /*  /   */
,48,18,0xb2,0xb3,0xa1,0xa4,0x90,0x95,0x80,0x85,0x70,0x75,0x60,0x65,0x50,0x55,0x41,0x44,0x32,0x33        /*  0   */
,49,15,0xb3,0xa1,0xa2,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x31,0x32,0x33,0x34,0x35       /*  1   */
,50,16,0xb1,0xb2,0xb3,0xb4,0xa5,0x95,0x85,0x74,0x63,0x52,0x41,0x31,0x32,0x33,0x34,0x35      /*  2   */
,51,17,0xb1,0xb2,0xb3,0xb4,0xa5,0x95,0x85,0x72,0x73,0x74,0x65,0x55,0x45,0x31,0x32,0x33,0x34     /*  3   */
,52,19,0xb4,0xa3,0xa4,0x92,0x94,0x82,0x84,0x71,0x74,0x60,0x64,0x50,0x51,0x52,0x53,0x54,0x55,0x44,0x34       /*  4   */
,53,19,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x82,0x83,0x84,0x75,0x65,0x55,0x45,0x31,0x32,0x33,0x34       /*  5   */
,54,23,0xb2,0xb3,0xb4,0xb5,0xa1,0x90,0x80,0x82,0x83,0x84,0x70,0x71,0x75,0x60,0x65,0x50,0x55,0x40,0x45,0x31,0x32,0x33,0x34       /*  6   */
,55,13,0xb1,0xb2,0xb3,0xb4,0xb5,0xa5,0x94,0x84,0x73,0x63,0x53,0x42,0x32     /*  7   */
,56,24,0xb1,0xb2,0xb3,0xb4,0xa0,0xa5,0x90,0x95,0x81,0x84,0x71,0x72,0x73,0x74,0x60,0x65,0x50,0x55,0x40,0x45,0x31,0x32,0x33,0x34      /*  8   */
,57,23,0xb1,0xb2,0xb3,0xb4,0xa0,0xa5,0x90,0x95,0x80,0x85,0x70,0x74,0x75,0x61,0x62,0x63,0x65,0x55,0x44,0x30,0x31,0x32,0x33       /*  9   */
,58,8,0x92,0x93,0x82,0x83,0x42,0x43,0x32,0x33       /*  :   */
,59,10,0x92,0x93,0x82,0x83,0x42,0x43,0x32,0x33,0x23,0x12        /*  ;   */
,60,10,0x95,0x83,0x84,0x72,0x60,0x61,0x52,0x43,0x44,0x35        /*  <   */
,61,12,0x70,0x71,0x72,0x73,0x74,0x75,0x50,0x51,0x52,0x53,0x54,0x55      /*  =   */
,62,10,0x90,0x81,0x82,0x73,0x64,0x65,0x53,0x41,0x42,0x30        /*  >   */
,63,14,0xb1,0xb2,0xb3,0xb4,0xa1,0xa5,0x95,0x84,0x73,0x63,0x43,0x44,0x33,0x34        /*  ?   */
,64,27,0xb2,0xb3,0xb4,0xa1,0xa5,0x90,0x93,0x94,0x95,0x80,0x82,0x85,0x70,0x72,0x75,0x60,0x62,0x64,0x65,0x50,0x53,0x55,0x56,0x41,0x32,0x33,0x34       /*  @   */
,65,20,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36      /*  A   */
,66,24,0xb1,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x72,0x73,0x74,0x61,0x65,0x51,0x55,0x41,0x45,0x31,0x32,0x33,0x34      /*  B   */
,67,13,0xb3,0xb4,0xb5,0xa2,0x91,0x81,0x71,0x61,0x51,0x42,0x33,0x34,0x35     /*  C   */
,68,20,0xb1,0xb2,0xb3,0xa1,0xa4,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x31,0x32,0x33      /*  D   */
,69,20,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31,0x32,0x33,0x34,0x35      /*  E   */
,70,16,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31      /*  F   */
,71,16,0xb3,0xb4,0xb5,0xa2,0x91,0x81,0x71,0x61,0x65,0x51,0x55,0x42,0x45,0x33,0x34,0x35      /*  G   */
,72,21,0xb1,0xb5,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x72,0x73,0x74,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x31,0x35     /*  H   */
,73,17,0xb1,0xb2,0xb3,0xb4,0xb5,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x31,0x32,0x33,0x34,0x35     /*  I   */
,74,13,0xb2,0xb3,0xb4,0xa4,0x94,0x84,0x74,0x64,0x54,0x44,0x31,0x32,0x33     /*  J   */
,75,18,0xb1,0xb6,0xa1,0xa5,0x91,0x94,0x81,0x83,0x71,0x72,0x61,0x63,0x51,0x54,0x41,0x45,0x31,0x36        /*  K   */
,76,13,0xb1,0xa1,0x91,0x81,0x71,0x61,0x51,0x41,0x31,0x32,0x33,0x34,0x35     /*  L   */
,77,26,0xb1,0xb5,0xa1,0xa5,0x91,0x92,0x94,0x95,0x81,0x82,0x84,0x85,0x71,0x72,0x74,0x75,0x61,0x63,0x65,0x51,0x53,0x55,0x41,0x45,0x31,0x35        /*  M   */
,78,25,0xb1,0xb5,0xa1,0xa2,0xa5,0x91,0x92,0x95,0x81,0x83,0x85,0x71,0x73,0x75,0x61,0x63,0x65,0x51,0x54,0x55,0x41,0x44,0x45,0x31,0x35     /*  N   */
,79,20,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34      /*  O   */
,80,18,0xb1,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31        /*  P   */
,81,22,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x24,0x15        /*  Q   */
,82,21,0xb1,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x84,0x71,0x72,0x73,0x61,0x64,0x51,0x54,0x41,0x45,0x31,0x36     /*  R   */
,83,16,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x82,0x73,0x74,0x65,0x55,0x45,0x31,0x32,0x33,0x34      /*  S   */
,84,15,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x33       /*  T   */
,85,19,0xb1,0xb5,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  U   */
,86,17,0xb0,0xb6,0xa0,0xa6,0x91,0x95,0x81,0x85,0x71,0x75,0x62,0x64,0x52,0x54,0x42,0x44,0x33     /*  V   */
,87,26,0xb0,0xb6,0xa0,0xa6,0x90,0x93,0x96,0x80,0x83,0x86,0x70,0x72,0x74,0x76,0x60,0x62,0x64,0x66,0x50,0x52,0x54,0x56,0x41,0x45,0x31,0x35        /*  W   */
,88,17,0xb0,0xb6,0xa1,0xa5,0x92,0x94,0x82,0x84,0x73,0x62,0x64,0x52,0x54,0x41,0x45,0x30,0x36     /*  X   */
,89,13,0xb0,0xb6,0xa1,0xa5,0x91,0x95,0x82,0x84,0x73,0x63,0x53,0x43,0x33     /*  Y   */
,90,17,0xb1,0xb2,0xb3,0xb4,0xb5,0xa5,0x94,0x84,0x73,0x62,0x52,0x41,0x31,0x32,0x33,0x34,0x35     /*  Z   */
,91,17,0xb3,0xb4,0xb5,0xb6,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x33,0x23,0x13,0x14,0x15,0x16     /*  [   */
,92,11,0xb0,0xa1,0x91,0x82,0x72,0x63,0x54,0x44,0x35,0x25,0x16       /*  \   */
,93,17,0xb0,0xb1,0xb2,0xb3,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x33,0x23,0x10,0x11,0x12,0x13     /*  ]   */
,94,12,0xb3,0xa3,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x50,0x56      /*  ^   */
,95,7,0x20,0x21,0x22,0x23,0x24,0x25,0x26        /*  _   */
,96,2,0xc2,0xb3     /*  `   */
,97,19,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36       /*  a   */
,98,20,0xb1,0xa1,0x91,0x93,0x94,0x81,0x82,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x42,0x45,0x31,0x33,0x34      /*  b   */
,99,13,0x92,0x93,0x94,0x95,0x81,0x71,0x61,0x51,0x41,0x32,0x33,0x34,0x35     /*  c   */
,100,20,0xb5,0xa5,0x92,0x93,0x95,0x81,0x84,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35     /*  d   */
,101,18,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x62,0x63,0x64,0x65,0x51,0x41,0x32,0x33,0x34,0x35       /*  e   */
,102,16,0xb4,0xb5,0xb6,0xa3,0x91,0x92,0x93,0x94,0x95,0x96,0x83,0x73,0x63,0x53,0x43,0x33     /*  f   */
,103,23,0x92,0x93,0x95,0x81,0x84,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35,0x25,0x11,0x12,0x13,0x14      /*  g   */
,104,18,0xb1,0xa1,0x91,0x93,0x94,0x81,0x82,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x31,0x35       /*  h   */
,105,10,0xb3,0x91,0x92,0x93,0x83,0x73,0x63,0x53,0x43,0x33       /*  i   */
,106,15,0xb4,0x91,0x92,0x93,0x94,0x84,0x74,0x64,0x54,0x44,0x34,0x24,0x11,0x12,0x13      /*  j   */
,107,17,0xb1,0xa1,0x91,0x95,0x81,0x84,0x71,0x73,0x61,0x62,0x63,0x51,0x54,0x41,0x45,0x31,0x36        /*  k   */
,108,11,0xb1,0xb2,0xb3,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x33      /*  l   */
,109,21,0x91,0x92,0x94,0x81,0x83,0x85,0x71,0x73,0x75,0x61,0x63,0x65,0x51,0x53,0x55,0x41,0x43,0x45,0x31,0x33,0x35        /*  m   */
,110,16,0x91,0x93,0x94,0x81,0x82,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x31,0x35     /*  n   */
,111,16,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34     /*  o   */
,112,20,0x91,0x93,0x94,0x81,0x82,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x42,0x45,0x31,0x33,0x34,0x21,0x11     /*  p   */
,113,20,0x92,0x93,0x95,0x81,0x84,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35,0x25,0x15     /*  q   */
,114,11,0x92,0x94,0x95,0x82,0x83,0x85,0x72,0x62,0x52,0x42,0x32      /*  r   */
,115,15,0x92,0x93,0x94,0x95,0x81,0x71,0x62,0x63,0x64,0x55,0x45,0x31,0x32,0x33,0x34      /*  s   */
,116,15,0xa2,0x90,0x91,0x92,0x93,0x94,0x95,0x82,0x72,0x62,0x52,0x42,0x33,0x34,0x35      /*  t   */
,117,16,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35     /*  u   */
,118,13,0x91,0x95,0x81,0x85,0x72,0x74,0x62,0x64,0x52,0x54,0x42,0x44,0x33        /*  v   */
,119,21,0x90,0x96,0x80,0x83,0x86,0x70,0x72,0x74,0x76,0x60,0x62,0x64,0x66,0x50,0x52,0x54,0x56,0x41,0x45,0x31,0x35        /*  w   */
,120,13,0x91,0x95,0x82,0x84,0x72,0x74,0x63,0x52,0x54,0x42,0x44,0x31,0x35        /*  x   */
,121,15,0x90,0x96,0x81,0x85,0x71,0x75,0x62,0x64,0x52,0x54,0x43,0x33,0x22,0x10,0x11      /*  y   */
,122,15,0x91,0x92,0x93,0x94,0x95,0x85,0x74,0x63,0x52,0x41,0x31,0x32,0x33,0x34,0x35      /*  z   */
,123,14,0xb4,0xb5,0xa3,0x93,0x83,0x73,0x61,0x62,0x53,0x43,0x33,0x23,0x14,0x15       /*  {   */
,124,11,0xb3,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x33,0x23,0x13      /*  |   */
,125,14,0xb1,0xb2,0xa3,0x93,0x83,0x73,0x64,0x65,0x53,0x43,0x33,0x23,0x11,0x12       /*  }   */
,126,6,0x61,0x62,0x65,0x50,0x53,0x54        /*  ~   */
,160,0      /*      */
,161,10,0x92,0x93,0x82,0x83,0x63,0x53,0x43,0x33,0x23,0x13       /*  ¡   */
,162,18,0xb4,0xa3,0xa4,0xa5,0x92,0x94,0x81,0x84,0x71,0x74,0x61,0x64,0x52,0x54,0x43,0x44,0x45,0x34       /*  ¢   */
,163,18,0xb3,0xb4,0xb5,0xa2,0x92,0x82,0x71,0x72,0x73,0x74,0x62,0x52,0x42,0x31,0x32,0x33,0x34,0x35       /*  £   */
,164,20,0xa0,0xa6,0x91,0x92,0x93,0x94,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46     /*  ¤   */
,165,20,0xb0,0xb6,0xa1,0xa5,0x92,0x94,0x83,0x71,0x72,0x73,0x74,0x75,0x63,0x51,0x52,0x53,0x54,0x55,0x43,0x33     /*  ¥   */
,166,8,0xb3,0xa3,0x93,0x83,0x43,0x33,0x23,0x13      /*  ¦   */
,167,22,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x82,0x83,0x71,0x74,0x61,0x65,0x52,0x53,0x55,0x44,0x35,0x25,0x11,0x12,0x13,0x14       /*  §   */
,168,2,0xb2,0xb4        /*  ¨   */
,169,25,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x93,0x95,0x81,0x83,0x85,0x71,0x73,0x75,0x61,0x63,0x65,0x51,0x53,0x55,0x41,0x45,0x32,0x33,0x34        /*  ©   */
,170,13,0xb1,0xb2,0xb3,0xa4,0x92,0x93,0x94,0x81,0x84,0x72,0x73,0x74,0x75        /*  ª   */
,171,10,0x83,0x86,0x72,0x75,0x61,0x64,0x52,0x55,0x43,0x46       /*  «   */
,172,8,0x70,0x71,0x72,0x73,0x74,0x75,0x65,0x55      /*  ¬   */
,173,5,0x61,0x62,0x63,0x64,0x65     /*  ­   */
,174,15,0xb2,0xb3,0xb4,0xa1,0xa3,0xa5,0x91,0x93,0x95,0x81,0x83,0x85,0x72,0x73,0x74      /*  ®   */
,175,7,0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6       /*  ¯   */
,176,4,0xb3,0xa2,0xa4,0x93      /*  °   */
,177,18,0x93,0x83,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x63,0x53,0x30,0x31,0x32,0x33,0x34,0x35,0x36       /*  ±   */
,178,12,0xb1,0xb2,0xb3,0xa4,0x93,0x94,0x81,0x82,0x71,0x72,0x73,0x74     /*  ²   */
,179,10,0xb1,0xb2,0xb3,0xa4,0x92,0x93,0x84,0x71,0x72,0x73       /*  ³   */
,180,2,0xc3,0xb2        /*  ´   */
,181,19,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x31,0x32,0x33,0x35,0x21,0x11      /*  µ   */
,182,29,0xb2,0xb3,0xb4,0xb5,0xa1,0xa2,0xa3,0xa5,0x91,0x92,0x93,0x95,0x82,0x83,0x85,0x73,0x75,0x63,0x65,0x53,0x55,0x43,0x45,0x33,0x35,0x23,0x25,0x13,0x15        /*  ¶   */
,183,1,0x53     /*  ·   */
,184,4,0x23,0x14,0x03,0x04      /*  ¸   */
,185,8,0xb2,0xb3,0xa3,0x93,0x83,0x72,0x73,0x74      /*  ¹   */
,186,12,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x72,0x73,0x74     /*  º   */
,187,10,0x80,0x83,0x71,0x74,0x62,0x65,0x51,0x54,0x40,0x43       /*  »   */
,188,24,0xb0,0xb1,0xb5,0xa1,0xa4,0x91,0x94,0x81,0x83,0x71,0x73,0x62,0x64,0x65,0x52,0x53,0x55,0x41,0x43,0x44,0x45,0x46,0x31,0x35     /*  ¼   */
,189,23,0xb0,0xb1,0xb5,0xa1,0xa4,0x91,0x94,0x81,0x83,0x71,0x73,0x62,0x64,0x65,0x66,0x52,0x56,0x41,0x45,0x31,0x34,0x35,0x36      /*  ½   */
,190,27,0xb0,0xb1,0xb2,0xb5,0xa2,0xa4,0x90,0x91,0x94,0x82,0x83,0x70,0x71,0x73,0x62,0x64,0x65,0x52,0x53,0x55,0x41,0x43,0x44,0x45,0x46,0x31,0x35      /*  ¾   */
,191,14,0x92,0x93,0x82,0x83,0x63,0x53,0x42,0x31,0x21,0x25,0x12,0x13,0x14,0x15       /*  ¿   */
,192,22,0xe3,0xd4,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36       /*  À   */
,193,22,0xe4,0xd3,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36       /*  Á   */
,194,23,0xe3,0xd2,0xd4,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36      /*  Â   */
,195,26,0xe2,0xe3,0xe5,0xd2,0xd4,0xd5,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36       /*  Ã   */
,196,22,0xd2,0xd4,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36       /*  Ä   */
,197,28,0xe2,0xe3,0xe4,0xd2,0xd4,0xc2,0xc3,0xc4,0xb3,0xa2,0xa4,0x92,0x94,0x82,0x84,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x55,0x40,0x46,0x30,0x36     /*  Å   */
,198,23,0xb3,0xb4,0xb5,0xa2,0xa3,0x92,0x93,0x82,0x83,0x71,0x73,0x74,0x61,0x63,0x51,0x52,0x53,0x40,0x43,0x30,0x33,0x34,0x35      /*  Æ   */
,199,17,0xb3,0xb4,0xb5,0xa2,0x91,0x81,0x71,0x61,0x51,0x42,0x33,0x34,0x35,0x24,0x15,0x04,0x05        /*  Ç   */
,200,22,0xe3,0xd4,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31,0x32,0x33,0x34,0x35       /*  È   */
,201,22,0xe4,0xd3,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31,0x32,0x33,0x34,0x35       /*  É   */
,202,23,0xe3,0xd2,0xd4,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31,0x32,0x33,0x34,0x35      /*  Ê   */
,203,22,0xd2,0xd4,0xb1,0xb2,0xb3,0xb4,0xb5,0xa1,0x91,0x81,0x71,0x72,0x73,0x74,0x61,0x51,0x41,0x31,0x32,0x33,0x34,0x35       /*  Ë   */
,204,19,0xe3,0xd4,0xb1,0xb2,0xb3,0xb4,0xb5,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x31,0x32,0x33,0x34,0x35      /*  Ì   */
,205,19,0xe4,0xd3,0xb1,0xb2,0xb3,0xb4,0xb5,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x31,0x32,0x33,0x34,0x35      /*  Í   */
,206,20,0xe3,0xd2,0xd4,0xb1,0xb2,0xb3,0xb4,0xb5,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x31,0x32,0x33,0x34,0x35     /*  Î   */
,207,19,0xd2,0xd4,0xb1,0xb2,0xb3,0xb4,0xb5,0xa3,0x93,0x83,0x73,0x63,0x53,0x43,0x31,0x32,0x33,0x34,0x35      /*  Ï   */
,208,22,0xb1,0xb2,0xb3,0xa1,0xa4,0x91,0x95,0x81,0x85,0x70,0x71,0x72,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x31,0x32,0x33       /*  Ð   */
,209,31,0xe2,0xe3,0xe5,0xd2,0xd4,0xd5,0xb1,0xb5,0xa1,0xa2,0xa5,0x91,0x92,0x95,0x81,0x83,0x85,0x71,0x73,0x75,0x61,0x63,0x65,0x51,0x54,0x55,0x41,0x44,0x45,0x31,0x35      /*  Ñ   */
,210,22,0xe3,0xd4,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  Ò   */
,211,22,0xe4,0xd3,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  Ó   */
,212,23,0xe3,0xd2,0xd4,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34      /*  Ô   */
,213,26,0xe2,0xe3,0xe5,0xd2,0xd4,0xd5,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  Õ   */
,214,22,0xd2,0xd4,0xb2,0xb3,0xb4,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  Ö   */
,215,9,0x81,0x85,0x72,0x74,0x63,0x52,0x54,0x41,0x45     /*  ×   */
,216,27,0xb2,0xb3,0xb4,0xb5,0xa1,0xa5,0x91,0x94,0x95,0x81,0x84,0x85,0x71,0x73,0x75,0x61,0x62,0x65,0x51,0x52,0x55,0x41,0x45,0x31,0x32,0x33,0x34      /*  Ø   */
,217,21,0xe3,0xd4,0xb1,0xb5,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34        /*  Ù   */
,218,21,0xe4,0xd3,0xb1,0xb5,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34        /*  Ú   */
,219,22,0xe3,0xd2,0xd4,0xb1,0xb5,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  Û   */
,220,21,0xd2,0xd4,0xb1,0xb5,0xa1,0xa5,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34        /*  Ü   */
,221,15,0xe4,0xd3,0xb0,0xb6,0xa1,0xa5,0x91,0x95,0x82,0x84,0x73,0x63,0x53,0x43,0x33      /*  Ý   */
,222,18,0xb1,0xa1,0x91,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x52,0x53,0x54,0x41,0x31       /*  Þ   */
,223,19,0xb2,0xb3,0xa1,0xa4,0x91,0x94,0x81,0x83,0x71,0x73,0x61,0x64,0x51,0x55,0x41,0x45,0x31,0x33,0x34      /*  ß   */
,224,21,0xc3,0xb4,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36        /*  à   */
,225,21,0xc4,0xb3,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36        /*  á   */
,226,22,0xc3,0xb2,0xb4,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36       /*  â   */
,227,25,0xc2,0xc3,0xc5,0xb2,0xb4,0xb5,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36        /*  ã   */
,228,21,0xb2,0xb4,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36        /*  ä   */
,229,27,0xd2,0xd3,0xd4,0xc2,0xc4,0xb2,0xb3,0xb4,0x91,0x92,0x93,0x94,0x85,0x75,0x62,0x63,0x64,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34,0x35,0x36      /*  å   */
,230,18,0x91,0x92,0x94,0x83,0x85,0x73,0x75,0x62,0x63,0x64,0x65,0x51,0x53,0x41,0x43,0x32,0x34,0x35       /*  æ   */
,231,17,0x92,0x93,0x94,0x95,0x81,0x71,0x61,0x51,0x41,0x32,0x33,0x34,0x35,0x23,0x14,0x03,0x04        /*  ç   */
,232,20,0xc3,0xb4,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x62,0x63,0x64,0x65,0x51,0x41,0x32,0x33,0x34,0x35     /*  è   */
,233,20,0xc4,0xb3,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x62,0x63,0x64,0x65,0x51,0x41,0x32,0x33,0x34,0x35     /*  é   */
,234,21,0xc3,0xb2,0xb4,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x62,0x63,0x64,0x65,0x51,0x41,0x32,0x33,0x34,0x35        /*  ê   */
,235,20,0xb2,0xb4,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x62,0x63,0x64,0x65,0x51,0x41,0x32,0x33,0x34,0x35     /*  ë   */
,236,11,0xc2,0xb3,0x91,0x92,0x93,0x83,0x73,0x63,0x53,0x43,0x33      /*  ì   */
,237,11,0xc4,0xb3,0x91,0x92,0x93,0x83,0x73,0x63,0x53,0x43,0x33      /*  í   */
,238,12,0xc3,0xb2,0xb4,0x91,0x92,0x93,0x83,0x73,0x63,0x53,0x43,0x33     /*  î   */
,239,11,0xb2,0xb4,0x91,0x92,0x93,0x83,0x73,0x63,0x53,0x43,0x33      /*  ï   */
,240,21,0xb1,0xb2,0xb4,0xa3,0x92,0x94,0x82,0x83,0x84,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34        /*  ð   */
,241,22,0xc2,0xc3,0xc5,0xb2,0xb4,0xb5,0x91,0x93,0x94,0x81,0x82,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x31,0x35       /*  ñ   */
,242,18,0xc3,0xb4,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  ò   */
,243,18,0xc4,0xb3,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  ó   */
,244,19,0xc3,0xb2,0xb4,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34      /*  ô   */
,245,22,0xc2,0xc3,0xc5,0xb2,0xb4,0xb5,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  õ   */
,246,18,0xb2,0xb4,0x92,0x93,0x94,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x45,0x32,0x33,0x34       /*  ö   */
,247,14,0x92,0x93,0x82,0x83,0x60,0x61,0x62,0x63,0x64,0x65,0x42,0x43,0x32,0x33       /*  ÷   */
,248,21,0x92,0x93,0x94,0x95,0x81,0x85,0x71,0x74,0x75,0x61,0x63,0x65,0x51,0x52,0x55,0x41,0x45,0x31,0x32,0x33,0x34        /*  ø   */
,249,18,0xc3,0xb4,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35       /*  ù   */
,250,18,0xc4,0xb3,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35       /*  ú   */
,251,19,0xc3,0xb2,0xb4,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35      /*  û   */
,252,18,0xb2,0xb4,0x91,0x95,0x81,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x44,0x45,0x32,0x33,0x35       /*  ü   */
,253,17,0xc4,0xb3,0x90,0x96,0x81,0x85,0x71,0x75,0x62,0x64,0x52,0x54,0x43,0x33,0x22,0x10,0x11        /*  ý   */
,254,22,0xb1,0xa1,0x91,0x93,0x94,0x81,0x82,0x85,0x71,0x75,0x61,0x65,0x51,0x55,0x41,0x42,0x45,0x31,0x33,0x34,0x21,0x11       /*  þ   */
,255,17,0xb2,0xb4,0x90,0x96,0x81,0x85,0x71,0x75,0x62,0x64,0x52,0x54,0x43,0x33,0x22,0x10,0x11        /*  ÿ   */
};



font650 monospaced650 = {
          .name     = "Monospaced 12"
        , .size     = 256
        , .first    = 0
        , .baseline = 2
        , .width    = 8     // 7 + 1
        , .height   = 12    //
        , .index    = monospaced_index
        , .glyphs   = monospaced_glyphs
};
