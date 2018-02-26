#include "WebSocket.hpp"

using namespace std::placeholders;

WebSocket::WebSocket()
{
    Service::RegisterService(
        this, "ws", { "connected", "disconnected", "data-received", "error" }
    );
}

void WebSocket::Init()
{
    _connected = false;
    _lastPayload = nullptr;
    _lastPayloadLength = 0;

    // Update at every cycle
    SetPeriodicUpdate(0, UPDATE_SYNC);
}

void WebSocket::Update()
{
    _webSocket.loop();
}

void WebSocket::secureConnectTo(String& host, String& path)
{
    _webSocket.onEvent(std::bind(&WebSocket::_onWebSocket, this, _1, _2, _3));
    _webSocket.beginSSL(host, 443, path, "", "");
}

void WebSocket::send(String& text)
{
    this->_webSocket.sendTXT(text);
}

void WebSocket::send(JsonObject& json)
{
    String text;
    json.prettyPrintTo(text);

    send(text);
}

bool WebSocket::isConnected() const { return _connected; }

byte* WebSocket::getLastPayload() const { return _lastPayload; }
int WebSocket::getLastPayloadLength() const { return _lastPayloadLength; }

void WebSocket::_onWebSocket(WStype_t type, uint8_t* payload, size_t len)
{
    _lastPayload = payload;
    _lastPayloadLength = len;

    switch (type)
    {
        case WStype_CONNECTED:
            SERVICE_PRINT("Connected\n");
            _connected = true;
            FireEvent(this, "ws.connected");
            break;

        case WStype_TEXT:
            SERVICE_PRINTF("Message: %s\n", payload);
            FireEvent(this, "ws.data-received");
            break;

        case WStype_DISCONNECTED:
            SERVICE_PRINT("Disconnected\n");
            _connected = false;
            FireEvent(this, "ws.disconnected");
            break;

        case WStype_ERROR:
            SERVICE_PRINTF("Error: %s\n", payload);
            _connected = false;
            FireEvent(this, "ws.error");
            break;
    }

    _lastPayload = nullptr;
    _lastPayloadLength = 0;
}