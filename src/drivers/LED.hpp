#include "../Service.hpp"

#ifndef LED_HPP
#define LED_HPP

class LedMatrixDriver : public Service
{
public:

    LedMatrixDriver();

    void Init();
    void Update();

    void ClearDisplay();

    long* GetBuffer() { return _Buffer; }

    void DrawDigitBig(uint8_t x, uint8_t digit);
    void DrawDigitSmall(uint8_t x, uint8_t y, uint8_t digit);

private:
    static const int IC_COUNT = 3;
    
    // MAX72xx registers
    static const int OP_NOOP = 0;
    static const int OP_DECODEMODE = 9;
    static const int OP_INTENSITY = 10;
    static const int OP_SCANLIMIT = 11;
    static const int OP_SHUTDOWN = 12;
    static const int OP_DISPLAYTEST = 15;

    // SPI pin used to shift out data
    static const int SPI_MOSI = 14;
    static const int SPI_CLK = 4;
    static const int SPI_CS = 5;

    // 8 rows of 8*3 pixel each = 24*8 display
    long _Buffer[8];

    // Sends a command with a certain value to every connected device.
    inline void writeCommand(volatile byte opcode, volatile byte data);
};

#endif
