#include <stdint.h>
#include <stddef.h>

#include <functional>

#ifndef TICKER_H
#define TICKER_H

extern "C" {
    typedef struct _ETSTIMER_ ETSTimer;
}

class Task
{

public:

    typedef std::function<void()> TaskFunct;

    Task();
    ~Task();

    void Attach(uint32_t ms, TaskFunct task, bool repeat = true);
    void Detach();

    void Enable();
    void Disable();

    TaskFunct funct;

private:

    ETSTimer* _timer;

    unsigned long _ms;
    bool _repeat;
};


#endif//TICKER_H
