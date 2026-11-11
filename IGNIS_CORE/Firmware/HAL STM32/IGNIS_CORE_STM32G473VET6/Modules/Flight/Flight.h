#ifndef FLIGHT_H
#define FLIGHT_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONTROL_LOOP_HZ      100
#define CONTROL_LOOP_MS      (1000 / CONTROL_LOOP_HZ)

#define FILTER_ALPHA         0.98f

#define PID_KP               1.2f
#define PID_KI               0.05f
#define PID_KD               0.08f

#define OUTPUT_MIN           -100.0f
#define OUTPUT_MAX           100.0f

typedef struct {
    float integral;
    float last_error;
} PIDState;

typedef struct {
    float filtered_pitch;
    float filtered_roll;
    float yaw_rate;
    float pitch_cmd;
    float roll_cmd;
    float yaw_cmd;
    uint8_t fin1_angle;
    uint8_t fin2_angle;
    uint8_t fin3_angle;
    uint8_t fin4_angle;
    uint8_t armed;
    uint8_t calibrated;
} FlightState_t;

extern FlightState_t flight_state;

void Flight_Init(void);
void Flight_Update(void);
void Flight_Arm(void);
void Flight_Disarm(void);

void ComplementaryFilter_Update(float ax, float ay, float az,
                                float gx, float gy, float gz,
                                float dt);
float PID_Update(float setpoint, float measurement, float dt,
                 float Kp, float Ki, float Kd,
                 PIDState *state);
void Mixing_Update(float pitch_in, float roll_in, float yaw_in);

#ifdef __cplusplus
}
#endif

#endif /* FLIGHT_H */
