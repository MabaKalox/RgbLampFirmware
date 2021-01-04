//
// Created by maba_kalox on 12/29/20.
//

#include "PWMLED.h"
#include <Arduino.h>

PWMLED::PWMLED(int PIN, int channel, int frequency, int resolution, double absolute_brightness):
    PIN(PIN),
    channel(channel),
    frequency(frequency),
    resolution(resolution),
    absolute_brightness(absolute_brightness) {}

void PWMLED::attach() const {
    ledcSetup(channel, frequency, resolution);
    ledcAttachPin(PIN, channel);
    ledcWrite(channel, 0);
}

double PWMLED::setBrightness(double new_brightness) {
    brightness = new_brightness;
    const int duty = (int)round(pow(2, resolution) * brightness * absolute_brightness);
    ledcWrite(channel, duty);
}


