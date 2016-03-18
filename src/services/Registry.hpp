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

        String file = String("/config/") + key;

        if (fs->exists(file)) {
            String s = fs->read(file);

            SERVICE_PRINTF("content: %s\n", s.c_str());

            return s.length() > 0 ? from_string<T>(s) : default_val;
        } else {
            SERVICE_PRINT("no file found");
            return default_val;
        }
    }

    void set(String key, String value);

private:
    FileSystemDriver* fs;

    // TODO adding inline would be great, but gives some warnings, even if it works.
    template<typename T> T from_string(String s);
};

#endif
