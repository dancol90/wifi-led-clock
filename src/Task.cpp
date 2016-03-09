#include <Arduino.h>
#include "Task.hpp"

extern "C" {
    #include "c_types.h"
    #include "eagle_soc.h"
    #include "ets_sys.h"
    #include "osapi.h"
}

void task_callback(Task* t) {
    t->funct();
}

Task::Task() : timer(0) {}

Task::~Task() {
	detach();
}

void Task::attach(uint32_t ms, TaskFunct task, bool repeat) {
    // First detach if needed
	detach();

    this->timer = new ETSTimer;
    this->ms = ms;
    this->repeat = repeat;
    this->funct = task;

    // Create a new timer, passing functor as parameter
	os_timer_setfn(
        timer,
        reinterpret_cast<ETSTimerFunc*>(task_callback),
        reinterpret_cast<void*>(this)
    );
    enable();
}

void Task::detach() {
	if (!timer)
		return;

    disable();
	delete timer;
	timer = 0;
}

void Task::enable() {
    // Arm the timer
	os_timer_arm(timer, ms, repeat);
}

void Task::disable() {
    os_timer_disarm(timer);
}
