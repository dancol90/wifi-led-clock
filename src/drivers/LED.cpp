#include "LED.hpp"
#include "../resources/fonts.h"

LedMatrixDriver::LedMatrixDriver() : Service()
{
    Service::RegisterService(this, "display", {});
}

void LedMatrixDriver::Init()
{
    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_CLK, OUTPUT);
    pinMode(SPI_CS, OUTPUT);
    digitalWrite(SPI_CS, HIGH);

    writeCommand(OP_DISPLAYTEST, 0);
    // Scanlimit is set to max on startup
    writeCommand(OP_SCANLIMIT, 7);
    // Decode is done in source
    writeCommand(OP_DECODEMODE, 0);
    // Set the brightness
    writeCommand(OP_INTENSITY, 1);
    // Wake the MAX72XX devices (NOTE: OP_SHUTDOWN is active low)
    writeCommand(OP_SHUTDOWN, 1);

    // Write whole empty buffer
    ClearDisplay();
    Update();
}

void LedMatrixDriver::Update()
{
    int shift;

    // This routine is similar to sendCommand, but every devices receives different data with the same opcode.
    // It leverage the shift-register nature of the chip, writing a full row at a time.

    // Shift out date for every device, starting from the last one
    for (int row = 0; row < 8; row++)
    {
        // Enable the line
        digitalWrite(SPI_CS, LOW);
        
        for (int i = IC_COUNT - 1; i >= 0; i--)
        {
            // buffer[i] (long) structure:
            // 00000000  xxxxxxxx  xxxxxxxx  xxxxxxxx
            //  ignored  matrix 2  matrix 1  matrix 0
            shift = i << 3; // Faster way to say i * 8
            
            shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, row + 1);
            shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, _Buffer[i] >> shift);
        }

        // Latch the data onto the display
        digitalWrite(SPI_CS, HIGH);
    }
}

void LedMatrixDriver::ClearDisplay()
{
    for (int i = 0; i < 8; i++)
        _Buffer[i] = 0;
}

// #############################################################################

void LedMatrixDriver::writeCommand(volatile byte opcode, volatile byte data)
{
    // Enable the line
    digitalWrite(SPI_CS, LOW);
    
    for (int i = 0; i < IC_COUNT; i++)
    {
        shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, opcode);
        shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, data);
    }

    // Latch the data onto the display
    digitalWrite(SPI_CS, HIGH);
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
