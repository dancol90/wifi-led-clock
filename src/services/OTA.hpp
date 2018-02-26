#include "../Service.hpp"
#include "../drivers/WiFi.hpp"
#include "../drivers/LED.hpp"
#include "../services/Registry.hpp"

#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef OTA_HPP
#define OTA_HPP

class OTAService : public Service
{
public:
    OTAService();

    void Init();
    void Update();

private:
    WiFiDriver* _wifi;
    RegistryService* _registry;
    LedMatrixDriver* _Led;

    void OnWifiConnected();
};

#endif
