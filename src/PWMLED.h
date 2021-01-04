//
// Created by maba_kalox on 12/29/20.
//

#ifndef LED_STRIP_PWMLED_H
#define LED_STRIP_PWMLED_H


class PWMLED {
private:
    const int PIN;
    const int channel;
    const int frequency;
    const int resolution;
    double brightness = 0;
    const double absolute_brightness;
public:
    PWMLED(int PIN, int channel, int frequency, int resolution, double absolute_brightness = 0.8);
    double setBrightness(double new_brightness);
    void attach() const;
};


#endif //LED_STRIP_PWMLED_H
