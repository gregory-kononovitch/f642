/*
 * file    : f642_x264.cc
 * project : f640
 *
 * Created on: Aug 22, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f642_x264.hh"


// Encoding
t508::f642::X264(int width, int height, int fps, const char *path) {

}

t508::f642::X264::~X264() {

}
//
int t508::f642::X264::setPresets(int preset, int tune) {
    return 0;
}

int t508::f642::X264::setQP(int qp) {
    return 0;
}

int t508::f642::X264::setQP(int qpmin, int qpmax, int qpstep) {
    return 0;
}

int t508::f642::X264::setParam(const char *name, const char *value) {
    return 0;
}

int t508::f642::X264::addFrame(uint8_t *rgb) {
    return 0;
}

int t508::f642::X264::closeFile() {
    return 0;
}

