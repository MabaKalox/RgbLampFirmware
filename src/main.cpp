#include <Arduino.h>
#include "PWMLED.h"
#include "RainbowColorGenerator.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "WiFi.h"
#include <tuple>
#include "vector"
#include "rgb2double.h"

std::vector<std::tuple<const char *, const char *>> known_wifi_list = {
        std::make_tuple("Family", "12019710730"),
        std::make_tuple("Maba_Kalox", "28271901")
};

AsyncWebServer server(80);

bool connect_to_wifi() {
    const int max_attempts_to_connect_wifi = 10;
    IPAddress local_IP(192, 168, 0, 109);
    IPAddress gateway(192, 168, 0, 22);
    IPAddress subnet(255, 255, 0, 0);

    bool is_connected = false;

    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }

    for (std::tuple<const char *, const char *> wifi_data: known_wifi_list) {
        WiFi.begin(
                std::get<0>(wifi_data),
                std::get<1>(wifi_data)
        );
        int connect_attempts_done = 0;
        while (!is_connected && connect_attempts_done < max_attempts_to_connect_wifi) {
            is_connected = (WiFiClass::status() == WL_CONNECTED);
            connect_attempts_done++;
            Serial.println("Connecting to WiFi..");
            delay(500);
        }
        if (is_connected) {
            Serial.println(WiFi.localIP());
            Serial.println(WiFi.gatewayIP());
            return true;
        } else {
            WiFi.disconnect();
        }
    }
    return false;
}

const int freq = 5000;
const int resolution = 8;
unsigned long last_leds_change_time = 0;
unsigned long leds_change_delay = 50;
int rainbow_speed_multiplier = 1;

double rgbDoubleArray[3] = {1, 1, 1};
double overall_brightness = 1;
PWMLED red_led(12, 0, freq, resolution);
PWMLED green_led(14, 1, freq, resolution);
PWMLED blue_led(27, 2, freq, resolution);
const int rainbow_res = 2000;
RainbowColorGenerator rainbowGen(rainbow_res);

const int default_mode = 1;
int mode;
bool update_required = true;


void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin()) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    red_led.attach();
    green_led.attach();
    blue_led.attach();
    mode = default_mode;

    if (connect_to_wifi()) {
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->redirect("/frontend");
        });

        server.serveStatic("/frontend", SPIFFS, "/index.html");

        server.on("/bt", HTTP_GET, [](AsyncWebServerRequest *request) {
            int params = request->params();
            for (int i = 0; i < params; i++) {
                AsyncWebParameter *p = request->getParam(i);
                if (strcmp(p->name().c_str(), "mode") == 0) {
                    mode = atol(p->value().c_str());
                }
                Serial.printf("%s: %s\n", p->name().c_str(), p->value().c_str());
            }
            update_required = true;
            request->send(200, "text/plain", "{status: \"ok\"}");
        });

        server.on("/upload_settings", HTTP_POST, [](AsyncWebServerRequest *request) {
            int params = request->params();
            for (int i = 0; i < params; i++) {
                AsyncWebParameter *p = request->getParam(i);
                if (strcmp(p->name().c_str(), "mode") == 0) {
                    mode = atol(p->value().c_str());
                }
                Serial.printf("%s: %s\n", p->name().c_str(), p->value().c_str());
            }
            if (request->hasParam("mode", true)) {
                auto param = request->getParam("mode", true);
                mode = param->value().toInt();
            }
            if (request->hasParam("red", true)) {
                auto param = request->getParam("red", true);
                rgbDoubleArray[0] = rgb2double(param->value().toInt());
            }
            if (request->hasParam("green", true)) {
                auto param = request->getParam("green", true);
                rgbDoubleArray[1] = rgb2double(param->value().toInt());
            }
            if (request->hasParam("blue", true)) {
                auto param = request->getParam("blue", true);
                rgbDoubleArray[2] = rgb2double(param->value().toInt());
            }
            if (request->hasParam("overall_brightness", true)) {
                auto param = request->getParam("overall_brightness", true);
                overall_brightness = param->value().toDouble();
            }
            if (request->hasParam("rainbow_speed", true)) {
                auto param = request->getParam("rainbow_speed", true);
                rainbow_speed_multiplier = param->value().toInt();
            }

            update_required = true;
            request->send(200, "text/plain", "{status: \"ok\"}");
        });

        server.begin();
    }
}

void loop() {
    if (mode == 0 && update_required) {
        red_led.setBrightness(0);
        green_led.setBrightness(0);
        blue_led.setBrightness(0);
        update_required = false;
    } else if (mode == 1 && update_required) {
        red_led.setBrightness(1*overall_brightness);
        green_led.setBrightness(1*overall_brightness);
        blue_led.setBrightness(1* overall_brightness);
        update_required = false;
    } else if (mode == 2 && rainbow_speed_multiplier && millis() - last_leds_change_time >= leds_change_delay) {

        rainbowGen.setLen(rainbow_res / rainbow_speed_multiplier);
        auto new_color = rainbowGen.get_next();

        red_led.setBrightness(std::get<0>(new_color) * overall_brightness);
        green_led.setBrightness(std::get<1>(new_color) * overall_brightness);
        blue_led.setBrightness(std::get<2>(new_color) * overall_brightness);
        last_leds_change_time = millis();
    } else if (mode == 3 && update_required) {
        red_led.setBrightness(rgbDoubleArray[0] * overall_brightness);
        green_led.setBrightness(rgbDoubleArray[1] * overall_brightness);
        blue_led.setBrightness(rgbDoubleArray[2] * overall_brightness);
        update_required = false;
    }
}