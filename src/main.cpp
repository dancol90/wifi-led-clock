/*
  Drivers (still services):
   - [OK] Led display
   - [OK] Time
   - [OK] WiFi
   - [OK] File System

  List of needed services:
   - [OK] Scroll management
   - Configuration (over SPIFFS)
   - [OK*] HTTP Server (?)
   - MQTT handling
*/

#include <Arduino.h>
#include "Service.hpp"

#include "drivers/LED.hpp"
#include "drivers/Clock.hpp"
#include "drivers/WiFi.hpp"
#include "services/Scroll.hpp"
#include "services/HTTP.hpp"

void setup() {
    Serial.begin(115200);

    Service* drivers[] = {
        // Drivers
        new LedMatrixDriver(),
        new ClockDriver(),
        new WiFiDriver(),

        // Services
        new ScrollService(),
        new HTTPService()
    };

    Service::init_all();

    Service::bind_event("clock.time-changed", [](Service* clock) {
        Time& t = ((ClockDriver*)clock)->now();
        LedMatrixDriver* led = S("display", LedMatrixDriver);

        if (t.second % 30 == 0) {
            S("scroll", ScrollService)->message("%d %d %d", t.day, t.month, t.year);
            return;
        }

        led->clear_buffer();

        led->draw_digit_big(0,  t.hour / 10);
        led->draw_digit_big(4,  t.hour % 10);

        led->draw_digit_big(9,  t.minute / 10);
        led->draw_digit_big(13, t.minute % 10);

        led->draw_digit_small(18, 3, t.second / 10);
        led->draw_digit_small(21, 3, t.second % 10);

        led->update();
    });
}

void loop() {
    Service::sync_update();
    delay(0);
}
