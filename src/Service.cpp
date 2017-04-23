#include "Service.hpp"

Service::ServiceMap Service::services;
Service::EventMap   Service::events;

void Service::register_service(Service* service, const char* name, std::vector<const char*> events) {
    SERVICE_PRINTF("Registering service %s...", name);

    // If there's no service with the same name
    if (Service::services.count(name) == 0) {
        // Register the service
        Service::services[name] = service;

        // For each declared event
        for (const char* event : events) {
            // Create a new buffer every time because map does no copy the content, just the pointer
            char* event_name = new char[strlen(name) + strlen(event) + 2];
            sprintf(event_name, "%s.%s", name, event);

            // Initialize the callback list for the event
            Service::events[event_name] = std::vector<EventCallback>();
        }

        SERVICE_PRINT("OK");
    } else
        SERVICE_PRINT("ERROR: already exists");
}

void Service::init_all() {
    for (auto& s : Service::services) {
        SERVICE_PRINTF("Initializing service %s\n", s.first);
        s.second->init();
    }
}

void Service::bind_event(const char* event, EventCallback callback) {
    SERVICE_PRINTF("Binding event %s...", event);

    if (Service::events.count(event) == 1) {
        // TODO check if the callback is already registered
        Service::events[event].push_back(callback);

        SERVICE_PRINT("OK");
    } else {
        SERVICE_PRINT("ERROR: not found");
    }
}

void Service::fire_event(Service* service, const char* event) {
    if (Service::events.count(event)) {
        for (EventCallback cb : Service::events[event]) {
            cb(service);
        }
    }
}

Service* Service::get(const char* name) {
    return Service::services[name];
}

void Service::sync_update() {
    for (auto& s : Service::services) {
        if (s.second->update_request != NO_UPDATE) {
            //SERVICE_PRINTF("Syncronous updating service %s\n", s.first);
            s.second->update();

            if (s.second->update_request != ALWAYS_UPDATE)
                s.second->update_request = NO_UPDATE;
        }
    }
}

void Service::set_periodic_update(unsigned int period, UpdateType ut) {
    if (ut == UPDATE_SYNC)
        if (period == 0)
            this->update_request = ALWAYS_UPDATE;
        else
            this->update_task.attach(period, [this]() {
                this->update_request = DO_UPDATE;
            });
    else
        this->update_task.attach(period, std::bind(&Service::update, this));
}

void Service::enable_periodic_update() {
    this->update_task.enable();
}

void Service::disable_periodic_update() {
    this->update_task.disable();
}
