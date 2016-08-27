#include <FS.h>
#undef max
#undef min

#include "../Service.hpp"

#ifndef FS_HPP
#define FS_HPP

class FileSystemDriver : public Service {
public:

    FileSystemDriver();

    void init();
    void update();

    bool exists(String path);

    File open(String path, bool write = false, bool append = false);
    String read(String path);
    void write(String path, String content);
};

#endif
