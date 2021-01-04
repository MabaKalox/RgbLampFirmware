//
// Created by maba_kalox on 12/31/20.
//

#ifndef LED_STRIP_RAINBOWCOLORGENERATOR_H
#define LED_STRIP_RAINBOWCOLORGENERATOR_H

#include <iostream>


class RainbowColorGenerator {
private:
    int len;
    int i = 0;
    const int phase_0;
    const int phase_1;
    const int phase_2;
public:
    RainbowColorGenerator(int len,
                          int phase_0 = 0, int phase_1 = 2,
                          int phase_2 = 4
    );
    void setLen(int new_len);

    std::tuple<double, double, double> get_next();
};


#endif //LED_STRIP_RAINBOWCOLORGENERATOR_H
