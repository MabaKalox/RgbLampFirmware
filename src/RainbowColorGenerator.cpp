//
// Created by maba_kalox on 12/31/20.
//

#include "RainbowColorGenerator.h"
#include <cmath>
#include "tuple"

RainbowColorGenerator::RainbowColorGenerator(int len, int phase_0,
                                             int phase_1, int phase_2
): len(len), phase_0(phase_0), phase_1(phase_1), phase_2(phase_2) {}

std::tuple<double, double, double> RainbowColorGenerator::get_next() {
    const double freq = 6.4 / len;
    double red = (sin(freq*i + 0) + 1) / 2;
    double green = (sin(freq*i + 2) + 1) / 2;
    double blue = (sin(freq*i + 4) + 1) / 2;
    if (i < len - 1) {
        i++;
    }
    else {
        i = 0;
    }
    return std::make_tuple(red, green, blue);
}

void RainbowColorGenerator::setLen(int new_len) {
    i = (int)((double)i / len * new_len);
    len = new_len;
}


