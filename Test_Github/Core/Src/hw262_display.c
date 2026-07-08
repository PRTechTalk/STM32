#include "hw262_display.h"
#include <stddef.h>

#ifndef HW262_DISPLAY_SHIFT_MSB_FIRST
#define HW262_DISPLAY_SHIFT_MSB_FIRST 1
#endif

#define HW262_DISPLAY_BLANK 0xffu
#define HW262_DISPLAY_DOT_MASK 0x80u

enum
{
    DISPLAY_0 = 0xc0,
    DISPLAY_1 = 0xf9,
    DISPLAY_2 = 0xa4,
    DISPLAY_3 = 0xb0,
    DISPLAY_4 = 0x99,
    DISPLAY_5 = 0x92,
    DISPLAY_6 = 0x82,
    DISPLAY_7 = 0xf8,
    DISPLAY_8 = 0x80,
    DISPLAY_9 = 0x90,
    DISPLAY_A = 0x88,
    DISPLAY_B = 0x83,
    DISPLAY_C = 0xc6,
    DISPLAY_D = 0xa1,
    DISPLAY_E = 0x86,
    DISPLAY_F = 0x8e,
    DISPLAY_G = 0x90,
    DISPLAY_H = 0x89,
    DISPLAY_I = 0xcf,
    DISPLAY_J = 0xe1,
    DISPLAY_L = 0xc7,
    DISPLAY_N = 0xab,
    DISPLAY_O = 0xa3,
    DISPLAY_P = 0x8c,
    DISPLAY_Q = 0x98,
    DISPLAY_R = 0xaf,
    DISPLAY_S = 0x92,
    DISPLAY_T = 0x87,
    DISPLAY_U = 0xc1,
    DISPLAY_Y = 0x91,
    DISPLAY_MINUS = 0xbf,
    DISPLAY_EQUAL = 0xb7,
    DISPLAY_UNDERSCORE = 0xf7,
    DISPLAY_LEFTBRACKET = 0xc6,
    DISPLAY_RIGHTBRACKET = 0xf0,
};

static const uint8_t digit_map[10] = {
    DISPLAY_0, DISPLAY_1, DISPLAY_2, DISPLAY_3, DISPLAY_4,
    DISPLAY_5, DISPLAY_6, DISPLAY_7, DISPLAY_8, DISPLAY_9,
};

static const uint8_t digit_select_map[HW262_DISPLAY_DIGITS] = {
    0x01u, 0x02u, 0x04u, 0x08u,
};

static void gpio_write(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state)
{
    HAL_GPIO_WritePin(port, pin, state);
}

static void pulse(GPIO_TypeDef *port, uint16_t pin)
{
    gpio_write(port, pin, GPIO_PIN_SET);
    gpio_write(port, pin, GPIO_PIN_RESET);
}

static void shift_byte(HW262_Display *display, uint8_t value)
{
#if HW262_DISPLAY_SHIFT_MSB_FIRST
    for (int8_t bit = 7; bit >= 0; bit--)
    {
        gpio_write(display->data_port, display->data_pin,
                   ((value >> bit) & 0x01u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        pulse(display->clock_port, display->clock_pin);
    }
#else
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        gpio_write(display->data_port, display->data_pin,
                   ((value >> bit) & 0x01u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        pulse(display->clock_port, display->clock_pin);
    }
#endif
}

static void shift_frame(HW262_Display *display, uint8_t segments, uint8_t digit_select)
{
    gpio_write(display->latch_port, display->latch_pin, GPIO_PIN_RESET);
    shift_byte(display, segments);
    shift_byte(display, digit_select);
    pulse(display->latch_port, display->latch_pin);
}

static uint8_t char_to_segments(char c)
{
    if (c >= '0' && c <= '9')
    {
        return digit_map[(uint8_t)(c - '0')];
    }

    if (c >= 'a' && c <= 'z')
    {
        c = (char)(c - ('a' - 'A'));
    }

    switch (c)
    {
    case 'A': return DISPLAY_A;
    case 'B': return DISPLAY_B;
    case 'C': return DISPLAY_C;
    case 'D': return DISPLAY_D;
    case 'E': return DISPLAY_E;
    case 'F': return DISPLAY_F;
    case 'G': return DISPLAY_G;
    case 'H': return DISPLAY_H;
    case 'I': return DISPLAY_I;
    case 'J': return DISPLAY_J;
    case 'L': return DISPLAY_L;
    case 'N': return DISPLAY_N;
    case 'O': return DISPLAY_O;
    case 'P': return DISPLAY_P;
    case 'Q': return DISPLAY_Q;
    case 'R': return DISPLAY_R;
    case 'S': return DISPLAY_S;
    case 'T': return DISPLAY_T;
    case 'U': return DISPLAY_U;
    case 'Y': return DISPLAY_Y;
    case '-': return DISPLAY_MINUS;
    case '=': return DISPLAY_EQUAL;
    case '_': return DISPLAY_UNDERSCORE;
    case '[': return DISPLAY_LEFTBRACKET;
    case ']': return DISPLAY_RIGHTBRACKET;
    case ' ':
    default: return HW262_DISPLAY_BLANK;
    }
}

void HW262_Display_Init(HW262_Display *display,
                        GPIO_TypeDef *data_port, uint16_t data_pin,
                        GPIO_TypeDef *clock_port, uint16_t clock_pin,
                        GPIO_TypeDef *latch_port, uint16_t latch_pin)
{
    if (display == NULL)
    {
        return;
    }

    display->data_port = data_port;
    display->data_pin = data_pin;
    display->clock_port = clock_port;
    display->clock_pin = clock_pin;
    display->latch_port = latch_port;
    display->latch_pin = latch_pin;
    display->refresh_digit = 0;
    display->enabled = true;

    gpio_write(display->data_port, display->data_pin, GPIO_PIN_RESET);
    gpio_write(display->clock_port, display->clock_pin, GPIO_PIN_RESET);
    gpio_write(display->latch_port, display->latch_pin, GPIO_PIN_RESET);

    HW262_Display_Clear(display);
}

void HW262_Display_Clear(HW262_Display *display)
{
    if (display == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < HW262_DISPLAY_DIGITS; i++)
    {
        display->buffer[i] = HW262_DISPLAY_BLANK;
    }

    shift_frame(display, HW262_DISPLAY_BLANK, 0x00u);
}

void HW262_Display_Enable(HW262_Display *display, bool enabled)
{
    if (display == NULL)
    {
        return;
    }

    display->enabled = enabled;

    if (!enabled)
    {
        shift_frame(display, HW262_DISPLAY_BLANK, 0x00u);
    }
}

void HW262_Display_Refresh(HW262_Display *display)
{
    if ((display == NULL) || !display->enabled)
    {
        return;
    }

    const uint8_t pos = display->refresh_digit;

    shift_frame(display, display->buffer[pos], digit_select_map[pos]);

    display->refresh_digit++;
    if (display->refresh_digit >= HW262_DISPLAY_DIGITS)
    {
        display->refresh_digit = 0;
    }
}

void HW262_Display_WriteText(HW262_Display *display, const char *text)
{
    if ((display == NULL) || (text == NULL))
    {
        return;
    }

    uint8_t out = 0;
    for (uint8_t i = 0; (text[i] != '\0') && (out < HW262_DISPLAY_DIGITS); i++)
    {
        if (text[i] == '.')
        {
            if (out > 0)
            {
                display->buffer[out - 1u] &= (uint8_t)~HW262_DISPLAY_DOT_MASK;
            }
            continue;
        }

        display->buffer[out++] = char_to_segments(text[i]);
    }

    while (out < HW262_DISPLAY_DIGITS)
    {
        display->buffer[out++] = HW262_DISPLAY_BLANK;
    }
}

void HW262_Display_WriteUInt(HW262_Display *display, uint16_t value, bool leading_zeros)
{
    if (display == NULL)
    {
        return;
    }

    if (value > 9999u)
    {
        HW262_Display_WriteText(display, "----");
        return;
    }

    uint16_t divisor = 1000u;
    bool started = leading_zeros;

    for (uint8_t pos = 0; pos < HW262_DISPLAY_DIGITS; pos++)
    {
        const uint8_t digit = (uint8_t)(value / divisor);
        value = (uint16_t)(value % divisor);

        if ((digit != 0u) || (pos == (HW262_DISPLAY_DIGITS - 1u)))
        {
            started = true;
        }

        display->buffer[pos] = started ? digit_map[digit] : HW262_DISPLAY_BLANK;
        divisor = (uint16_t)(divisor / 10u);
    }
}

void HW262_Display_WriteInt(HW262_Display *display, int16_t value, bool leading_zeros)
{
    if (display == NULL)
    {
        return;
    }

    if (value < 0)
    {
        if (value < -999)
        {
            HW262_Display_WriteText(display, "----");
            return;
        }

        display->buffer[0] = DISPLAY_MINUS;
        uint16_t magnitude = (uint16_t)(-value);
        uint16_t divisor = 100u;
        bool started = leading_zeros;

        for (uint8_t pos = 1; pos < HW262_DISPLAY_DIGITS; pos++)
        {
            const uint8_t digit = (uint8_t)(magnitude / divisor);
            magnitude = (uint16_t)(magnitude % divisor);

            if ((digit != 0u) || (pos == (HW262_DISPLAY_DIGITS - 1u)))
            {
                started = true;
            }

            display->buffer[pos] = started ? digit_map[digit] : HW262_DISPLAY_BLANK;
            divisor = (uint16_t)(divisor / 10u);
        }
        return;
    }

    HW262_Display_WriteUInt(display, (uint16_t)value, leading_zeros);
}

void HW262_Display_WriteRaw(HW262_Display *display, uint8_t position, uint8_t segments)
{
    if ((display == NULL) || (position >= HW262_DISPLAY_DIGITS))
    {
        return;
    }

    display->buffer[position] = segments;
}

void HW262_Display_SetDot(HW262_Display *display, uint8_t position, bool on)
{
    if ((display == NULL) || (position >= HW262_DISPLAY_DIGITS))
    {
        return;
    }

    if (on)
    {
        display->buffer[position] &= (uint8_t)~HW262_DISPLAY_DOT_MASK;
    }
    else
    {
        display->buffer[position] |= HW262_DISPLAY_DOT_MASK;
    }
}
