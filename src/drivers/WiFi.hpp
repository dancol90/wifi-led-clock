#include "../Service.hpp"
#include "../services/Registry.hpp"

#include <ESP8266WiFi.h>

#ifndef WIFI_HPP
#define WIFI_HPP

const String WIFI_AP_SSID = "ESP8266";

struct WiFiNetwork
{
    String SSID;
    int RSSI;
    int encryption;
};

class WiFiDriver : public Service
{
public:
    typedef std::vector<WiFiNetwork> NetworkList;

    WiFiDriver();

    void Init();
    void Update();

    void EnableAccessPoint();
    void DisableAccessPoint();
    void ConnectTo(String ssid, String pkey);

    NetworkList& Scan();
    NetworkList& LastScanResult();
private:
    RegistryService* _Registry;

    wl_status_t _PrevStatus;
    NetworkList _Networks;
};

#endif
