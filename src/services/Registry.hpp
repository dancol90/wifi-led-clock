#include "../Service.hpp"
#include "../drivers/FS.hpp"

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

class RegistryService : public Service {
public:
    RegistryService();

    void init();
    void update();

    template<typename T>
    T get(String key, T default_val = "") {
        SERVICE_PRINTF("Opening config file %s...", key.c_str());

        File f = fs->open(String("/config/") + key);

        if (!f) {
            Serial.println("no file found");
            return default_val;
        }

        String s = f.readString();
        SERVICE_PRINTF("content: %s\n", s.c_str());
        f.close();

        return from_string<T>(s);
    }

    void set(String key, String value);
    //void set(String key, int value);
    //void set(String key, long value);

private:
    FileSystemDriver* fs;

    // TODO adding inline would be great, but gives some warnings, even if it works.
    template<typename T> T from_string(String s);
};

#endif
