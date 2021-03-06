#include "../Service.hpp"
#include "../drivers/WiFi.hpp"
#include "../services/Registry.hpp"
#include "../services/Scroll.hpp"

#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef HTTP_HPP
#define HTTP_HPP

class HTTPService : public Service
{
public:
    HTTPService();

    void Init();
    void Update();

private:
    WiFiDriver* _wifi;
    RegistryService* _registry;
    ScrollService* _Scroll;

    ESP8266WebServer _server;

    bool Authenticate();
};

#endif
