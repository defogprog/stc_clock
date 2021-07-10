/**
 * @file    main.c
 * @brief   Alternative FW for chineese clock kit with STC12C2052AD MCU
 * @author  defog (https://github.com/defogprog/stc_clock)
 * @version 0.1
 * @date    10/07/2021
 * 
 * @copyright GNU GPLv3 (c) 2021
 */

#include <stdint.h>
#include "stc12c.h"

/**
 * @brief   Defines
 */
#define DIGITS_PORT             P1
#define BLINK_PERIOD            250
#define _TIM_PERIOD(_ms)        (-_ms##000 - 1)
#define TIM_PERIOD(_ms)         _TIM_PERIOD(_ms)
#define LED_SCAN_PERIOD_MS      1
#define BUTTON_SCAN_PERIOD_MS   50

#define countof(_a) (sizeof(_a)/sizeof(*_a))

/**
 * @brief   Enums
 */
enum {
    _0,_1,_2,_3,
    _4,_5,_6,_7,
    _8,_9,_A,_B,
    _C,_D,_E,_F,
    _H,_L,_I,_P,
    _DASH,__,
};

/**
 * @brief   Typedefs
 * 
 */
typedef enum  {
    DOT_OFF,
    DOT_ON,
    DOT_BLINK,
} dot_state;

typedef struct {
    uint8_t symb;
    uint8_t blink : 1;
    dot_state dot : 2;
} digit_t;


typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} time_t;

typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t year;
} date_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t on : 1;
} alarm_t;

typedef enum {
    MODE_CLOCK,
    MODE_SECONDS,
    MODE_DATE,
    MODE_YEAR,
    MODE_ALARM,
    MODE_SET_HOUR,
    MODE_SET_MIN,
    MODE_SET_SEC,
    MODE_SET_DAY,
    MODE_SET_MON,
    MODE_SET_YEAR,
    MODE_SET_ALM_HOUR,
    MODE_SET_ALM_MIN,
} clock_mode_t;

/**
 * @brief   Constants
 */
static const uint8_t days_in_month[12] = {
    31, 28, 31, 30,
    31, 30, 31, 31,
    30, 31, 30, 31
};
static const static uint8_t symbols[] = {
    0xFC,0x24,0xBA,0xAE,    // 0, 1, 2, 3,
    0x66,0xCE,0xDE,0xA4,    // 4, 5, 6, 7,
    0xFE,0xEE,0xF6,0x5E,    // 8, 9, A, b,
    0xD8,0x3E,0xDA,0xD2,    // C, d, E, F,
    0x76,0x58,0x50,0xF2,    // H, L, I,
    0x02,0x08,              // -, _.
};
/**
 * @brief   Variables
 */
static volatile digit_t digits[4];
static volatile clock_mode_t clock_mode;
static volatile time_t time;
static volatile date_t date;
static volatile alarm_t alarm;
static volatile uint16_t ticks = 0;          // This is actual time

/**
 * @brief   Functions
 */
static void init_timers(void) {
    // Init Timer0
    AUXR &= ~AUXR_T0x12;                // SYSclk/12
    TMOD |= TMOD_T0_M0;                 // 16-bit timer mode
    TL0 = TIM_PERIOD(LED_SCAN_PERIOD_MS) & 0xFF;
    TH0 = (TIM_PERIOD(LED_SCAN_PERIOD_MS) >> 8) & 0xFF;
    TCON_TR0 = 1;                       // Enable Tim0
    IE_ET0 = 1;                         // Enable Tim0 Interrupts

    // Init Timer1
    AUXR &= ~AUXR_T1x12;                // SYSclk/12
    TMOD |= TMOD_T1_M0;                 // 16-bit timer mode
    TL1 = TIM_PERIOD(BUTTON_SCAN_PERIOD_MS) & 0xFF;
    TH1 = (TIM_PERIOD(BUTTON_SCAN_PERIOD_MS) >> 8) & 0xFF;
    TCON_TR1 = 1;
    IE_ET1 = 1;
}

/**
 * @brief   ISRs
 */
void T0_ISR(void) __interrupt (TIM0_VEC) {
    static uint8_t position = 0;
    static uint16_t times = 0;

    TL0 = TIM_PERIOD(LED_SCAN_PERIOD_MS) & 0xFF;
    TH0 = (TIM_PERIOD(LED_SCAN_PERIOD_MS) >> 8) & 0xFF;

    // Time and calendar
    if (++ticks == 1000) {
        ticks = 0;
        if (++time.seconds > 59) {
            time.seconds = 0;
            if (++time.minutes > 59) {
                time.minutes = 0;
                if (++time.hours > 23) {
                    time.hours = 0;
                    if (++date.day > days_in_month[date.month]) {
                        date.day = 1;
                        if (++date.month > 12) {
                            date.month = 1;
                            if (++date.year > 99) {
                                date.year = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    uint8_t i;
    for (i=0; i<4; i++) {
        digits[i].symb = 0;
        digits[i].blink = 0;
        digits[i].dot = DOT_OFF;
    }

    switch (clock_mode) {
        // Display "HR:MN" with blinking colon
        case MODE_CLOCK:
            digits[0].symb = symbols[time.hours / 10];
            digits[1].symb = symbols[time.hours % 10];
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[time.minutes / 10];
            digits[3].symb = symbols[time.minutes % 10];
            break;

        // Display "  :SE" with solid colon
        case MODE_SECONDS:
            digits[1].dot = DOT_ON;
            digits[2].symb = symbols[time.seconds / 10];
            digits[3].symb = symbols[time.seconds % 10];
            break;
        // Display "DY.MO" with solid dot
        case MODE_DATE:
            digits[0].symb = symbols[date.day / 10];
            digits[0].dot = DOT_ON;
            digits[1].symb = symbols[date.day % 10];
            digits[2].symb = symbols[date.month / 10];
            digits[3].symb = symbols[date.month % 10];
            break;
        // Display "YYYY"
        case MODE_YEAR:
            digits[0].symb = symbols[_2];
            digits[1].symb = symbols[_0];
            digits[2].symb = symbols[date.year / 10];
            digits[3].symb = symbols[date.year % 10];
            break;
        // Display "HR:MN" with solid colon
        case MODE_ALARM:
            digits[0].symb = symbols[alarm.hour / 10];
            digits[1].symb = symbols[alarm.hour % 10];
            digits[1].dot = DOT_ON;
            digits[2].symb = symbols[alarm.minute / 10];
            digits[3].symb = symbols[alarm.minute % 10];
            break;
    }

    P3 |= 0x0F;                         // Turn off digits

    if (digits[position].blink && times > (BLINK_PERIOD/2))
        DIGITS_PORT = 0;
    else
        DIGITS_PORT = digits[position].symb;

    switch (digits[position].dot) {
        case DOT_BLINK:
            if (times > (BLINK_PERIOD/2))
                break;
        case DOT_ON:
            DIGITS_PORT |= (1<<0);
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

void T1_ISR(void) __interrupt(TIM1_VEC) {

}

/**
 * @brief   Main function
 */
int main(void) {

    // Init time
    time.hours = time.minutes = time.seconds = 0;

    // Init date
    date.day = 1;
    date.month = 1;
    date.year = 20;

    // Init alarm
    alarm.hour = 12;
    alarm.minute = 0;
    alarm.on = 0;

    // Set default clock mode
    clock_mode = MODE_YEAR;

    init_timers();

    // Enable interrupts globally
    IE_EA = 1;

    // Loop forever
    while (1) {
    }
}

/**
 * @brief   End
 */
