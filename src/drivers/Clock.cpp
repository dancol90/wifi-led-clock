#include <Arduino.h>
#include "Clock.hpp"

static const short days_per_month[]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// Sakamoto's costants for DOW calculation
static const short dow_constants[]   = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

inline bool is_leap(int year) {
    return (year % 4)   == 0 &&
           (year % 100) != 0 ||
           (year % 400) == 0;
}

ClockDriver::ClockDriver() : Service(), do_event(true) {
    // Set time to 0:00 0-0-0000
    memset(&time, 0, sizeof(Time));

    Service::register_service(this, "clock", { "time-changed" });
}

void ClockDriver::init() {
    //long timestamp = getTimestamp();
    //setTime(timestamp + TIMEZONE * 60 * 60);

    last_millis = millis();
    buffer_millis = 0;

    set_periodic_update(10);
}

void ClockDriver::update() {
    int now = millis();
    int delta = now - last_millis;

    // Suppose that timestamp starts from 01/01/2010
    buffer_millis += delta;

    // How many seconds elapsed?
    int seconds   = buffer_millis / 1000UL;
    // Keep only the milliseconds that not count to a round number
    buffer_millis = buffer_millis % 1000UL;

    time.second += seconds;

    // Cascade update all time components.
    if (time.second >= 60) {
        time.second = 0;
        time.minute++;

        if (time.minute >= 60) {
            time.minute = 0;
            time.hour++;

            if (time.hour >= 24) {
                time.hour = 0;
                time.day++;

                time.dow = ++time.dow % 7;

                if (time.day > days_per_month[time.month - 1] + (time.leap && time.month == 2)) {
                    time.day = 1;
                    time.month++;

                    if (time.month > 12) {
                        time.month = 1;
                        time.year++;
                    }
                }
            }
        }
    }

    last_millis = now;

    if (do_event && seconds != 0)
        Service::fire_event(this, "clock.time-changed");
}

void ClockDriver::set(unsigned long timestamp) {
    SERVICE_PRINTF("Setting clock to timestamp %ld\n", timestamp);

    // Get only today's seconds
    int since_midnight = timestamp % 86400UL;

    // Calculate time info.
    time.hour   =  since_midnight / 3600;
    time.minute = (since_midnight % 3600) / 60;
    time.second =  since_midnight % 60;

    // Get how many days passed since 1 Jen 2010.
    int days = timestamp / 86400UL;

    // As said, timestamp = 0 => 1 Jen 2010
    time.year = 2010;

    // Starting from 2010, remove every year's days from the count, to get correct year.
    while (days >= 365 + is_leap(time.year)) {
        days -= 365 + is_leap(time.year);

        time.year++;
    }

    // Is the year we just found leap?
    time.leap = is_leap(time.year);

    // Start from Jen, in base 0
    time.month = 0;

    // As for the year, remove month's day from the count, to get correct day.
    while (days >= days_per_month[time.month]) {
        days -= days_per_month[time.month] + (time.leap && (time.month == 1));

        time.month++;
    }

    // Bring back the month to base 1
    time.month++;

    // Do the same for days
    time.day = days + 1;

    // Sakamoto algorithm for Day Of Week
    int y = time.year - (time.month < 3);
    time.dow = (y + y/4 - y/100 + y/400 + dow_constants[time.month-1] + time.day) % 7;
}

Time& ClockDriver::now() {
    return time;
}

void ClockDriver::suppress_event(bool suppress) {
    do_event = !suppress;
}
