#include "HTTP.hpp"

const char net_entry[] PROGMEM = "{\"ssid\": \"_1\",\"rssi\": _2,\"enc\": _3}";

HTTPService::HTTPService() : server(80) {
    Service::register_service(
        this, "http", { }
    );
}

void HTTPService::init() {
    wifi = S("wifi", WiFiDriver);
    registry = S("registry", RegistryService);
    scroll = S("scroll", ScrollService);

    server.on("/wifi", HTTP_GET, [=]() {
        if (!authenticate())
            return server.requestAuthentication();

        auto& networks = wifi->scan();

        String json = "[";

        for (auto& net : networks) {
            String entry(FPSTR(net_entry));

            entry.replace("_1", net.SSID);
            entry.replace("_2", String(net.RSSI));
            entry.replace("_3", String(net.encryption));

            json += entry + ',';
        }

        json += ']';

        json.replace(",]", "]");

        server.send(200, "text/json", json);
    });
    server.on("/wifi", HTTP_POST, [=]() {
        if (!authenticate())
            return server.requestAuthentication();

        // TODO do some checks on arguments (maybe sanitize!!)
        String ssid = server.arg("ssid");
        String pkey = server.arg("pkey");

        // Send output first
        server.send(200, "text/json", F("\"OK\""));

        // then connect
        wifi->connect_to(ssid, pkey);
    });

    server.on("/message", HTTP_POST, [=]() {
        if (!authenticate())
            return server.requestAuthentication();

        // TODO handle buffer overflows with long strings
        //      (unless the server lib already handles it)
        String message = server.arg("m");

        server.send(200, "text/json", F("\"OK\""));

        if (message.length())
            scroll->message(message);
    });

    server.onNotFound([=](){
        server.send(404, "text/plain", F("Not found"));
    });

    server.begin();

    set_periodic_update(0, UPDATE_SYNC);
}

void HTTPService::update() {
    server.handleClient();
}

bool HTTPService::authenticate() {
    String user = registry->get("http_username", String());
    String pass = registry->get("http_password", String());

    return server.authenticate(user.c_str(), pass.c_str());
}
