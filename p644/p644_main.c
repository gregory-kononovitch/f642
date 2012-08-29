/*
 * p644_main.cpp
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
#include "p644_osc.h"
#include "p644_point.h"

extern "C" {
#include "../../f610-equa/f611/f611_include.h"
}

using namespace p644;


int main(int argc, char *argv[]) {
  int r = 0, i, fd;
  char file_name[256];
  struct timeval tv1, tv2;
  //
  uint8_t *img;
  img = (uint8_t*) malloc(3 * 800 * 450);
  //
  struct output_stream *stream = f611_init_output("cbrodger", 3, 800, 450, 2);

  //
  gettimeofday(&tv1, NULL);
  srand(tv1.tv_sec);
  //
  Brodger brodge = Brodger(800, 450);
  for(i = 0 ; i < 45 ; i++) {
    //Brodger brodge = Brodger(800, 450, 3);

    gettimeofday(&tv1, NULL);
    brodge.brodge(img);
    gettimeofday(&tv2, NULL);

    if (tv2.tv_usec - tv1.tv_usec < 0)
      printf("Brodger done in %ld.%ld.\n", tv2.tv_sec - tv1.tv_sec - 1, (1000000 + tv2.tv_usec - tv1.tv_usec)/1000);
    else
      printf("Brodger done in %ld.%ld\n", tv2.tv_sec - tv1.tv_sec, (tv2.tv_usec - tv1.tv_usec)/1000);

    //
    f611_add_frame(stream, img);

    //
//    sprintf(file_name, "/work/test/brodger-%d.raw", i);
//    fd = open(file_name, O_RDWR | O_CREAT);
//    if (fd < 0) {
//      printf("Pb opening file, exiting");
//      return fd;
//    }
//
//    write(fd, img, 3 * brodge.width * brodge.height);
//    printf("Image copied.\n");
//    r = close(fd);
  }

  return r;
}
