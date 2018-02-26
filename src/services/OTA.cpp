#include "OTA.hpp"

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


OTAService::OTAService()
{
    Service::RegisterService(this, "ota", { });
}

void OTAService::Init()
{
    _wifi = S("wifi", WiFiDriver);
    _registry = S("registry", RegistryService);
    _Led = S("display", LedMatrixDriver);

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword((const char *)"123");

    Service::BindEvent("wifi.connected", [=](Service* s) { OnWifiConnected(); });
    SetPeriodicUpdate(0, UPDATE_SYNC);
}

void OTAService::Update()
{
    ArduinoOTA.handle();
}

void OTAService::OnWifiConnected()
{
    SERVICE_PRINT("Setting up OTA\n");

    ArduinoOTA.onStart([=]()
    {
        SERVICE_PRINT("Start\n");
        Service::SuspendAsyncUpdates();
    });

    ArduinoOTA.onEnd([=]()
    {
        SERVICE_PRINT("End\n");
        //ESP.restart();
    });

    ArduinoOTA.onProgress([=](unsigned int progress, unsigned int total)
    {
        int percent = 100 * progress / total;
        SERVICE_PRINTF("Progress: %u%%\n", percent);

        _Led->ClearBuffer();

        _Led->DrawDigitBig(6, percent / 100);
        _Led->DrawDigitBig(10, (percent % 100) / 10);
        _Led->DrawDigitBig(14, percent % 10);

        _Led->Update();

    });

    ArduinoOTA.onError([=](ota_error_t error)
    {
        SERVICE_PRINTF("Error[%u]: ", error);

        if (error == OTA_AUTH_ERROR) SERVICE_PRINT("Auth Failed\n");
        else if (error == OTA_BEGIN_ERROR) SERVICE_PRINT("Begin Failed\n");
        else if (error == OTA_CONNECT_ERROR) SERVICE_PRINT("Connect Failed\n");
        else if (error == OTA_RECEIVE_ERROR) SERVICE_PRINT("Receive Failed\n");
        else if (error == OTA_END_ERROR) SERVICE_PRINT("End Failed\n");
    });

    ArduinoOTA.begin();
}