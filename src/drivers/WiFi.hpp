#include "../Service.hpp"

#include <ESP8266WiFi.h>

#ifndef WIFI_HPP
#define WIFI_HPP

struct WiFiNetwork {
    String SSID;
    int RSSI;
    int encryption;
};

class WiFiDriver : public Service {
public:
    typedef std::vector<WiFiNetwork> NetworkList;

    WiFiDriver();

    void init();
    void update();

    void enable_acess_point();
    void disable_acess_point();
    void set_connection(String ssid, String pkey);

    NetworkList& scan();
    NetworkList& last_scan_result();
private:
    wl_status_t prev_status;

    NetworkList networks;
};

#endif
