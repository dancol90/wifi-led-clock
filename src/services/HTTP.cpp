#include "HTTP.hpp"

const char net_entry[] PROGMEM = "{\"ssid\": \"_1\",\"rssi\": _2,\"enc\": _3}";

HTTPService::HTTPService() : server(80) {
    Service::register_service(
        this, "http", { }
    );
}

void HTTPService::init() {
    wifi = S("wifi", WiFiDriver);

    server.on("/", HTTP_GET, [=]() {
        // TODO load html page from SPIFS
        server.send(200, "text/html", "Home");
    });

    server.on("/wifi", HTTP_GET, [=]() {
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
    server.on("/wifi", HTTP_POST,[=]() {
        // TODO take the parameters "ssid" & "pkey" as wifi credentials
        String ssid = server.arg("ssid");
        String pkey = server.arg("pkey");

        Serial.printf("%s, %s\n", ssid.c_str(), pkey.c_str());

        wifi->set_connection(ssid, pkey);

        server.send(200, "text/json", F("\"Ok\""));
    });

    server.onNotFound([=](){
        server.send(404, "text/plain", F("Not found"));
    });

    server.begin();

    // set_periodic_update(5);
}

void HTTPService::update() {
    server.handleClient();
}
