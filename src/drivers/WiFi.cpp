#include "WiFi.hpp"

#include <Arduino.h>

WiFiDriver::WiFiDriver() {
    Service::register_service(this, "wifi", {
        "disconnected", "connected", "failed", "ap-mode"
    });
}

void WiFiDriver::init() {
    // Just for now, delete all saved info
    WiFi.disconnect();

    // Ensure that we are in STA mode
    WiFi.mode(WIFI_STA);
    // Save connection info (SSID & key) to flash
    WiFi.persistent(true);

    prev_status = WiFi.status();

    if (WiFi.SSID().length() > 0)
        WiFi.begin();
    else
        enable_acess_point();

    set_periodic_update(1000);
}

void WiFiDriver::update() {
    wl_status_t status = WiFi.status();

    SERVICE_PRINTF("WiFi update: prev %d, now %d\n", prev_status, status);

    /*
        If not configured, wifi is in IDLE,
        If configured well, wifi is DISCONNECTED, then CONNECTED,
        If configured wrong, wifi is DISCONNECTED, then FAILED or NO_SSID_AVAIL
        So I can assume DISCONNECTED status as "hold on, something is going to happen"
    */

    if (status != prev_status) {
        switch (status) {
            case WL_CONNECTED:
                // Here, it connected
                SERVICE_PRINTF("WiFi connected with IP %s\n", WiFi.localIP().toString().c_str());

                disable_acess_point();

                Service::fire_event(this, "wifi.connected");
                break;
            case WL_CONNECTION_LOST:
                // What about WL_DISCONNECTED? It's the state after this
                // Here it has lost connection
                SERVICE_PRINT("WiFi disconnected");

                Service::fire_event(this, "wifi.disconnected");
                break;
            case WL_NO_SSID_AVAIL:
            case WL_CONNECT_FAILED:
                // Wrong config, cannot connect, start AP mode
                SERVICE_PRINT("WiFi failed connection");

                Service::fire_event(this, "wifi.failed");

                enable_acess_point();

                break;
        }

        prev_status = status;
    }
}

void WiFiDriver::set_connection(String ssid, String pkey) {
    SERVICE_PRINTF("WiFi connecting to new network %s (key: %s)\n", ssid.c_str(), pkey.c_str());
    // Enable STA and set network info
    WiFi.begin(ssid.c_str(), pkey.c_str());
}

void WiFiDriver::enable_acess_point() {
    // Do nothing if AP is already on
    if (WiFi.getMode() & WIFI_AP) {
        SERVICE_PRINT("WiFi access point already enabled");
        return;
    }

    SERVICE_PRINT("WiFi enabling access point");

    // Setup and turn on the access point
    // TODO load ssid & co. from configuration
    WiFi.softAP("ESP8266");

    SERVICE_PRINTF("AP IP address is %s", WiFi.softAPIP().toString().c_str());

    Service::fire_event(this, "wifi.ap-mode");
}

void WiFiDriver::disable_acess_point() {
    SERVICE_PRINT("WiFi disabling access point");
    WiFi.enableAP(false);
}


WiFiDriver::NetworkList& WiFiDriver::scan() {
    int count = WiFi.scanNetworks();

    networks.clear();

    for (int i = 0; i < count; i++) {
        networks.push_back({
            WiFi.SSID(i),
            WiFi.RSSI(i),
            WiFi.encryptionType(i)
        });
    }

    return networks;
}


WiFiDriver::NetworkList& WiFiDriver::last_scan_result() {
    return networks;
}
