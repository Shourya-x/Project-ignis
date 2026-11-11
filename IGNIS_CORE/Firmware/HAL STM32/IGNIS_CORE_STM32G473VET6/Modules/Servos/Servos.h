#ifndef SERVOS_H
#define SERVOS_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SERVO_MIN_US         1000
#define SERVO_MAX_US         2000
#define ANGLE_TO_PULSE(angle) (SERVO_MIN_US + ((angle) * (SERVO_MAX_US - SERVO_MIN_US) / 180))

void Servo_Init(void);
void Servo_SetFin1(uint8_t angle);
void Servo_SetFin2(uint8_t angle);
void Servo_SetFin3(uint8_t angle);
void Servo_SetFin4(uint8_t angle);
void Servo_AllCenter(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVOS_H */
