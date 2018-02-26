#include "FS.hpp"

FileSystemDriver::FileSystemDriver() : Service()
{
    Service::RegisterService(this, "fs", {});
}

void FileSystemDriver::Init()
{
    SPIFFS.begin();
}

void FileSystemDriver::Update() {}

bool FileSystemDriver::Exists(String path)
{
    return SPIFFS.exists(path);
}

File FileSystemDriver::Open(String path, bool write, bool append)
{
    String mode = write ? (append ? "a" : "w") : "r";

    return SPIFFS.open(path, mode.c_str());
}

String FileSystemDriver::Read(String path)
{
    File f = Open(path);
    String s;

    if (f)
    {
        s = f.readString();
        f.close();
    }

    return s;
}

void FileSystemDriver::Write(String path, String content)
{
    File f = Open(path, true);

    if (f)
    {
        f.print(content);
        f.close();
    }
}
