#include <stdint.h>
#include <stddef.h>

#include <functional>

#ifndef TICKER_H
#define TICKER_H

extern "C" {
	typedef struct _ETSTIMER_ ETSTimer;
}

class Task {

public:

    typedef std::function<void()> TaskFunct;

	Task();
	~Task();

    void attach(uint32_t ms, TaskFunct task, bool repeat = true);
	void detach();

    void enable();
    void disable();

	TaskFunct funct;

private:

	ETSTimer* timer;

    unsigned long ms;
    bool repeat;
};


#endif//TICKER_H
