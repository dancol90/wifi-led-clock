#include "LED.hpp"
#include "../resources/fonts.h"

LedMatrixDriver::LedMatrixDriver() : Service(), lc(13, 14, 15, LedMatrixDriver::IC_COUNT) {
    Service::register_service(this, "display", {});
}

void LedMatrixDriver::init() {
    for(int i = 0; i < IC_COUNT; i++) {
        // The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
        lc.shutdown(i, false);
        // Set the brightness
        lc.setIntensity(i, 15);
        // and clear the display
        lc.clearDisplay(i);
    }

    clear_buffer();
}

void LedMatrixDriver::update() {
    int shift;

    for(int d = 0; d < IC_COUNT; d++) {
        // buffer[i] (long) structure:
        // 00000000  xxxxxxxx  xxxxxxxx  xxxxxxxx
        //  ignored  matrix 0  matrix 1  matrix 2
        shift = 8 * d;//(2 - d);

        for(int i = 0; i < 8; i++)
            lc.setRow(d, i, buffer[i] >> shift);
    }
}

void LedMatrixDriver::clear_buffer() {
    for(int i = 0; i < 8; i++) {
        buffer[i] = 0;
    }
}

void LedMatrixDriver::clear_display() {
    for(int i = 0; i < IC_COUNT; i++) {
        // clear the display
        lc.clearDisplay(i);
    }
}

// #############################################################################

void LedMatrixDriver::draw_digit_big(uint8_t x, uint8_t digit) {
    uint8_t offset = 8 * digit;

    long tmp, mask;

    for(int j = 0; j < 8; j++) {
        // Allineo la riga del carattere (formato xxxx0000) al bordo sinistro
        tmp = (long)font_numbers[offset + j] << 16;

        mask = ((long)B11110000 << 16) >> x;

        buffer[j] = (buffer[j] & ~mask) + ( (tmp >> x) & mask );
    }
}


void LedMatrixDriver::draw_digit_small(uint8_t x, uint8_t y, uint8_t digit) {
    uint8_t offset = 5 * digit;

    long tmp, mask;

    uint8_t count = y + 5;

    if(count > 8) count = 8;

    for(uint8_t j = y; j < count; j++) {
        // Allineo la riga del carattere (formato xxx00000) al bordo sinistro
        tmp = (long)small_numbers[offset + j - y] << 16;

        mask = ((long)B11100000 << 16) >> x;

        buffer[j] = (buffer[j] & ~mask) + ( (tmp >> x) & mask );
    }
}
