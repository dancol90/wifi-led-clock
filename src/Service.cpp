#include "Service.hpp"

Service::ServiceMap Service::_Services;
Service::EventMap   Service::_Events;

void Service::RegisterService(Service* service, const char* name, std::vector<const char*> events)
{
    SERVICES_PRINTF("Registering service %s...", name);

    // If there's no service with the same name
    if (Service::_Services.count(name) == 0)
    {
        // Register the service
        Service::_Services[name] = service;

        // For each declared event
        for (const char* event : events)
        {
            // Create a new buffer every time because map does no copy the content, just the pointer
            char* event_name = new char[strlen(name) + strlen(event) + 2];
            sprintf(event_name, "%s.%s", name, event);

            // Initialize the callback list for the event
            Service::_Events[event_name] = std::vector<EventCallback>();
        }

        service->_ServiceName = name;

        SERVICES_PRINT("OK\n");
    }
    else
        SERVICES_PRINT("ERROR: already exists\n");
}

void Service::InitAll()
{
    for (auto& s : Service::_Services)
    {
        SERVICES_PRINTF("Initializing service %s\n", s.first);
        s.second->Init();
    }
}

void Service::BindEvent(const char* event, EventCallback callback)
{
    SERVICES_PRINTF("Binding event %s...", event);

    if (Service::_Events.count(event) == 1)
    {
        // TODO check if the callback is already registered
        Service::_Events[event].push_back(callback);

        SERVICES_PRINT("OK\n");
    }
    else
    {
        SERVICES_PRINT("ERROR: not found\n");
    }
}

void Service::FireEvent(Service* service, const char* event)
{
    if (Service::_Events.count(event))
    {
        for (EventCallback cb : Service::_Events[event])
        {
            cb(service);
        }
    }
}

Service* Service::Get(const char* name)
{
    return Service::_Services[name];
}

void Service::SyncUpdate()
{
    for (auto& s : Service::_Services)
    {
        if (s.second->_UpdateRequest != NO_UPDATE)
        {
            //SERVICE_PRINTF("Syncronous updating service %s\n", s.first);
            s.second->Update();

            if (s.second->_UpdateRequest != ALWAYS_UPDATE)
                s.second->_UpdateRequest = NO_UPDATE;
        }
    }
}

void Service::SuspendAsyncUpdates()
{
    for (auto& s : Service::_Services)
        s.second->DisablePeriodicUpdate();
}

void Service::SetPeriodicUpdate(unsigned int period, UpdateType ut)
{
    if (ut == UPDATE_SYNC)
        if (period == 0)
            this->_UpdateRequest = ALWAYS_UPDATE;
        else
            this->_UpdateTask.Attach(period, [this]()
        {
            this->_UpdateRequest = DO_UPDATE;
        });
    else
        this->_UpdateTask.Attach(period, std::bind(&Service::Update, this));
}

void Service::EnablePeriodicUpdate()
{
    this->_UpdateTask.Enable();
}

void Service::DisablePeriodicUpdate()
{
    this->_UpdateTask.Disable();
}

