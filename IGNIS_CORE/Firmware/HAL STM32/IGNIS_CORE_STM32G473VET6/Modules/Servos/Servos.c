#include "Servos.h"
#include "tim.h"

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void Servo_SetFin1(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ANGLE_TO_PULSE(angle));
}

void Servo_SetFin2(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, ANGLE_TO_PULSE(angle));
}

void Servo_SetFin3(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, ANGLE_TO_PULSE(angle));
}

void Servo_SetFin4(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, ANGLE_TO_PULSE(angle));
}

void Servo_AllCenter(void)
{
    Servo_SetFin1(90);
    Servo_SetFin2(90);
    Servo_SetFin3(90);
    Servo_SetFin4(90);
}
