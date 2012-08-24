/*
 * file    : f643.h
 * project : f640
 *
 * Created on: Aug 24, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#ifndef F643_H_
#define F643_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <lame/lame.h>
#include <mp4v2/mp4v2.h>


/*
 * From flv, concerning x264 payloads :
 * 1) First frame contains a ~header :
 *      - The nal[2].p_payload from x264_encoder_parameters (+ x264_encoder_headers)
 *      - The frame nal[0].p_payload
 * 2) Frames nal[0].p_payload
 *
 * => The 0th nal[2].p_payload contains X264 parameters {(name, value)}
 */




#endif /* F643_H_ */
