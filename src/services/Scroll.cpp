#include "Scroll.hpp"
#include "../resources/fonts.h"

ScrollService::ScrollService() {
    // Set the update interval once for all
    set_periodic_update(SCROLL_SPEED);
    disable_periodic_update();

    Service::register_service(
        this, "scroll",
        { "start", "end", "step" }
    );
}

void ScrollService::init() {
    display = S("display", LedMatrixDriver);
    clock   = S("clock",   ClockDriver);

    scrolling = false;
}

void ScrollService::update() {
    auto* buffer = display->get_buffer();

    // If the current character has been completely showed, load a new one.
    if (column_left == 0) {
        // Get the char entire data (5 row, each one is a byte)
        for (int r = 0; r < 5; r++) {
            if (step != 0 && step <= text_len * 5) {
                uint8_t c = text[(step - 1) / 5];
                glyph[r] = font5x5[(c - 32) * 5 + r];
            } else {
                glyph[r] = 0x00;
            }
        }

        // Reset the counter
        column_left = 5;
    }

    // For each row of the screen
    for (int i = 0; i < 8; i++) {
        // Move the row to the left
        buffer[i] = buffer[i] << 1;

        // Do this only if this row is one in which the char goes.
        if(i >= 2 && i < 7)
            buffer[i] += glyph[i - 2] >> (column_left + 2) & 0x01;
    }

    column_left--;
    step++;

    display->update();

    // Do scroll ended? e.g, have we moved everything?
    if (step == 5 + text_len * 5 + 24)
        stop();
}

void ScrollService::start() {
    scrolling = true;
    text_len = strlen(text);

    // Reset the step count
    step = 0;
    column_left = 0;

    clock->suppress_event(true);
    enable_periodic_update();
}

void ScrollService::stop() {
    scrolling = false;
    disable_periodic_update();
    clock->suppress_event(false);
}

void ScrollService::message(String format, ...) {
    va_list argptr;
    // Get parameter list
    va_start(argptr, format);
    // Create string and save it to this->text
    vsnprintf(text, 250, format.c_str(), argptr);
    // Cleanup
    va_end(argptr);

    start();
}
