#ifndef HW262_DISPLAY_H
#define HW262_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

/* Initialize and blank the HW-262 four-digit seven-segment display. */
void HW262_Display_Init(void);

/* Store a value for display. Values above 9999 are limited to 9999. */
void HW262_Display_SetNumber(uint16_t value, bool leading_zeroes);

/* Blank all four digits. */
void HW262_Display_Clear(void);

/*
 * Refresh one digit. Call this continuously from the main loop; the function
 * limits itself to one update per millisecond using HAL_GetTick().
 */
void HW262_Display_Process(void);

#endif /* HW262_DISPLAY_H */
