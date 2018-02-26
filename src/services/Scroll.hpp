#include "../Service.hpp"
#include "../drivers/LED.hpp"
#include "../drivers/Clock.hpp"

#ifndef SCROLL_HPP
#define SCROLL_HPP

class ScrollService : public Service
{
public:
    static const unsigned int SCROLL_SPEED = 75;

    ScrollService();

    void Init();
    void Update();

    void start();
    void stop();

    bool is_scrolling() { return _scrolling; }

    char* get_buffer() { return _text; }
    void  message(String format, ...);

private:
    LedMatrixDriver* _display;
    ClockDriver* _Clock;

    unsigned int _step;

    bool    _scrolling;
    char    _text[250];
    uint8_t _text_len, _position, _column_left;
    uint8_t _glyph[8];
};

#endif
