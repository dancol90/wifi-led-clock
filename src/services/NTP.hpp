#include "../Service.hpp"
#include "../drivers/WiFi.hpp"
#include "../drivers/Clock.hpp"
#include "../services/Registry.hpp"

#ifndef NTP_HPP
#define NTP_HPP

// Configuration defaults
const String NTP_SERVER_HOST = "ntp1.inrim.it";
const int NTP_TIMEZONE = +1;

class NTPService : public Service {
public:
    NTPService();

    void init();
    void update();

private:
    WiFiDriver* wifi;
    ClockDriver* clock;

    RegistryService* registry;
};

#endif
