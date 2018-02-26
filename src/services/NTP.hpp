#include "../Service.hpp"
#include "../drivers/WiFi.hpp"
#include "../drivers/Clock.hpp"
#include "../services/Registry.hpp"

#ifndef NTP_HPP
#define NTP_HPP

// Configuration defaults
const String NTP_SERVER_HOST = "ntp1.inrim.it";
const int NTP_TIMEZONE = +1;

class NTPService : public Service
{
public:
    NTPService();

    void Init();
    void Update();

private:
    WiFiDriver* _wifi;
    ClockDriver* _Clock;
    RegistryService* _registry;
};

#endif
