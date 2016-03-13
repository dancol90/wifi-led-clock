#include "../Service.hpp"
#include "../drivers/WiFi.hpp"
#include "../drivers/Clock.hpp"

#ifndef NTP_HPP
#define NTP_HPP

class NTPService : public Service {
public:
    NTPService();

    void init();
    void update();

private:
    WiFiDriver* wifi;
    ClockDriver* clock;
};

#endif
