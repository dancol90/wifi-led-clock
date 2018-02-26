#include "WiFi.hpp"

#include <Arduino.h>

WiFiDriver::WiFiDriver()
{
    Service::RegisterService(this, "wifi", {
        "disconnected", "connected", "failed", "ap-mode"
    });
}

void WiFiDriver::Init()
{
    _Registry = S("registry", RegistryService);

    // Ensure that we are in STA mode
    WiFi.mode(WIFI_STA);
    // Save connection info (SSID & key) to flash
    WiFi.persistent(true);

    _PrevStatus = WiFi.status();

    if (WiFi.SSID().length() > 0)
        WiFi.begin();
    else
        EnableAccessPoint();

    SetPeriodicUpdate(1000, UPDATE_SYNC);
}

void WiFiDriver::Update()
{
    wl_status_t status = WiFi.status();

    //SERVICE_PRINTF("WiFi update: prev %d, now %d\n", _PrevStatus, status);

    /*
        If not configured, wifi is in IDLE,
        If configured well, wifi is DISCONNECTED, then CONNECTED,
        If configured wrong, wifi is DISCONNECTED, then FAILED or NO_SSID_AVAIL
        So I can assume DISCONNECTED status as "hold on, something is going to happen"
    */

    if (status != _PrevStatus)
    {
        switch (status)
        {
            case WL_CONNECTED:
                // Here, it connected
                SERVICE_PRINTF("WiFi connected with IP %s\n", WiFi.localIP().toString().c_str());

                DisableAccessPoint();

                Service::FireEvent(this, "wifi.connected");
                break;
            case WL_CONNECTION_LOST:
                // What about WL_DISCONNECTED? It's the state after this
                // Here it has lost connection
                SERVICE_PRINT("WiFi disconnected\n");

                Service::FireEvent(this, "wifi.disconnected");
                break;
            case WL_NO_SSID_AVAIL:
            case WL_CONNECT_FAILED:
                // Wrong config, cannot connect, start AP mode
                SERVICE_PRINT("WiFi failed connection\n");

                Service::FireEvent(this, "wifi.failed");

                EnableAccessPoint();

                break;
        }

        _PrevStatus = status;
    }
}

void WiFiDriver::ConnectTo(String ssid, String pkey)
{
    SERVICE_PRINTF("WiFi connecting to new network %s (key: %s)\n", ssid.c_str(), pkey.c_str());
    // Enable STA and set network info
    WiFi.begin(ssid.c_str(), pkey.c_str());
}

void WiFiDriver::EnableAccessPoint()
{
    // Do nothing if AP is already on
    if (WiFi.getMode() & WIFI_AP)
    {
        SERVICE_PRINT("WiFi access point already enabled\n");
    }
    else
    {
        SERVICE_PRINT("WiFi enabling access point\n");

        // Setup and turn on the access point
        String ssid = _Registry->Get("wifi_ap_ssid", WIFI_AP_SSID);
        WiFi.softAP(ssid.c_str());

        SERVICE_PRINTF("AP IP address is %s\n", WiFi.softAPIP().toString().c_str());

        Service::FireEvent(this, "wifi.ap-mode");
    }
}

void WiFiDriver::DisableAccessPoint()
{
    if (WiFi.getMode() & WIFI_AP)
    {
        SERVICE_PRINT("WiFi disabling access point\n");
        WiFi.softAPdisconnect(true);
    }
    else
    {
        SERVICE_PRINT("WiFi access point already disabled\n");
    }
}


WiFiDriver::NetworkList& WiFiDriver::Scan()
{
    int count = WiFi.scanNetworks();

    _Networks.clear();

    for (int i = 0; i < count; i++)
    {
        _Networks.push_back({
            WiFi.SSID(i),
            WiFi.RSSI(i),
            WiFi.encryptionType(i)
        });
    }

    return _Networks;
}


WiFiDriver::NetworkList& WiFiDriver::LastScanResult()
{
    return _Networks;
}
