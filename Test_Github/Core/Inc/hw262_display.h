#ifndef HW262_DISPLAY_H
#define HW262_DISPLAY_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HW262_DISPLAY_DIGITS 4u

typedef struct
{
    GPIO_TypeDef *data_port;
    uint16_t data_pin;
    GPIO_TypeDef *clock_port;
    uint16_t clock_pin;
    GPIO_TypeDef *latch_port;
    uint16_t latch_pin;
    uint8_t buffer[HW262_DISPLAY_DIGITS];
    uint8_t refresh_digit;
    bool enabled;
} HW262_Display;

void HW262_Display_Init(HW262_Display *display,
                        GPIO_TypeDef *data_port, uint16_t data_pin,
                        GPIO_TypeDef *clock_port, uint16_t clock_pin,
                        GPIO_TypeDef *latch_port, uint16_t latch_pin);

void HW262_Display_Clear(HW262_Display *display);
void HW262_Display_Enable(HW262_Display *display, bool enabled);
void HW262_Display_Refresh(HW262_Display *display);

void HW262_Display_WriteText(HW262_Display *display, const char *text);
void HW262_Display_WriteInt(HW262_Display *display, int16_t value, bool leading_zeros);
void HW262_Display_WriteUInt(HW262_Display *display, uint16_t value, bool leading_zeros);
void HW262_Display_WriteRaw(HW262_Display *display, uint8_t position, uint8_t segments);
void HW262_Display_SetDot(HW262_Display *display, uint8_t position, bool on);

#ifdef __cplusplus
}
#endif

#endif
