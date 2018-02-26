#include <LedControl.h>

#include "../Service.hpp"

#ifndef LED_HPP
#define LED_HPP

class LedMatrixDriver : public Service
{
public:

    LedMatrixDriver();

    void Init();
    void Update();

    void ClearBuffer();
    void ClearDisplay();

    long* GetBuffer() { return _Buffer; }

    void DrawDigitBig(uint8_t x, uint8_t digit);
    void DrawDigitSmall(uint8_t x, uint8_t y, uint8_t digit);

private:
    static const int IC_COUNT = 3;

    LedControl _LedControl;
    // 8 rows of 8*3 pixel each = 24*8 display
    long _Buffer[8];
};

#endif
