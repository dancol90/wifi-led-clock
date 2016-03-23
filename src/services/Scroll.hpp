#include "../Service.hpp"
#include "../drivers/LED.hpp"
#include "../drivers/Clock.hpp"

#ifndef SCROLL_HPP
#define SCROLL_HPP

class ScrollService : public Service {
public:
    static const unsigned int SCROLL_SPEED = 75;

    ScrollService();

    void init();
    void update();

    void start();
    void stop();

    bool is_scrolling() { return scrolling; }

    char* get_buffer() { return text; }
    void  message(String format, ...);

private:
    LedMatrixDriver* display;
    ClockDriver* clock;

    unsigned int step;

    bool    scrolling;
    char    text[250];
    uint8_t text_len, position, column_left;
    uint8_t glyph[8];
};

#endif
