//
// Created by maba_kalox on 1/4/21.
//
#include <cmath>
#include "rgb2double.h"

double rgb2double(int rgb) {
    return floor((double)rgb / 255 * 1000) / 1000;
}
