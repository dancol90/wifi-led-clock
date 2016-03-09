#include <LedControl.h>

#include "../Service.hpp"

#ifndef LED_HPP
#define LED_HPP

class LedMatrixDriver : public Service {
public:

    LedMatrixDriver();

    void init();
    void update();

    void clear_buffer();
    void clear_display();

    long* get_buffer() { return buffer; }

    void draw_digit_big(uint8_t x, uint8_t digit);
    void draw_digit_small(uint8_t x, uint8_t y, uint8_t digit);

private:
    static const int IC_COUNT = 3;

    LedControl lc;
    // 8 rows of 8*3 pixel each = 24*8 display
    long buffer[8];
};

#endif
