#include "Scroll.hpp"
#include "../resources/fonts.h"

ScrollService::ScrollService()
{
    // Set the update interval once for all
    SetPeriodicUpdate(SCROLL_SPEED);
    DisablePeriodicUpdate();

    Service::RegisterService(
        this, "scroll",
        { "start", "end", "step" }
    );
}

void ScrollService::Init()
{
    _display = S("display", LedMatrixDriver);
    _Clock = S("clock", ClockDriver);

    _scrolling = false;
}

void ScrollService::Update()
{
    auto* buffer = _display->GetBuffer();

    // If the current character has been completely showed, load a new one.
    if (_column_left == 0)
    {
        // Get the char entire data (5 row, each one is a byte)
        for (int r = 0; r < 5; r++)
        {
            if (_step != 0 && _step <= _text_len * 5)
            {
                uint8_t c = _text[(_step - 1) / 5];
                _glyph[r] = pgm_read_byte(font5x5 + (c - 32) * 5 + r);
            }
            else
            {
                _glyph[r] = 0x00;
            }
        }

        // Reset the counter
        _column_left = 5;
    }

    // For each row of the screen
    for (int i = 0; i < 8; i++)
    {
        // Move the row to the left
        buffer[i] = buffer[i] << 1;

        // Do this only if this row is one in which the char goes.
        if (i >= 2 && i < 7)
            buffer[i] += _glyph[i - 2] >> (_column_left + 2) & 0x01;
    }

    _column_left--;
    _step++;

    _display->Update();

    // Do scroll ended? e.g, have we moved everything?
    if (_step == 5 + _text_len * 5 + 24)
        stop();
}

void ScrollService::start()
{
    _scrolling = true;
    _text_len = strlen(_text);

    // Reset the step count
    _step = 0;
    _column_left = 0;

    _Clock->SuppressEvent(true);
    EnablePeriodicUpdate();
}

void ScrollService::stop()
{
    _scrolling = false;
    DisablePeriodicUpdate();
    _Clock->SuppressEvent(false);
}

void ScrollService::message(String format, ...)
{
    va_list argptr;
    // Get parameter list
    va_start(argptr, format);
    // Create string and save it to this->text
    vsnprintf(_text, 250, format.c_str(), argptr);
    // Cleanup
    va_end(argptr);

    start();
}
