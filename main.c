#include <stdint.h>
#include "stc12c.h"

#define DS1 P3_0
#define DS2 P3_1
#define DS3 P3_2
#define DS4 P3_3

#define DIG_PORT P1
#define DIG_DOT  P1_0

#define FOSC    12000000
#define T1MS    (65536 - FOSC/12/1000)
#define T2MS    (65536 - FOSC/12/500)
#define T5MS    (65536 - FOSC/12/200)
#define T10MS   (65536 - FOSC/12/100)

#define countof(_a) (sizeof(_a)/sizeof(*_a))

const uint8_t digits[] = {
    0xFC,0x24,0xBA,0xAE,    // 0, 1, 2, 3,
    0x66,0xCE,0xDE,0xA4,    // 4, 5, 6, 7,
    0xFE,0xEE,0xF6,0x5E,    // 9, 0, A, b,
    0xD8,0x3E,0xDA,0xD2,    // C, d, E, F,
    0x02,0x76,0x58,         // -, H, L,
};

/**
 * FUNCTIONS
 */
static void led_set_digit(uint8_t dig, uint8_t dot) {
    if (dig >= countof(digits)) {
        return;
    }
    DIG_PORT = digits[dig];
    if (dot) DIG_DOT = 1;
}

static void init_timers(void) {
    // Init Timer0
    TMOD = 0x01;
    TL0 = T5MS;
    TH0 = T5MS>>8;
    TR0 = 1;
    ET0 = 1;
}

/*
 * ISRs
 */
void T0_ISR(void) __interrupt (TIM0_VEC) {
    static uint8_t position = 0;
    uint8_t dot = 0;

    TL0 = T5MS;
    TH0 = T5MS>>8;

    static uint8_t times = 0;

        DS1 = DS2 = DS3 = DS4 = 1;
        if (position == 1 && times <= 50) dot = 1;
        led_set_digit(position, dot);
        if (position == 0) DS1 = 0; else
        if (position == 1) DS2 = 0; else
        if (position == 2) DS3 = 0; else
        if (position == 3) DS4 = 0;
        if (++position == 4) {
            position = 0;
            if (++times == 100) {
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

    // Enable interrupts globally
    EA = 1;

    while (1) {
        // P3 = (DS1 | DS2 | DS3 | DS4);
        // for (num=0; num<4; num++) {
        //     P3 = ~(1<<num);
        //     for (d = 0; d < countof(digits); d++) {
        //         P1 = digits[d];
        //         volatile uint16_t delay = (uint16_t)(-1);
        //         while (--delay);
        //         while (--delay);
        //     }
        // }
    }
}
