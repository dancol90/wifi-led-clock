#include "LED.hpp"
#include "../resources/fonts.h"

LedMatrixDriver::LedMatrixDriver() : Service(), _LedControl(14, 4, 5, LedMatrixDriver::IC_COUNT)
{
    Service::RegisterService(this, "display", {});
}

void LedMatrixDriver::Init()
{
    for (int i = 0; i < IC_COUNT; i++)
    {
        // The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
        _LedControl.shutdown(i, false);
        // Set the brightness
        _LedControl.setIntensity(i, 8);
        // and clear the display
        _LedControl.clearDisplay(i);
    }

    ClearBuffer();
}

void LedMatrixDriver::Update()
{
    int shift;

    for (int d = 0; d < IC_COUNT; d++)
    {
        // buffer[i] (long) structure:
        // 00000000  xxxxxxxx  xxxxxxxx  xxxxxxxx
        //  ignored  matrix 0  matrix 1  matrix 2
        shift = 8 * d;//(2 - d);

        for (int i = 0; i < 8; i++)
            _LedControl.setRow(d, i, _Buffer[i] >> shift);
    }
}

void LedMatrixDriver::ClearBuffer()
{
    for (int i = 0; i < 8; i++)
    {
        _Buffer[i] = 0;
    }
}

void LedMatrixDriver::ClearDisplay()
{
    for (int i = 0; i < IC_COUNT; i++)
    {
        // clear the display
        _LedControl.clearDisplay(i);
    }
}

// #############################################################################

void LedMatrixDriver::DrawDigitBig(uint8_t x, uint8_t digit)
{
    uint8_t offset = 8 * digit;

    long tmp, mask;

    for (int j = 0; j < 8; j++)
    {
        // Allineo la riga del carattere (formato xxxx0000) al bordo sinistro
        tmp = (long)pgm_read_byte(font_numbers + offset + j) << 16;

        mask = ((long)B11110000 << 16) >> x;

        _Buffer[j] = (_Buffer[j] & ~mask) + ((tmp >> x) & mask);
    }
}


void LedMatrixDriver::DrawDigitSmall(uint8_t x, uint8_t y, uint8_t digit)
{
    uint8_t offset = 5 * digit;

    long tmp, mask;

    uint8_t count = y + 5;

    if (count > 8) count = 8;

    for (uint8_t j = y; j < count; j++)
    {
        // Allineo la riga del carattere (formato xxx00000) al bordo sinistro
        tmp = (long)pgm_read_byte(small_numbers + offset + j - y) << 16;

        mask = ((long)B11100000 << 16) >> x;

        _Buffer[j] = (_Buffer[j] & ~mask) + ((tmp >> x) & mask);
    }
}
