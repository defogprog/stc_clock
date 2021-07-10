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
#define BUTTON_MODE             P3_4
#define BUTTON_SET              P3_5
#define _TIM_PERIOD(_ms)        (-_ms##000 - 1)
#define TIM_PERIOD(_ms)         _TIM_PERIOD(_ms)
#define LED_SCAN_PERIOD_MS      1
#define BUTTON_SCAN_PERIOD_MS   50
#define BLINK_PERIOD            250

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

enum {
    BUT_RELEASED,
    BUT_PRESSED,
    BUT_SHORT_PRESS,
    BUT_LONG_PRESS,
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
} date_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t on : 1;
} alarm_t;

typedef enum {
    MODE_NORMAL         = 0x10,
    MODE_DATE,
    MODE_SECONDS,
    MODE_NORMAL_END,
    MODE_SET            = 0x20,
    MODE_SET_HOUR,
    MODE_SET_MIN,
    MODE_SET_DAY,
    MODE_SET_MON,
    MODE_SET_END,
    MODE_ALARM          = 0x40,
    MODE_ALARM_HOUR,
    MODE_ALARM_MIN,
    MODE_ALARM_STOP,
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
static const uint8_t *params[MODE_SET_END - MODE_SET - 1] = {
    &(time.hours),
    &(time.minutes),
    &(date.day),
    &(date.month)
};

static const uint8_t limits[MODE_SET_END - MODE_SET - 1] = {
    24, 60, 31, 12,
};

/**
 * @brief   Variables
 */
static volatile digit_t digits[4];
static volatile clock_mode_t clock_mode;
static volatile time_t time;
static volatile date_t date;
static volatile alarm_t alarm[2];
static volatile uint16_t ticks = 0;          // This is actual time
static volatile struct {
    uint8_t mode : 2;
    uint8_t set : 2;
} buttons_state = {
    .mode = BUT_RELEASED,
    .set = BUT_RELEASED,
};

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

static uint8_t dec(uint8_t num) {
    return num / 10;
}

static uint8_t ones(uint8_t num) {
    return num % 10;
}

/**
 * @brief           Increment number with overflow
 * 
 * @param num       Ptr to number to be incremented
 * @param limit     
 * @return uint8_t  0 if no overflow, 1 of overflowed
 */
static void inc_with_ovf(uint8_t *num, uint8_t limit) {
    // if (!num) return;
    if (++(*num) == limit) {
        (*num) = 0;
    }
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
        if (++time.seconds == 60) {
            time.seconds = 0;
            if (++time.minutes == 60) {
                time.minutes = 0;
                if (++time.hours == 24) {
                    time.hours = 0;
                    if (++date.day == days_in_month[date.month] - 1) {
                        date.day = 0;
                        if (++date.month == 11) {
                            date.month = 0;
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
        case MODE_NORMAL:
            digits[0].symb = symbols[dec(time.hours)];
            digits[1].symb = symbols[ones(time.hours)];
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[dec(time.minutes)];
            digits[3].symb = symbols[ones(time.minutes)];
            break;
        // Display "  :SE" with blinking colon
        case MODE_SECONDS:
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[dec(time.seconds)];
            digits[3].symb = symbols[ones(time.seconds)];
            break;
        // Display "DY.MO" with solid dot
        case MODE_DATE:
            digits[0].symb = symbols[dec(date.day)];
            digits[0].dot = DOT_ON;
            digits[1].symb = symbols[ones(date.day)];
            digits[2].symb = symbols[dec(date.month)];
            digits[3].symb = symbols[ones(date.month)];
            break;
        // Display "HR:MN" with solid colon
        case MODE_ALARM:
            digits[0].symb = symbols[dec(alarm[0].hour)];
            digits[1].symb = symbols[ones(alarm[0].hour)];
            digits[1].dot = DOT_ON;
            digits[2].symb = symbols[dec(alarm[0].minute)];
            digits[3].symb = symbols[ones(alarm[0].minute)];
            break;
        // Display "HR:MN" with blinkig HR and cloln
        case MODE_SET_HOUR:
            digits[0].symb = symbols[dec(time.hours)];
            digits[0].blink = 1;
            digits[1].symb = symbols[ones(time.hours)];
            digits[1].blink = 1;
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[dec(time.minutes)];
            digits[3].symb = symbols[ones(time.minutes)];
            break;
        // Display "HR:MN" with blinkig MN and cloln
        case MODE_SET_MIN:
            digits[0].symb = symbols[dec(time.hours)];
            digits[1].symb = symbols[ones(time.hours)];
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[dec(time.minutes)];
            digits[2].blink = 1;
            digits[3].symb = symbols[ones(time.minutes)];
            digits[3].blink = 1;
            break;
        // Display "DY.MO" with blinking DY
        case MODE_SET_DAY:
            digits[0].symb = symbols[dec(date.day)];
            digits[0].blink = 1;
            digits[0].dot = DOT_ON;
            digits[1].symb = symbols[ones(date.day)];
            digits[1].blink = 1;
            digits[2].symb = symbols[dec(date.month)];
            digits[3].symb = symbols[ones(date.month)];
            break;
        // Display "DY.MO" with blinking MO
        case MODE_SET_MON:
            digits[0].symb = symbols[dec(date.day)];
            digits[0].dot = DOT_ON;
            digits[1].symb = symbols[ones(date.day)];
            digits[2].symb = symbols[dec(date.month)];
            digits[2].blink = 1;
            digits[3].symb = symbols[ones(date.month)];
            digits[3].blink = 1;
            break;
        default:
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
    static uint8_t timeout = 0;
    static struct {
        uint8_t mode;
        uint8_t set;
    } press_duration = {
        .mode = 0,
        .set = 0,
    };

    BUTTON_MODE = 1;
    BUTTON_SET = 1;

    // MODE button state machine
    if (BUTTON_MODE == 0) {
        switch (buttons_state.mode) {
            case BUT_RELEASED:
                buttons_state.mode = BUT_PRESSED;
                if (clock_mode & MODE_SET) {
                    if (++clock_mode == MODE_SET_END) {
                        clock_mode = MODE_NORMAL;
                    }
                }
                break;
            case BUT_PRESSED:
                if (++press_duration.mode == 20) {
                    press_duration.mode = 0;
                    buttons_state.mode = BUT_SHORT_PRESS;
                }
                break;
            case BUT_SHORT_PRESS:
                if (clock_mode & MODE_NORMAL) {
                    if (clock_mode == MODE_SECONDS) {
                        time.seconds = 0;
                    } else {
                        clock_mode = MODE_SET_HOUR;
                    }
                } else {
                    clock_mode = MODE_NORMAL;
                }
                buttons_state.mode = BUT_LONG_PRESS;
                break;
            case BUT_LONG_PRESS:
                break;
            default:
                break;
        }
    } else {
        press_duration.mode = 0;
        buttons_state.mode = BUT_RELEASED;
    }

    // SET button state machine
    if (BUTTON_SET == 0) {
        uint8_t *param;
        uint8_t limit;

        switch (buttons_state.set) {
            case BUT_RELEASED:
                buttons_state.set = BUT_PRESSED;
                if (clock_mode & MODE_NORMAL) {
                    if (++clock_mode == MODE_NORMAL_END) {
                        clock_mode = MODE_NORMAL;
                    }
                }
                if (clock_mode == MODE_DATE) {
                    timeout = 20;
                }
                if (clock_mode == MODE_SECONDS) {
                    timeout = 0;
                }
                if (clock_mode & MODE_SET) {
                    param = params[clock_mode - MODE_SET - 1];
                    limit = limits[clock_mode - MODE_SET - 1];
                    inc_with_ovf(param, limit);
                }
                break;
            case BUT_PRESSED:
                if (++press_duration.set == 20) {
                    press_duration.set = 0;
                    buttons_state.set = BUT_LONG_PRESS;
                }
                break;
            case BUT_SHORT_PRESS:
                break;
            case BUT_LONG_PRESS:
                if (clock_mode & MODE_SET) {
                    param = params[clock_mode - MODE_SET - 1];
                    limit = limits[clock_mode - MODE_SET - 1];
                    inc_with_ovf(param, limit);
                    // if (clock_mode > MODE_SET_MIN) {
                    //     (*param)++;
                    // }
                }
                break;
            default:
                break;
        }
    } else {
        press_duration.set = 0;
        buttons_state.set = BUT_RELEASED;
    }

    if (timeout != 0) {
        if (--timeout == 0) {
            clock_mode = MODE_NORMAL;
        }
    }
}

/**
 * @brief   Main function
 */
int main(void) {

    // Init time
    time.hours = 12;
    time.minutes = 0;
    time.seconds = 0;

    // Init date
    date.day = 1;
    date.month = 1;

    // Init alarm
    alarm[0].hour = 13;
    alarm[0].minute = 0;
    alarm[0].on = 0;

    // Set default clock mode
    clock_mode = MODE_NORMAL;

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
