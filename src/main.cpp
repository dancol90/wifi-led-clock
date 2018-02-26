/*
  Drivers (still services):
   - [OK] Led display
   - [OK] Time
   - [OK] WiFi
   - [OK] File System

  List of needed services:
   - [OK] Scroll management
   - [OK] Configuration (over SPIFFS)
   - [OK*] HTTP Server (?)
   - [OK] NTP
   - MQTT handling
*/

#include <Arduino.h>
#include "Service.hpp"

#include "drivers/LED.hpp"
#include "drivers/Clock.hpp"
#include "drivers/WiFi.hpp"
#include "drivers/FS.hpp"
#include "services/Scroll.hpp"
#include "services/HTTP.hpp"
#include "services/Registry.hpp"
#include "services/NTP.hpp"
#include "services/WebSocket.hpp"
#include "services/OTA.hpp"
#include "application/SlackApplication.hpp"
#include "application/ClockApplication.hpp"


void setup()
{
    Serial.begin(115200);

    Service* drivers[] = {
        // "Priority" services & drivers
        new FileSystemDriver(),
        new RegistryService(),

        // Drivers
        new ClockDriver(),
        new WiFiDriver(),
        new LedMatrixDriver(),

        // Services
        new ScrollService(),
        new HTTPService(),
        new NTPService(),
        new WebSocket(),
        new OTAService(),

        // Application
        //new SlackApplication(),
        new ClockApplication()
    };

    Service::InitAll();
}

void loop()
{
    Service::SyncUpdate();
    delay(0);
}
