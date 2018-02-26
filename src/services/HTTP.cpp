#include "HTTP.hpp"

const char net_entry[] PROGMEM = "{\"ssid\": \"_1\",\"rssi\": _2,\"enc\": _3}";

HTTPService::HTTPService() : _server(80)
{
    Service::RegisterService(
        this, "http", { }
    );
}

void HTTPService::Init()
{
    _wifi = S("wifi", WiFiDriver);
    _registry = S("registry", RegistryService);
    _Scroll = S("scroll", ScrollService);

    _server.on("/wifi", HTTP_GET, [=]()
    {
        if (!Authenticate())
            return _server.requestAuthentication();

        auto& networks = _wifi->Scan();

        String json = "[";

        for (auto& net : networks)
        {
            String entry(FPSTR(net_entry));

            entry.replace("_1", net.SSID);
            entry.replace("_2", String(net.RSSI));
            entry.replace("_3", String(net.encryption));

            json += entry + ',';
        }

        json += ']';

        json.replace(",]", "]");

        _server.send(200, "text/json", json);
    });
    _server.on("/wifi", HTTP_POST, [=]()
    {
        if (!Authenticate())
            return _server.requestAuthentication();

        // TODO do some checks on arguments (maybe sanitize!!)
        String ssid = _server.arg("ssid");
        String pkey = _server.arg("pkey");

        // Send output first
        _server.send(200, "text/json", F("\"OK\""));

        // then connect
        _wifi->ConnectTo(ssid, pkey);
    });

    _server.on("/message", HTTP_POST, [=]()
    {
        if (!Authenticate())
            return _server.requestAuthentication();

        // TODO handle buffer overflows with long strings
        //      (unless the server lib already handles it)
        String message = _server.arg("m");

        _server.send(200, "text/json", F("\"OK\""));

        if (message.length())
            _Scroll->message(message);
    });

    _server.onNotFound([=]()
    {
        _server.send(404, "text/plain", F("Not found"));
    });

    _server.begin();

    SetPeriodicUpdate(0, UPDATE_SYNC);
}

void HTTPService::Update()
{
    _server.handleClient();
}

bool HTTPService::Authenticate()
{
    String user = _registry->Get("http_username", String());
    String pass = _registry->Get("http_password", String());

    return _server.authenticate(user.c_str(), pass.c_str());
}
