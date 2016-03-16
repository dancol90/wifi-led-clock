#include "NTP.hpp"

extern "C" {
    #include <sntp.h>
}

// Constants to compensate timestamp differences
const unsigned long JAN_1_2010 = 1262304000UL;

NTPService::NTPService() : Service() {
    Service::register_service(this, "ntp", {});
}

void NTPService::init() {
    wifi = S("wifi", WiFiDriver);
    clock = S("clock", ClockDriver);
    registry = S("registry", RegistryService);

    Service::bind_event("wifi.connected", [=](Service* s) {
        SERVICE_PRINT("Fetching current time with NTP");

        String host  = registry->get("ntp_host", NTP_SERVER_HOST);
        int timezone = registry->get("ntp_timezone", NTP_TIMEZONE);

        sntp_setservername(0, (char*)host.c_str());
        sntp_set_timezone(timezone);

        sntp_init();

        // Wait a moment to let the sntp system to start.
        delay(200);

        // Espressif API uses the GMT+8 timezone
        unsigned long current_stamp = sntp_get_current_timestamp();

        // Compensate for the wrong timezone and change reference year to 2010
        clock->set(current_stamp - JAN_1_2010);
    });
}

void NTPService::update() {}
