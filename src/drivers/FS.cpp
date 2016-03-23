#include "FS.hpp"

FileSystemDriver::FileSystemDriver() : Service() {
    Service::register_service(this, "fs", {});
}

void FileSystemDriver::init() {
    SPIFFS.begin();
}

void FileSystemDriver::update() {}

bool FileSystemDriver::exists(String path) {
    return SPIFFS.exists(path);
}

File FileSystemDriver::open(String path, bool write, bool append) {
    String mode = write ? (append ? "a" : "w") : "r";

    return SPIFFS.open(path, mode.c_str());
}

String FileSystemDriver::read(String path) {
    File f = open(path);
    String s;

    if (f) {
        s = f.readString();
        f.close();
    }

    return s;
}

void FileSystemDriver::write(String path, String content) {
    File f = open(path, true);

    if (f) {
        f.print(content);
        f.close();
    }
}
