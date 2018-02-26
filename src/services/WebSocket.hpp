#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#include "../Service.hpp"
#include "../drivers/WiFi.hpp"

#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

class WebSocket : public Service
{
public:
    WebSocket();

    void Init();
    void Update();

    void secureConnectTo(String& host, String& path);

    void send(String& data);
    void send(JsonObject& json);

    byte* getLastPayload() const;
    int getLastPayloadLength() const;

    bool isConnected() const;

private:
    WiFiDriver* _wifi;

    WebSocketsClient _webSocket;
    bool _connected;

    byte* _lastPayload;
    int _lastPayloadLength;

    void _onWebSocket(WStype_t type, uint8_t *payload, size_t len);
};

#endif
