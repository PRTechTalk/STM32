#include "hw262_display.h"

#include "main.h"

#define HW262_DIGIT_COUNT  4U
#define HW262_BLANK        0xFFU

/* Active-low segment patterns: dp g f e d c b a. */
static const uint8_t segment_patterns[10] =
{
  0xC0U, 0xF9U, 0xA4U, 0xB0U, 0x99U,
  0x92U, 0x82U, 0xF8U, 0x80U, 0x90U
};

static uint8_t display_digits[HW262_DIGIT_COUNT] =
{
  HW262_BLANK, HW262_BLANK, HW262_BLANK, HW262_BLANK
};

static void HW262_ShiftByte(uint8_t value)
{
  uint8_t bit;

  for (bit = 0U; bit < 8U; bit++)
  {
    HAL_GPIO_WritePin(HW262_DISP_DATA_GPIO_Port, HW262_DISP_DATA_Pin,
                      ((value & 0x80U) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(HW262_DISP_CLK_GPIO_Port, HW262_DISP_CLK_Pin,
                      GPIO_PIN_SET);
    HAL_GPIO_WritePin(HW262_DISP_CLK_GPIO_Port, HW262_DISP_CLK_Pin,
                      GPIO_PIN_RESET);
    value <<= 1;
  }
}

static void HW262_WriteDigit(uint8_t position, uint8_t value)
{
  uint8_t segments = (value < 10U) ? segment_patterns[value] : 0xFFU;
  uint8_t digit_select = (uint8_t)(1U << position);

  HAL_GPIO_WritePin(HW262_DISP_LATCH_GPIO_Port, HW262_DISP_LATCH_Pin,
                    GPIO_PIN_RESET);
  HW262_ShiftByte(segments);
  HW262_ShiftByte(digit_select);
  HAL_GPIO_WritePin(HW262_DISP_LATCH_GPIO_Port, HW262_DISP_LATCH_Pin,
                    GPIO_PIN_SET);
}

void HW262_Display_Init(void)
{
  HAL_GPIO_WritePin(HW262_DISP_CLK_GPIO_Port, HW262_DISP_CLK_Pin,
                    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(HW262_DISP_DATA_GPIO_Port, HW262_DISP_DATA_Pin,
                    GPIO_PIN_RESET);
  HW262_Display_Clear();

  /* Shift an all-off frame into the two registers. */
  HAL_GPIO_WritePin(HW262_DISP_LATCH_GPIO_Port, HW262_DISP_LATCH_Pin,
                    GPIO_PIN_RESET);
  HW262_ShiftByte(0xFFU);
  HW262_ShiftByte(0x00U);
  HAL_GPIO_WritePin(HW262_DISP_LATCH_GPIO_Port, HW262_DISP_LATCH_Pin,
                    GPIO_PIN_SET);
}

void HW262_Display_SetNumber(uint16_t value, bool leading_zeroes)
{
  int32_t position;

  if (value > 9999U)
  {
    value = 9999U;
  }

  for (position = (int32_t)HW262_DIGIT_COUNT - 1; position >= 0; position--)
  {
    display_digits[position] = (uint8_t)(value % 10U);
    value /= 10U;

    if (!leading_zeroes && value == 0U && position > 0)
    {
      int32_t blank_position;

      for (blank_position = position - 1; blank_position >= 0; blank_position--)
      {
        display_digits[blank_position] = HW262_BLANK;
      }
      break;
    }
  }
}

void HW262_Display_Clear(void)
{
  uint32_t position;

  for (position = 0U; position < HW262_DIGIT_COUNT; position++)
  {
    display_digits[position] = HW262_BLANK;
  }
}

void HW262_Display_Process(void)
{
  static uint32_t last_refresh = UINT32_MAX;
  static uint8_t position = 0U;
  uint32_t now = HAL_GetTick();

  if (now == last_refresh)
  {
    return;
  }

  last_refresh = now;
  HW262_WriteDigit(position, display_digits[position]);
  position = (uint8_t)((position + 1U) % HW262_DIGIT_COUNT);
}
