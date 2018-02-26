#include "../Service.hpp"
#include "../drivers/WiFi.hpp"
#include "../services/WebSocket.hpp"
#include "../services/Registry.hpp"

#ifndef SLACKAPPLICATION_HPP
#define SLACKAPPLICATION_HPP

class SlackApplication : public Service {
public:
    SlackApplication();

    void Init();
    void Update();

private:
    WiFiDriver* _wifi;
    WebSocket* ws;
    RegistryService* _registry;

    int message_counter;

    void connect();

    void _onDataReceived();
};

#endif
