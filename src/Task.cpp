#include <Arduino.h>
#include "Task.hpp"

extern "C"
{
    #include "c_types.h"
    #include "eagle_soc.h"
    #include "ets_sys.h"
    #include "osapi.h"
}

void task_callback(Task* t)
{
    t->funct();
}

Task::Task() : _timer(0) {}

Task::~Task()
{
    Detach();
}

void Task::Attach(uint32_t ms, TaskFunct task, bool repeat)
{
    // First detach if needed
    Detach();

    this->_timer = new ETSTimer;
    this->_ms = ms;
    this->_repeat = repeat;
    this->funct = task;

    // Create a new timer, passing functor as parameter
    os_timer_setfn(
        _timer,
        reinterpret_cast<ETSTimerFunc*>(task_callback),
        reinterpret_cast<void*>(this)
    );
    Enable();
}

void Task::Detach()
{
    if (!_timer)
        return;

    Disable();
    delete _timer;
    _timer = 0;
}

void Task::Enable()
{
    if (_timer)
        // Arm the timer
        os_timer_arm(_timer, _ms, _repeat);
}

void Task::Disable()
{
    if (_timer)
        os_timer_disarm(_timer);
}
