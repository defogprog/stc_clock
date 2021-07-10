#include <stdint.h>
#include "stc12c.h"

#define DS1             P3_0
#define DS2             P3_1
#define DS3             P3_2
#define DS4             P3_3

#define DIG_PORT        P1
#define DIG_DOT         P1_0

#define BLINK_PERIOD    250

#define FOSC            12000000
#define T1MS            (65536 - FOSC/(12*1000))
#define T2MS            (65536 - FOSC/(12*500))
#define T5MS            (65536 - FOSC/(12*200))
#define T10MS           (65536 - FOSC/(12*100))

#define countof(_a) (sizeof(_a)/sizeof(*_a))

enum {
    _0,_1,_2,_3,
    _4,_5,_6,_7,
    _8,_9,_A,_B,
    _C,_D,_E,_F,
    _DASH,_H,_L,_I,
    __,
};

const static uint8_t symbols[] = {
    0xFC,0x24,0xBA,0xAE,    // 0, 1, 2, 3,
    0x66,0xCE,0xDE,0xA4,    // 4, 5, 6, 7,
    0xFE,0xEE,0xF6,0x5E,    // 8, 9, A, b,
    0xD8,0x3E,0xDA,0xD2,    // C, d, E, F,
    0x02,0x76,0x58,0x50,    // -, H, L, I,
    0x08,                   // _
};

volatile static uint8_t digits[4] = { _L,_0,_H,_I};

/**
 * FUNCTIONS
 */
static void init_timers(void) {
    // Init Timer0
    AUXR &= ~AUXR_T0x12;                // SYSclk/12
    TMOD = TMOD_T1_M0;                  // 16-bit Timer mode
    TL0 = T10MS;
    TH0 = T10MS>>8;
    TCON_TR0 = 1;                       // Enable Tim0
    IE_ET0 = 1;                         // Enable Tim0 Interrupts
}

/*
 * ISRs
 */
void T0_ISR(void) __interrupt (TIM0_VEC) {
    static uint8_t position = 0;
    uint8_t dot = 0;

    TL0 = T10MS;
    TH0 = T10MS>>8;

    static uint16_t times = 0;

    P3 |= 0x0F;                         // Turn off digits
    if (position == 1 && times <= (BLINK_PERIOD/2))
        dot = 1;
    DIG_PORT = symbols[digits[position]];
    P3 &= ~(1<<position);               // Turn on digits

    if (++position == 4) {
        position = 0;
        if (++times == BLINK_PERIOD) {
            times = 0;
        }
    }
}

/*
 * MAIN
 */
int main(void) {
    uint8_t d = 0;
    uint8_t num = 0;

    init_timers();

    IE_EA = 1;                          // Enable interrupts globally

    while (1) {

    }
}
