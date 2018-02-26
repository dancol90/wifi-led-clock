#include "../Service.hpp"

#ifndef CLOCK_HPP
#define CLOCK_HPP

struct Time
{
    int Hours;
    int Minutes;
    int Seconds;

    int Day;
    int Month;
    int Year;

    int DayOfWeek;

    bool IsLeap;
};

class ClockDriver : public Service
{
public:

    ClockDriver();

    void Init();
    void Update();

    void Set(unsigned long timestamp);
    Time& Now();

    void SuppressEvent(bool suppress);

private:
    long _LastMillis, _BufferMillis;
    bool _DoEvent;
    Time _Time;
};

#endif
