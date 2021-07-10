#include <stdint.h>
#include "stc12c.h"

#define DS1             P3_0
#define DS2             P3_1
#define DS3             P3_2
#define DS4             P3_3

#define DIG_PORT        P1
#define DIG_DOT         P1_0

#define BLINK_PERIOD    250

#define _TIM_PERIOD(_ms)    (-_ms##000 - 1)
#define TIM_PERIOD(_ms)     _TIM_PERIOD(_ms)

#define SCAN_PERIOD_MS     1

#define countof(_a) (sizeof(_a)/sizeof(*_a))

enum {
    _0,_1,_2,_3,
    _4,_5,_6,_7,
    _8,_9,_A,_B,
    _C,_D,_E,_F,
    _H,_L,_I,_P,
    _DASH,__,
};

const static uint8_t symbols[] = {
    0xFC,0x24,0xBA,0xAE,    // 0, 1, 2, 3,
    0x66,0xCE,0xDE,0xA4,    // 4, 5, 6, 7,
    0xFE,0xEE,0xF6,0x5E,    // 8, 9, A, b,
    0xD8,0x3E,0xDA,0xD2,    // C, d, E, F,
    0x76,0x58,0x50,0xF2,    // H, L, I,
    0x02,0x08,              // -, _.
};

typedef enum  {
    DOT_OFF,
    DOT_ON,
    DOT_BLINK,
} dot_state;

typedef struct _digit_t {
    uint8_t symb;
    uint8_t blink : 1;
    dot_state dot : 2;
} digit_t;

volatile static digit_t digits[4] = { 
    [0].symb = _1,
    [1].symb = _2,
    [2].symb = _P,
    [3].symb = _P,
    [1].dot = DOT_BLINK,
    [2].dot = DOT_ON,
    [3].dot = DOT_BLINK,
    [0].blink = 1,
    [2].blink = 1,
};

/**
 * FUNCTIONS
 */
static void init_timers(void) {
    // Init Timer0
    AUXR &= ~AUXR_T0x12;                // SYSclk/12
    TMOD = TMOD_T0_M0;                  // 16-bit Timer mode
    TL0 = TIM_PERIOD(SCAN_PERIOD_MS) & 0xFF;
    TH0 = TIM_PERIOD(SCAN_PERIOD_MS) >> 8;
    TCON_TR0 = 1;                       // Enable Tim0
    IE_ET0 = 1;                         // Enable Tim0 Interrupts
}

/*
 * ISRs
 */
void T0_ISR(void) __interrupt (TIM0_VEC) {
    static uint8_t position = 0;

    TL0 = TIM_PERIOD(SCAN_PERIOD_MS) & 0xFF;
    TH0 = TIM_PERIOD(SCAN_PERIOD_MS) >> 8;

    static uint16_t times = 0;

    P3 |= 0x0F;                         // Turn off digits

    if (digits[position].blink && times > (BLINK_PERIOD/2))
        DIG_PORT = 0;
    else
        DIG_PORT = symbols[digits[position].symb];

    switch (digits[position].dot) {
        case DOT_BLINK:
            if (times > (BLINK_PERIOD/2))
                break;
        case DOT_ON:
            DIG_PORT |= (1<<0);
            break;
    }
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
