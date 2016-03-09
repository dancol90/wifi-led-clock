#include "../Service.hpp"
#include "../drivers/WiFi.hpp"

#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef HTTP_HPP
#define HTTP_HPP

class HTTPService : public Service {
public:
    HTTPService();

    void init();
    void update();

private:
    WiFiDriver* wifi;
    ESP8266WebServer server;
};

#endif
