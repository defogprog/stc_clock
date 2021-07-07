#include <stdint.h>
#include "stc12c.h"

#define DS1 (1<<0)
#define DS2 (1<<1)
#define DS3 (1<<2)
#define DS4 (1<<3)

#define countof(_a) (sizeof(_a)/sizeof(*_a))

const uint8_t digits[] = {
	0xFC,0x24,0xBA,0xAE,    // 0, 1, 2, 3,
	0x66,0xCE,0xDE,0xA4,    // 4, 5, 6, 7,
	0xFE,0xEE,0xF6,0x5E,    // 9, 0, A, b,
	0xD8,0x3E,0xDA,0xD2,    // C, d, E, F,
    0x02,0x76,0x58,         // -, H, L,
};

int main(void) {
    uint8_t d = 0;
    uint8_t num = 0;
    P3 = ~(DS1);
        
    while (1) {
        P3 = (DS1 | DS2 | DS3 | DS4);
        for (num=0; num<4; num++) {
            P3 = ~(1<<num);
            for (d = 0; d < countof(digits); d++) {
                P1 = digits[d];
                volatile uint16_t delay = (uint16_t)(-1);
                while (--delay);
                while (--delay);
            }
        }
    }
    return 0;
}
