#include "../Service.hpp"
#include "../services/Registry.hpp"

#include <ESP8266WiFi.h>

#ifndef WIFI_HPP
#define WIFI_HPP

const String WIFI_AP_SSID = "ESP8266";

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
    void connect_to(String ssid, String pkey);

    NetworkList& scan();
    NetworkList& last_scan_result();
private:
    RegistryService* registry;

    wl_status_t prev_status;
    NetworkList networks;
};

#endif
