#include "SlackApplication.hpp"

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "../services/Scroll.hpp"

const char ping_message[] PROGMEM = "{\"id\": \"_1\",\"type\": \"ping\"}";

SlackApplication::SlackApplication() {
    Service::RegisterService(
        this, "slack_app", { "message-received" }
    );
}

void SlackApplication::Init() {
    _wifi = S("wifi", WiFiDriver);
    _registry = S("registry", RegistryService);
    ws = S("ws", WebSocket);

    message_counter = 0;

    // Setup ping every 5 seconds
    // ------------------------------------

    SetPeriodicUpdate(5000, UPDATE_SYNC);
    DisablePeriodicUpdate();

    // Setup event handlers
    // -------------------------------------

    // Connect to Slack only when internet is available
    Service::BindEvent("wifi.connected", [=](Service* s) { connect(); });

    Service::BindEvent("ws.connected", [=](Service* s) { EnablePeriodicUpdate(); });
    Service::BindEvent("ws.disconnected", [=](Service* s) { DisablePeriodicUpdate(); });
    Service::BindEvent("ws.data-received", [=](Service* s) { _onDataReceived(); });
}

void SlackApplication::Update() {
    if (!ws->isConnected()) {
        SERVICE_PRINT("WS not connected\n");
        return;
    }

    SERVICE_PRINT("Ping to Slack\n");

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root[F("id")] = message_counter++;
    root[F("type")] = F("ping");

    ws->send(root);
}

void SlackApplication::connect() {
    SERVICE_PRINT("Connecting to Slack\n");

    String token = _registry->Get<String>("slack_token");
    String fingerprint = _registry->Get<String>("slack_ssl_fingerprint");

    String slack_url = "https://slack.com/api/rtm.connect?token=" + token;

    // Auth to Slack and get a websocket url
    // -------------------------------------

    HTTPClient http;

    http.begin(slack_url, fingerprint);
    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        SERVICE_PRINTF("HTTP GET failed with code %d\n", httpCode);
        return;
    }

    // Parse the response
    // -------------------------------------

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(http.getStream());

    bool success = root[F("ok")];

    if (!success) {
        SERVICE_PRINT("Auth error\n");
        return;
    }

    ScrollService* _Scroll = S("scroll", ScrollService);
    _Scroll->message(root[F("team")][F("name")]);

    // Get the ws url and escape it
    String url = root[F("url")];
    url.replace("\\/", "/");

    // Search the first path separator, skipping the initial "wss://"
    int i = url.indexOf('/', 6);

    String host = url.substring(6, i);
    String path = url.substring(i);

    // Open WebSocket connection and register event handler
    // -------------------------------------

    SERVICE_PRINTF("WebSocket Host=%s Path=%s\n", host.c_str(), path.c_str());
    ws->secureConnectTo(host, path);
}

void SlackApplication::_onDataReceived() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(ws->getLastPayload());

    if (root[F("type")] == F("message")) {
        // {"type":"message","channel":"D85DUKSBX","user":"U7V0QCDLZ","text":"Hello","ts":"1511738389.000132","source_team":"T7VJ72AVB","team":"T7VJ72AVB"}
        ScrollService* _Scroll = S("scroll", ScrollService);
        _Scroll->message(root[F("text")]);
    }

    //SERVICE_PRINTF("Type is %s\n", );
}