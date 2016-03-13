#include "NTP.hpp"

extern "C" {
    #include <sntp.h>
}

// TODO: move this to registry
const char* NTP_SERVER_HOST = "ntp1.inrim.it";

// Constants to compensate timestamp differences
const unsigned long GMT8 = 8UL * 60 * 60;
const unsigned long JAN_1_2010 = 1262304000UL;

NTPService::NTPService() : Service() {
    Service::register_service(this, "ntp", {});
}

void NTPService::init() {
    wifi = S("wifi", WiFiDriver);
    clock = S("clock", ClockDriver);

    Service::bind_event("wifi.connected", [=](Service* s) {
        SERVICE_PRINT("Fetching current time with NTP");

        sntp_setservername(0, (char*)NTP_SERVER_HOST);

        sntp_init();

        // Wait a moment to let the sntp system to start.
        delay(200);

        // Espressif API uses the GMT+8 timezone
        unsigned long current_stamp = sntp_get_current_timestamp();

        // Compensate for the wrong timezone and change reference year to 2010
        clock->set(current_stamp - GMT8 - JAN_1_2010);
    });
}

void NTPService::update() {}
