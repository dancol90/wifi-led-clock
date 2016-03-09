#include "../Service.hpp"

#ifndef CLOCK_HPP
#define CLOCK_HPP

struct Time {
    int hour;
    int minute;
    int second;

    int day;
    int month;
    int year;

    int dow;

    bool leap;
};

class ClockDriver : public Service {
public:

    ClockDriver();

    void init();
    void update();

    void set(long timestamp);
    Time& now();

    void suppress_event(bool suppress);

private:
    long last_millis, buffer_millis;
    bool do_event;
    Time time;
};

#endif
