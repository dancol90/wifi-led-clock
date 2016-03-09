#include <Arduino.h>
#include "Task.hpp"

#include <map>
#include <vector>
#include <functional>

#ifndef SERVICE_HPP
#define SERVICE_HPP

#ifdef DEBUG
    #define SERVICE_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__);
#else
    #define SERVICE_PRINTF(...)
#endif

#define S(name, type) ((type*)Service::get(name))

struct char_compare {
    bool operator () (const char *a, const char *b) const { return strcmp(a,b) < 0; }
};

class Service {
public:

    Service() {};

    virtual void init() = 0;
    virtual void update() = 0;

    typedef std::function<void(Service* svc)> EventCallback;

    typedef std::map<const char*, Service*, char_compare> ServiceMap;
    typedef std::map<const char*, std::vector<EventCallback>, char_compare> EventMap;

    static Service* get(const char* name);
    static void bind_event(const char* event, EventCallback callback);

    static void init_all();

protected:
    static void register_service(Service* service, const char* name,
                                 std::vector<const char*> events);
    static void fire_event(Service* service, const char* event);

    void set_periodic_update(unsigned int period);
    void enable_periodic_update();
    void disable_periodic_update();

private:
    Task update_task;

    // List of service (name,instance) pairs.
    static ServiceMap services;
    // Map every event name (declared by services in register_service) with one or more callbacks.
    static EventMap events;
};

#endif
