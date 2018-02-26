#include "../Service.hpp"
#include "../drivers/FS.hpp"

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

class RegistryService : public Service
{
public:
    RegistryService();

    void Init();
    void Update();

    template<typename T>
    T Get(String key, T default_val = "")
    {
        String content;
        GetContent(key, content);

        return content.length() > 0 ? from_string<T>(content) : default_val;
    }

    void Set(String key, String value);

private:
    FileSystemDriver* _fs;

    // TODO adding inline would be great, but gives some warnings, even if it works.
    template<typename T> T from_string(String s);

    bool GetContent(String& key, String& content);
};

#endif
