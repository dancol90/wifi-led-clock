#include <Arduino.h>
#include "Task.hpp"

#include <map>
#include <vector>
#include <functional>

#ifndef SERVICE_HPP
#define SERVICE_HPP

#ifdef _DEBUG
// This macros works only with string literals, and cannot be used in headers files.
#define SERVICES_PRINTF(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)
#define SERVICES_PRINT(text)         SERVICES_PRINTF(text)

#define SERVICE_PRINTF(format, ...)  Serial.printf_P(PSTR("[%s] " format), _ServiceName.c_str(), ##__VA_ARGS__)
#define SERVICE_PRINT(text)          SERVICE_PRINTF(text)
#else
#define SERVICES_PRINT(text)
#define SERVICES_PRINTF(...)

#define SERVICE_PRINTF(...)
#define SERVICE_PRINT(text)
#endif

#define S(name, type) ((type*)Service::Get(name))

struct char_compare
{
    bool operator () (const char *a, const char *b) const { return strcmp(a, b) < 0; }
};

enum UpdateType
{
    UPDATE_SYNC,
    UPDATE_ASYNC
};

enum UpdateRequest
{
    NO_UPDATE,
    DO_UPDATE,
    ALWAYS_UPDATE
};

class Service
{
public:
    typedef std::function<void(Service* svc)> EventCallback;

    typedef std::map<const char*, Service*, char_compare> ServiceMap;
    typedef std::map<const char*, std::vector<EventCallback>, char_compare> EventMap;

    Service() {};

    virtual void Init() = 0;
    virtual void Update() = 0;

    static Service* Get(const char* name);
    static void BindEvent(const char* event, EventCallback callback);

    static void InitAll();
    static void SyncUpdate();

    static void SuspendAsyncUpdates();

protected:
    static void RegisterService(Service* service, const char* name, std::vector<const char*> events);
    static void FireEvent(Service* service, const char* event);

    void SetPeriodicUpdate(unsigned int period, UpdateType ut = UPDATE_ASYNC);
    void EnablePeriodicUpdate();
    void DisablePeriodicUpdate();

    UpdateRequest _UpdateRequest;
    String _ServiceName;
private:
    Task _UpdateTask;

    // List of service (name,instance) pairs.
    static ServiceMap _Services;
    // Map every event name (declared by services in register_service) with one or more callbacks.
    static EventMap _Events;
};

#endif
