#ifndef WS2812_H
#define WS2812_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize TIM4_CH4 (PB9) + DMA1_Channel1 for WS2812 */
void WS2812_Init(void);

/* Set the color of the single onboard WS2812 LED */
void WS2812_SetRGB(uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* WS2812_H */
