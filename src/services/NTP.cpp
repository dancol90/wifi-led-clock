#include "NTP.hpp"

extern "C"
{
    #include <sntp.h>
}

// Constants to compensate timestamp differences
const unsigned long JAN_1_2010 = 1262304000UL;

NTPService::NTPService() : Service()
{
    Service::RegisterService(this, "ntp", {});
}

void NTPService::Init()
{
    _wifi = S("wifi", WiFiDriver);
    _Clock = S("clock", ClockDriver);
    _registry = S("registry", RegistryService);

    Service::BindEvent("wifi.connected", [=](Service* s)
    {
        SERVICE_PRINT("Fetching current time with NTP\n");

        String host = _registry->Get("ntp_host", NTP_SERVER_HOST);
        int timezone = _registry->Get("ntp_timezone", NTP_TIMEZONE);

        sntp_setservername(0, (char*)host.c_str());
        sntp_set_timezone(timezone);

        sntp_init();

        // Wait a moment to let the sntp system to start.
        delay(200);

        // Espressif API uses the GMT+8 timezone
        unsigned long current_stamp = sntp_get_current_timestamp();

        // Compensate for the wrong timezone and change reference year to 2010
        _Clock->Set(current_stamp - JAN_1_2010);
    });
}

void NTPService::Update() {}
