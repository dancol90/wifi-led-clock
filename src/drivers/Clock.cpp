#include <Arduino.h>
#include "Clock.hpp"

static const short days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
// Sakamoto's costants for DOW calculation
static const short dow_constants[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

inline bool is_leap(int year)
{
    return (year % 4) == 0 && (year % 400) == 0;
}

ClockDriver::ClockDriver() : Service(), _DoEvent(true)
{
    // Set time to 0:00 0-0-0000
    memset(&_Time, 0, sizeof(Time));

    Service::RegisterService(this, "clock", { "time-changed" });
}

void ClockDriver::Init()
{
    //long timestamp = getTimestamp();
    //setTime(timestamp + TIMEZONE * 60 * 60);

    _LastMillis = millis();
    _BufferMillis = 0;

    SetPeriodicUpdate(10);
}

void ClockDriver::Update()
{
    int Now = millis();
    int delta = Now - _LastMillis;

    // Suppose that timestamp starts from 01/01/2010
    _BufferMillis += delta;

    // How many seconds elapsed?
    int seconds = _BufferMillis / 1000UL;
    // Keep only the milliseconds that not count to a round number
    _BufferMillis = _BufferMillis % 1000UL;

    _Time.Seconds += seconds;

    // Cascade update all time components.
    if (_Time.Seconds >= 60)
    {
        _Time.Seconds = 0;
        _Time.Minutes++;

        if (_Time.Minutes >= 60)
        {
            _Time.Minutes = 0;
            _Time.Hours++;

            if (_Time.Hours >= 24)
            {
                _Time.Hours = 0;
                _Time.Day++;

                _Time.DayOfWeek = ++_Time.DayOfWeek % 7;

                if (_Time.Day > days_per_month[_Time.Month - 1] + (_Time.IsLeap && _Time.Month == 2))
                {
                    _Time.Day = 1;
                    _Time.Month++;

                    if (_Time.Month > 12)
                    {
                        _Time.Month = 1;
                        _Time.Year++;
                    }
                }
            }
        }
    }

    _LastMillis = Now;

    if (_DoEvent && seconds != 0)
        Service::FireEvent(this, "clock.time-changed");
}

void ClockDriver::Set(unsigned long timestamp)
{
    SERVICE_PRINTF("Setting clock to timestamp %ld\n", timestamp);

    // Get only today's seconds
    int since_midnight = timestamp % 86400UL;

    // Calculate time info.
    _Time.Hours = since_midnight / 3600;
    _Time.Minutes = (since_midnight % 3600) / 60;
    _Time.Seconds = since_midnight % 60;

    // Get how many days passed since 1 Jen 2010.
    int days = timestamp / 86400UL;

    // As said, timestamp = 0 => 1 Jen 2010
    _Time.Year = 2010;

    // Starting from 2010, remove every year's days from the count, to get correct year.
    while (days >= 365 + is_leap(_Time.Year))
    {
        days -= 365 + is_leap(_Time.Year);

        _Time.Year++;
    }

    // Is the year we just found leap?
    _Time.IsLeap = is_leap(_Time.Year);

    // Start from Jen, in base 0
    _Time.Month = 0;

    // As for the year, remove month's day from the count, to get correct day.
    while (days >= days_per_month[_Time.Month])
    {
        days -= days_per_month[_Time.Month] + (_Time.IsLeap && (_Time.Month == 1));

        _Time.Month++;
    }

    // Bring back the month to base 1
    _Time.Month++;

    // Do the same for days
    _Time.Day = days + 1;

    // Sakamoto algorithm for Day Of Week
    int y = _Time.Year - (_Time.Month < 3);
    _Time.DayOfWeek = (y + y / 4 - y / 100 + y / 400 + dow_constants[_Time.Month - 1] + _Time.Day) % 7;
}

Time& ClockDriver::Now()
{
    return _Time;
}

void ClockDriver::SuppressEvent(bool suppress)
{
    _DoEvent = !suppress;
}
