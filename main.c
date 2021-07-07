#if 1

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

#else

#include <8051.h>

// #define P1 0x90
// #define P3 0xB0

__sfr __at (0x90) P1;
__sfr __at (0xB0) P3;
#define P_1 0x90
#define P_3 0xB0

__sbit __at (P_1^0) P10;
__sbit __at (P_1^1) P11;
__sbit __at (P_1^2) P12;
__sbit __at (P_1^3) P13;
__sbit __at (P_1^4) P14;
__sbit __at (P_1^5) P15;
__sbit __at (P_1^6) P16;
__sbit __at (P_1^7) P17;

__sbit __at (P_3^0) P30;
__sbit __at (P_3^1) P31;
__sbit __at (P_3^2) P32;
__sbit __at (P_3^3) P33;
__sbit __at (P_3^4) P34;
__sbit __at (P_3^5) P35;
__sbit __at (P_3^6) P36;
__sbit __at (P_3^7) P37;

void delay()
{
    int i, j;

    for (i=0; i<1000; i++)
    for (j=0; j<500; j++);
}


void main()
{
    P10 = 0;
    delay();
    P10 = 1;
    P11 = 0;
    delay();
    P11 = 1;
    P12 = 0;
    delay();
    P12 = 1;
    P13 = 0;
    delay();
    P13 = 1;
    P14 = 0;
    delay();
    P14 = 1;
    P15 = 0;
    delay();
    P15 = 1;
    P16 = 0;
    delay();
    P16 = 1;
    P17 = 0;
    delay();
    P17 = 1;

    P30 = 0;
    delay();
    P30 = 1;
    P31 = 0;
    delay();
    P31 = 1;
    P32 = 0;
    delay();
    P32 = 1;
    P33 = 0;
    delay();
    P33 = 1;
    P34 = 0;
    delay();
    P34 = 1;
    P35 = 0;
    delay();
    P35 = 1;
    P36 = 0;
    delay();
    P36 = 1;
    P37 = 0;
    delay();
    P37 = 1;

    while (1)
    {
        P1 = 0x00;
        delay();
        P1 = 0xff;

        P3 = 0x00;
        delay();
        P3 = 0xff;
    }
}

#endif
