#include "Registry.hpp"

RegistryService::RegistryService() : Service()
{
    Service::RegisterService(this, "registry", {});
}

void RegistryService::Init()
{
    _fs = S("fs", FileSystemDriver);
}
void RegistryService::Update() {}

void RegistryService::Set(String key, String value)
{
    if (key)
        _fs->Write(String("/config/" + key), value);
}

bool RegistryService::GetContent(String& key, String& content)
{
    SERVICE_PRINTF("Opening config file %s...", key.c_str());

    String file = String("/config/") + key;

    if (_fs->Exists(file))
    {
        content = _fs->Read(file);
        SERVICES_PRINTF("content: %s\n", content.c_str());
        return true;
    }
    else
    {
        SERVICES_PRINT("error\n");
        return false;
    }
}

template<> int RegistryService::from_string<int>(String s) { return s.toInt(); }
template<> String RegistryService::from_string<String>(String s) { return s; }
