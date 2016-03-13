#include "../Service.hpp"
#include "../drivers/FS.hpp"

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

class RegistryService : public Service {
public:
    RegistryService();

    void init();
    void update();

    String get(String key, String default_val = "");

    void set(String key, String value);
    //void set(String key, int value);
    //void set(String key, long value);

private:
    FileSystemDriver* fs;
};

#endif
