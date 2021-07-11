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
#define ALARM_PIN               P3_7
#define _TIM_PERIOD(_ms)        (-_ms##000 - 1)
#define TIM_PERIOD(_ms)         _TIM_PERIOD(_ms)
#define LED_SCAN_PERIOD_MS      5
#define BUTTON_SCAN_PERIOD_MS   50
#define BLINK_PERIOD            (250 / LED_SCAN_PERIOD_MS)

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
    _DASH,__,_N,
};

enum {
    BUT_RELEASED,
    BUT_PRESSED,
    BUT_SHORT_PRESS,
    BUT_LONG_PRESS,
};

enum {
    MODE_12H,
    MODE_24H
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
    uint8_t blink;
    dot_state dot;
} digit_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t on;
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
    MODE_ALARM_ON_OFF,
    MODE_ALARM_END,
} clock_mode_t;

/**
 * @brief   Constants
 */
static const uint8_t days_in_month[12] = {
    31, 28, 31, 30,
    31, 30, 31, 31,
    30, 31, 30, 31
};
static const uint8_t symbols[] = {
    0xFC,0x24,0xBA,0xAE,    // 0, 1, 2, 3,
    0x66,0xCE,0xDE,0xA4,    // 4, 5, 6, 7,
    0xFE,0xEE,0xF6,0x5E,    // 8, 9, A, b,
    0xD8,0x3E,0xDA,0xD2,    // C, d, E, F,
    0x76,0x58,0x50,0xF2,    // H, L, I, P,
    0x02,0x08,0x16          // -, _, n,
};
static const uint8_t *params[] = {
    &(hours),
    &(minutes),
    &(day),
    &(month)
};

static const uint8_t limits[] = {
    0,  0,  1,  1,  // Lower limit
    24, 60, 32, 13, // Higher limit
};

static const uint16_t alarm_pattern = 0x0055;
/**
 * @brief   Variables
 */
static volatile digit_t digits[4];
static volatile clock_mode_t clock_mode;
static volatile uint8_t hours;
static volatile uint8_t minutes;
static volatile uint8_t seconds;
static volatile uint8_t day;
static volatile uint8_t month;
static volatile uint8_t mode_12_24;
static volatile uint8_t alarm_hour;
static volatile uint8_t alarm_minute;
static volatile uint8_t alarm_on = 1;
static volatile uint8_t alarm_active;
// static volatile alarm_t alarm[2];
static volatile uint16_t ticks = 0;          // This is actual time
static volatile struct {
    uint8_t mode;
    uint8_t set;
} buttons_state = {
    .mode = BUT_RELEASED,
    .set = BUT_RELEASED,
};
static uint8_t press_duration_mode = 0;
static uint8_t press_duration_set = 0;
static uint8_t position = 0;
static uint8_t times = 0;

/**
 * @brief   Functions
 */
static void init_timers(void) {
    TMOD = TMOD_T0_M0 | TMOD_T1_M0;         // 16-bit timer mode
    TL0  = TIM_PERIOD(LED_SCAN_PERIOD_MS) & 0xFF;
    TH0  = (TIM_PERIOD(LED_SCAN_PERIOD_MS) >> 8) & 0xFF;
    TL1  = TIM_PERIOD(BUTTON_SCAN_PERIOD_MS) & 0xFF;
    TH1  = (TIM_PERIOD(BUTTON_SCAN_PERIOD_MS) >> 8) & 0xFF;
    TCON = (1<<4)|(1<<6);                   // Enable Timers
    IE   = (1<<1)|(1<<3);                   // Enable Timers' Interrupts
}

static uint8_t dec(uint8_t num) {
    return num / 10;
}

static uint8_t ones(uint8_t num) {
    return num % 10;
}

/**
 * @brief   ISRs
 */
void T0_ISR(void) __interrupt (TIM0_VEC) {

    TL0 = TIM_PERIOD(LED_SCAN_PERIOD_MS) & 0xFF;
    TH0 = (TIM_PERIOD(LED_SCAN_PERIOD_MS) >> 8) & 0xFF;

    // Time and calendar
    if (++ticks == (1000 / LED_SCAN_PERIOD_MS)) {
        ticks = 0;
        if (++seconds == 60) {
            seconds = 0;
            if (++minutes == 60) {
                minutes = 0;
                if (++hours == 24) {
                    hours = 0;
                    if (++day == days_in_month[month] - 1) {
                        day = 0;
                        if (++month == 11) {
                            month = 0;
                        }
                    }
                }
            }
            // Alarm handling
            if (alarm_on) {
                if (alarm_minute == minutes && alarm_hour == hours) {
                    alarm_active = 100;
                }
            }
        }
    }
    uint8_t i;
    for (i=0; i<4; i++) {
        digits[i].symb = 0;
        digits[i].dot = DOT_OFF;
        if (alarm_active) {
            digits[i].blink = 1;
        } else {
            digits[i].blink = 0;
        }
    }

    i = 0;
    // Render symbols on LED display
    switch (clock_mode) {
        // Display "HR:MN" with blinkig MN and colon
        case MODE_SET_MIN:
            i = 2;
        // Display "HR:MN" with blinkig HR and colon
        case MODE_SET_HOUR:
            digits[i].blink = 1;
            digits[i+1].blink = 1;
        // Display "HR:MN" with blinking colon
        case MODE_NORMAL:
            digits[0].symb = symbols[dec(hours)];
            digits[1].symb = symbols[ones(hours)];
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[dec(minutes)];
            digits[3].symb = symbols[ones(minutes)];
            break;
        // Display "  :SE" with blinking colon
        case MODE_SECONDS:
            digits[1].dot = DOT_BLINK;
            digits[2].symb = symbols[dec(seconds)];
            digits[3].symb = symbols[ones(seconds)];
            break;
        // Display "DY.MO" with blinking MO
        case MODE_SET_MON:
            i = 2;
        // Display "DY.MO" with blinking DY
        case MODE_SET_DAY:
            digits[i].blink = 1;
            digits[i+1].blink = 1;
        // Display "DY.MO" with solid dot
        case MODE_DATE:
            digits[0].symb = symbols[dec(day)];
            digits[0].dot = DOT_ON;
            digits[1].symb = symbols[ones(day)];
            digits[2].symb = symbols[dec(month)];
            digits[3].symb = symbols[ones(month)];
            break;
        // Display "HR:MN" with blinking MN and solid colon
        case MODE_ALARM_MIN:
            i = 2;
        // Display "HR:MN" with blinking HR and solid colon
        case MODE_ALARM_HOUR:
            digits[i].blink = 1;
            digits[i+1].blink = 1;
        // Display "HR:MN" with solid colon
        case MODE_ALARM:
            digits[0].symb = symbols[dec(alarm_hour)];
            digits[1].symb = symbols[ones(alarm_hour)];
            digits[1].dot = DOT_ON;
            digits[2].symb = symbols[dec(alarm_minute)];
            digits[3].symb = symbols[ones(alarm_minute)];
            if (alarm_on) {
                uint8_t j;
                for (j = 0; j < 4; j++) {
                    digits[j].dot = DOT_BLINK;
                }
            }
            break;
        case MODE_ALARM_ON_OFF:
            digits[0].symb = symbols[_A];
            digits[1].symb = symbols[_L];
            digits[1].dot = DOT_ON;
            digits[2].symb = symbols[_0];
            digits[2].blink = 1;
            if (alarm_on) {
                digits[3].symb = symbols[_N];
            } else {
                digits[3].symb = symbols[_F];
            }
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

    BUTTON_MODE = 1;
    BUTTON_SET = 1;

    // MODE button state machine
    if (BUTTON_MODE == 0) {
        // Mute alarm
        if (alarm_active) {
            alarm_active = 0;
            ALARM_PIN = 1;
        }
        switch (buttons_state.mode) {
            case BUT_RELEASED:
                buttons_state.mode = BUT_PRESSED;
                if (clock_mode & MODE_NORMAL) {
                    if (++clock_mode == MODE_NORMAL_END) {
                        clock_mode = MODE_NORMAL;
                    }
                    if (clock_mode == MODE_DATE) {
                        timeout = 20;
                    }
                    if (clock_mode == MODE_SECONDS) {
                        timeout = 0;
                    }
                }
                if (clock_mode > MODE_ALARM && clock_mode < MODE_ALARM_END) {
                    if (++clock_mode == MODE_ALARM_END) {
                        clock_mode = MODE_NORMAL;
                    }
                }
                if (clock_mode & MODE_SET) {
                    if (++clock_mode == MODE_SET_END) {
                        clock_mode = MODE_NORMAL;
                    }
                }
                break;
            case BUT_PRESSED:
                if (++press_duration_mode == 20) {
                    press_duration_mode = 0;
                    buttons_state.mode = BUT_SHORT_PRESS;
                }
                break;
            case BUT_SHORT_PRESS:
                if (clock_mode & MODE_NORMAL) {
                    if (clock_mode == MODE_SECONDS) {
                        seconds = 0;
                    } else {
                        clock_mode = MODE_SET_HOUR;
                    }
                } else if (clock_mode == MODE_ALARM) {
                    timeout = 0;
                    clock_mode = MODE_ALARM_HOUR;
                } else {
                    clock_mode = MODE_NORMAL;
                }
                
                buttons_state.mode = BUT_LONG_PRESS;
                break;
            default:
                break;
        }
    } else {
        press_duration_mode = 0;
        buttons_state.mode = BUT_RELEASED;
    }

    // SET button state machine
    if (BUTTON_SET == 0) {
        uint8_t *param;
        uint8_t min, max;
        uint8_t index = (clock_mode & ~MODE_SET) - 1;
        param = (uint8_t*)params[index];
        min = limits[index];
        max = limits[index + 4];

        // Mute alarm
        if (alarm_active) {
            alarm_active = 0;
            ALARM_PIN = 1;
        }
        switch (buttons_state.set) {
            case BUT_RELEASED:
                buttons_state.set = BUT_PRESSED;
                if (clock_mode & MODE_SET) {
                    if (++(*param) == max) {
                        (*param) = min;
                    }
                }
                if (clock_mode == MODE_NORMAL) {
                    clock_mode = MODE_ALARM;
                    timeout = 40;
                }
                if (clock_mode & MODE_ALARM) {
                    if (clock_mode == MODE_ALARM_HOUR) {
                        if (++alarm_hour == 24) {
                            alarm_hour = 0;
                        }
                    }
                    if (clock_mode == MODE_ALARM_MIN) {
                        if (++alarm_minute == 60) {
                            alarm_minute = 0;
                        }
                    }
                    if (clock_mode == MODE_ALARM_ON_OFF) {
                        alarm_on ^= 1;
                    }
                }
                break;
            case BUT_PRESSED:
                if (++press_duration_set == 20) {
                    press_duration_set = 0;
                    buttons_state.set = BUT_LONG_PRESS;
                }
                break;
            case BUT_LONG_PRESS:
                if (clock_mode & MODE_SET) {
                    if (++(*param) == max) {
                        (*param) = min;
                    }
                }
                if (clock_mode == MODE_ALARM_HOUR) {
                    if (++alarm_hour == 24) {
                        alarm_hour = 0;
                    }
                }
                if (clock_mode == MODE_ALARM_MIN) {
                    if (++alarm_minute == 60) {
                        alarm_minute = 0;
                    }
                }
                if (clock_mode == MODE_SECONDS) {
                    seconds = 0;
                }
                break;
            default:
                break;
        }
    } else {
        press_duration_set = 0;
        buttons_state.set = BUT_RELEASED;
    }

    if (timeout != 0) {
        if (--timeout == 0) {
            clock_mode = MODE_NORMAL;
        }
    }

    static uint8_t alarm_bit = 0;
    if (alarm_on && alarm_active) {
        if (--alarm_active) {
            ALARM_PIN = !(alarm_pattern & (1 << alarm_bit));
            if (++alarm_bit == 16) {
                alarm_bit = 0;
            }
        }
    }
}

/**
 * @brief   Main function
 */
int main(void) {

    // Init time
    hours = 12;
    minutes = 0;
    seconds = 0;

    // Init date
    day = 1;
    month = 1;

    // Init alarm
    alarm_hour = 13;
    alarm_minute = 0;
    // alarm.on = 0;

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
