#include "../Service.hpp"
#include "../drivers/Clock.hpp"
#include "../drivers/LED.hpp"
#include "../services/Scroll.hpp"

#ifndef CLOCKAPPLICATION_HPP
#define CLOCKAPPLICATION_HPP

class ClockApplication : public Service {
public:
    ClockApplication();

    void Init();
    void Update();

private:
    ClockDriver* _Clock;
    LedMatrixDriver* _Led;
    ScrollService* _Scroll;
};

#endif
