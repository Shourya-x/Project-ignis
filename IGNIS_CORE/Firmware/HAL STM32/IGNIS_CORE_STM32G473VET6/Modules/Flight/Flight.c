#include "Flight.h"
#include "IMU.h"
#include "Servos.h"
#include "USB.h"
#include "WS2812.h"
#include <math.h>

FlightState_t flight_state = {
    .filtered_pitch = 0.0f,
    .filtered_roll  = 0.0f,
    .yaw_rate       = 0.0f,
    .pitch_cmd      = 0.0f,
    .roll_cmd       = 0.0f,
    .yaw_cmd        = 0.0f,
    .fin1_angle     = 90,
    .fin2_angle     = 90,
    .fin3_angle     = 90,
    .fin4_angle     = 90,
    .armed          = 0,
    .calibrated     = 0
};

static PIDState pid_pitch = {0.0f, 0.0f};
static PIDState pid_roll  = {0.0f, 0.0f};
static PIDState pid_yaw   = {0.0f, 0.0f};
static uint32_t last_control_time = 0;

void Flight_Init(void)
{
    flight_state.armed = 0;
    flight_state.calibrated = 1;
    Servo_Init();
    Servo_AllCenter();
    last_control_time = HAL_GetTick();
}

void Flight_Arm(void)
{
    flight_state.armed = 1;
    WS2812_SetRGB(255, 0, 0);  /* Red = ARMED */
}

void Flight_Disarm(void)
{
    flight_state.armed = 0;
    Servo_AllCenter();
    WS2812_SetRGB(0, 255, 0);  /* Green = DISARMED */
}

void Flight_Update(void)
{
    uint32_t now = HAL_GetTick();
    uint32_t dt_ms = now - last_control_time;

    if (dt_ms >= CONTROL_LOOP_MS)
    {
        float dt = dt_ms / 1000.0f;

        /* 1. Read IMU */
        float ax, ay, az, gx, gy, gz;
        IMU_GetScaled(&ax, &ay, &az, &gx, &gy, &gz);
        flight_state.yaw_rate = gz;

        /* 2. Complementary filter */
        ComplementaryFilter_Update(ax, ay, az, gx, gy, gz, dt);

        /* 3. PID controllers (target = 0 deg) */
        if (flight_state.armed)
        {
            flight_state.pitch_cmd = PID_Update(0.0f, flight_state.filtered_pitch, dt, PID_KP, PID_KI, PID_KD, &pid_pitch);
            flight_state.roll_cmd  = PID_Update(0.0f, flight_state.filtered_roll,  dt, PID_KP, PID_KI, PID_KD, &pid_roll);
            flight_state.yaw_cmd   = PID_Update(0.0f, gz,                          dt, PID_KP * 0.5f, PID_KI * 0.1f, PID_KD * 0.05f, &pid_yaw);
        }
        else
        {
            flight_state.pitch_cmd = 0.0f;
            flight_state.roll_cmd  = 0.0f;
            flight_state.yaw_cmd   = 0.0f;
            pid_pitch.integral = 0.0f;
            pid_roll.integral  = 0.0f;
            pid_yaw.integral   = 0.0f;
        }

        /* 4. X-configuration fin mixing */
        Mixing_Update(flight_state.pitch_cmd, flight_state.roll_cmd, flight_state.yaw_cmd);

        /* 5. Update servos */
        Servo_SetFin1(flight_state.fin1_angle);
        Servo_SetFin2(flight_state.fin2_angle);
        Servo_SetFin3(flight_state.fin3_angle);
        Servo_SetFin4(flight_state.fin4_angle);

        /* 6. Send USB telemetry */
        USB_SendTelemetry();

        last_control_time = now;
    }
}

void ComplementaryFilter_Update(float ax, float ay, float az,
                                float gx, float gy, float gz,
                                float dt)
{
    (void)gz;
    float accel_pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.29578f;
    float accel_roll  = atan2f(ay, -az) * 57.29578f;

    float gyro_pitch = flight_state.filtered_pitch + gx * dt;
    float gyro_roll  = flight_state.filtered_roll  + gy * dt;

    flight_state.filtered_pitch = FILTER_ALPHA * gyro_pitch + (1.0f - FILTER_ALPHA) * accel_pitch;
    flight_state.filtered_roll  = FILTER_ALPHA * gyro_roll  + (1.0f - FILTER_ALPHA) * accel_roll;
}

float PID_Update(float setpoint, float measurement, float dt,
                 float Kp, float Ki, float Kd,
                 PIDState *state)
{
    float error = setpoint - measurement;

    float P = Kp * error;

    state->integral += error * dt;
    if (state->integral > OUTPUT_MAX) state->integral = OUTPUT_MAX;
    if (state->integral < OUTPUT_MIN) state->integral = OUTPUT_MIN;
    float I = Ki * state->integral;

    float derivative = (error - state->last_error) / dt;
    float D = Kd * derivative;
    state->last_error = error;

    float output = P + I + D;
    if (output > OUTPUT_MAX) output = OUTPUT_MAX;
    if (output < OUTPUT_MIN) output = OUTPUT_MIN;

    return output;
}

void Mixing_Update(float pitch_in, float roll_in, float yaw_in)
{
    /* Orthogonal 4-Fin X-Configuration Aerodynamic Mixing:
     * Fins mounted aft at 45°, 135°, 225°, 315° around longitudinal axis Z.
     * Radial pivot axes: each fin rotates about its outward spanwise axis.
     * Roll (Z): All 4 fins deflect with identical sign for pure roll damping torque.
     * Pitch (X): Fins 2 & 3 oppose Fins 1 & 4 (moment about Y).
     * Yaw (Y):   Fins 3 & 4 oppose Fins 1 & 2 (moment about X).
     */
    float pitch_deflect = pitch_in * 0.7071f;
    float yaw_deflect   = yaw_in   * 0.7071f;
    float roll_deflect  = roll_in  * 0.9f;

    float fin1_deflect = -pitch_deflect - yaw_deflect + roll_deflect;   /* Fin 1 (45°  / Q1: +X, +Y) */
    float fin2_deflect = +pitch_deflect - yaw_deflect + roll_deflect;   /* Fin 2 (135° / Q2: -X, +Y) */
    float fin3_deflect = +pitch_deflect + yaw_deflect + roll_deflect;   /* Fin 3 (225° / Q3: -X, -Y) */
    float fin4_deflect = -pitch_deflect + yaw_deflect + roll_deflect;   /* Fin 4 (315° / Q4: +X, -Y) */

    int16_t f1 = 90 + (int16_t)fin1_deflect;
    int16_t f2 = 90 + (int16_t)fin2_deflect;
    int16_t f3 = 90 + (int16_t)fin3_deflect;
    int16_t f4 = 90 + (int16_t)fin4_deflect;

    if (f1 > 180) f1 = 180;
    if (f1 < 0)   f1 = 0;
    if (f2 > 180) f2 = 180;
    if (f2 < 0)   f2 = 0;
    if (f3 > 180) f3 = 180;
    if (f3 < 0)   f3 = 0;
    if (f4 > 180) f4 = 180;
    if (f4 < 0)   f4 = 0;

    flight_state.fin1_angle = (uint8_t)f1;
    flight_state.fin2_angle = (uint8_t)f2;
    flight_state.fin3_angle = (uint8_t)f3;
    flight_state.fin4_angle = (uint8_t)f4;
}
