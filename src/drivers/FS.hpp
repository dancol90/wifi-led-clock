#include <FS.h>
#undef max
#undef min

#include "../Service.hpp"

#ifndef FS_HPP
#define FS_HPP

class FileSystemDriver : public Service
{
public:
    FileSystemDriver();

    void Init();
    void Update();

    bool Exists(String path);

    File Open(String path, bool write = false, bool append = false);
    String Read(String path);
    void Write(String path, String content);
};

#endif
