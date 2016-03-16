#include "Registry.hpp"

RegistryService::RegistryService() : Service() {
    Service::register_service(this, "registry", {});
}

void RegistryService::init() {
    fs = S("fs", FileSystemDriver);
}
void RegistryService::update() {}

void RegistryService::set(String key, String value) {
    if (key)
        fs->write(String("/config/" + key), value);
}

template<> int RegistryService::from_string<int>(String s) { return s.toInt(); }
template<> String RegistryService::from_string<String>(String s) { return s; }
