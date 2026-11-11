#include "WS2812.h"
#include "tim.h"
#include "dma.h"

#define PREAMBLE_SLOTS  2
#define LED_COUNT       1
#define BITS_PER_LED    24
#define RESET_SLOTS     240
#define TOTAL_SLOTS     (PREAMBLE_SLOTS + (BITS_PER_LED * LED_COUNT) + RESET_SLOTS)

#define WS2812_HIGH     120
#define WS2812_LOW      60

static uint16_t pwmData[TOTAL_SLOTS];
static volatile uint8_t ws2812_busy = 0;

void WS2812_Init(void)
{
    for (uint32_t i = 0; i < TOTAL_SLOTS; i++)
    {
        pwmData[i] = 0;
    }

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    TIM4->CCER |= TIM_CCER_CC4E;
}

void WS2812_SetRGB(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t startTick = HAL_GetTick();
    while (ws2812_busy && ((HAL_GetTick() - startTick) < 10U))
    {
    }

    uint32_t color =
        ((uint32_t)g << 16) |
        ((uint32_t)r << 8)  |
        (uint32_t)b;

    /* Leading zero slots absorb initial DMA preload pipeline before first timer overflow */
    for (uint32_t i = 0; i < PREAMBLE_SLOTS; i++)
    {
        pwmData[i] = 0;
    }

    for (uint8_t i = 0; i < 24; i++)
    {
        if (color & (1UL << (23 - i)))
            pwmData[PREAMBLE_SLOTS + i] = WS2812_HIGH;
        else
            pwmData[PREAMBLE_SLOTS + i] = WS2812_LOW;
    }

    for (uint32_t i = PREAMBLE_SLOTS + 24; i < TOTAL_SLOTS; i++)
    {
        pwmData[i] = 0;
    }

    ws2812_busy = 1;

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE | TIM_FLAG_CC4);

    if (HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_4, (uint32_t *)pwmData, TOTAL_SLOTS) != HAL_OK)
    {
        ws2812_busy = 0;
        return;
    }

    startTick = HAL_GetTick();
    while (ws2812_busy && ((HAL_GetTick() - startTick) < 10U))
    {
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
        HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_4);
        __HAL_DMA_DISABLE(htim->hdma[TIM_DMA_ID_CC4]);
        /* Keep Channel 4 output enabled with CCR4 = 0 so PB9 stays actively driven LOW (0V) */
        TIM4->CCER |= TIM_CCER_CC4E;
        ws2812_busy = 0;
    }
}
