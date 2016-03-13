#include "Registry.hpp"

RegistryService::RegistryService() : Service() {
    Service::register_service(this, "registry", {});
}

void RegistryService::init() {
    fs = S("fs", FileSystemDriver);
}
void RegistryService::update() {}

String RegistryService::get(String key, String default_val) {
    SERVICE_PRINTF("Opening config file %s...", key.c_str());

    //File f = fs->open(String("/config/") + key);
    File f = SPIFFS.open(String("/config/") + key, "r");

    if (!f) {
        Serial.println("no file found");
        return default_val;
    }

    String s = f.readString();
    SERVICE_PRINTF("content: %s\n", s.c_str());
    f.close();

    return s;
}

void RegistryService::set(String key, String value) {
    if (key)
        fs->write(String("/config/" + key), value);
}
