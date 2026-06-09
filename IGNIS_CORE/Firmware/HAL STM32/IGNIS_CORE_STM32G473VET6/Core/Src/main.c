/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : IGNIS Rocket - X-Configuration Fin Control
  *                   MPU6500 + MS5611 + 4 Servos + USB Telemetry
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Private define ------------------------------------------------------------*/

// ==================== I2C2 Pins ====================
#define I2C2_SCL_PIN GPIO_PIN_8    // PA8
#define I2C2_SDA_PIN GPIO_PIN_4    // PC4
#define I2C2_SCL_PORT GPIOA
#define I2C2_SDA_PORT GPIOC

// ==================== MPU6500 ====================
#define MPU6500_ADDR         0x68
#define MPU6500_WHO_AM_I     0x75
#define MPU6500_PWR_MGMT_1   0x6B
#define MPU6500_ACCEL_CONFIG 0x1C
#define MPU6500_GYRO_CONFIG  0x1B
#define MPU6500_ACCEL_XOUT_H 0x3B
#define MPU6500_GYRO_XOUT_H  0x43

// ==================== MS5611 ====================
#define MS5611_ADDR          0x77
#define MS5611_RESET         0x1E
#define MS5611_READ_ADC      0x00
#define MS5611_CONV_D1       0x40  // Pressure
#define MS5611_CONV_D2       0x50  // Temperature

// ==================== Control Parameters ====================
#define CONTROL_LOOP_HZ      100
#define CONTROL_LOOP_MS      (1000 / CONTROL_LOOP_HZ)  // 10ms

#define FILTER_ALPHA         0.98f   // Complementary filter

#define PID_KP               1.2f
#define PID_KI               0.05f
#define PID_KD               0.08f

#define OUTPUT_MIN           -100.0f
#define OUTPUT_MAX           100.0f

#define SERVO_MIN_US         1000
#define SERVO_MAX_US         2000
#define ANGLE_TO_PULSE(angle) (SERVO_MIN_US + ((angle) * (SERVO_MAX_US - SERVO_MIN_US) / 180))

// ==================== Commands ====================
#define ARM_CMD              'A'
#define DISARM_CMD           'D'
#define CALIBRATE_CMD        'C'

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;
TIM_HandleTypeDef htim2;

// MS5611 calibration coefficients
static uint16_t C[8];

// MPU6500 calibration offsets
int16_t accel_offset[3] = {0, 0, 0};
int16_t gyro_offset[3]  = {0, 0, 0};

// Filter state
float filtered_pitch = 0;
float filtered_roll = 0;
uint32_t last_control_time = 0;

// PID states
typedef struct {
    float integral;
    float last_error;
} PIDState;

PIDState pid_pitch = {0, 0};
PIDState pid_roll = {0, 0};
PIDState pid_yaw = {0, 0};

// Control outputs
float pitch_cmd = 0;
float roll_cmd = 0;
float yaw_cmd = 0;

// Fin angles (0-180, 90 = center)
uint8_t fin1_angle = 90;
uint8_t fin2_angle = 90;
uint8_t fin3_angle = 90;
uint8_t fin4_angle = 90;

// System state
uint8_t armed = 0;
uint8_t calibrated = 0;

// USB receive buffer
uint8_t usb_rx_buffer[64];
uint8_t usb_rx_len = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM2_Init(void);

// MPU6500 functions
void MPU6500_Init(void);
uint8_t MPU6500_Read_Byte(uint8_t reg);
void MPU6500_Read_Raw(int16_t *ax, int16_t *ay, int16_t *az,
                      int16_t *gx, int16_t *gy, int16_t *gz);
void MPU6500_Calibrate(void);
void MPU6500_Get_Scaled(float *ax, float *ay, float *az,
                        float *gx, float *gy, float *gz);

// MS5611 functions
void MS5611_Init(void);
void MS5611_Read_Calibration(void);
void MS5611_Read(int32_t *temp, int32_t *pres);

// Servo functions
void Servo_Init(void);
void Servo_SetFin1(uint8_t angle);
void Servo_SetFin2(uint8_t angle);
void Servo_SetFin3(uint8_t angle);
void Servo_SetFin4(uint8_t angle);
void Servo_AllCenter(void);

// Control functions
void ComplementaryFilter_Update(float ax, float ay, float az,
                                 float gx, float gy, float gz,
                                 float dt);
float PID_Update(float setpoint, float measurement, float dt,
                 float Kp, float Ki, float Kd,
                 PIDState *state);
void Mixing_Update(float pitch_in, float roll_in, float yaw_in);
void SendTelemetry(void);
void Process_USB_Command(uint8_t *data, uint8_t len);

/* ==================== MAIN ==================== */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USB_Device_Init();
    MX_I2C2_Init();
    MX_TIM2_Init();

    HAL_Delay(1000);  // Wait for USB to enumerate

    CDC_Transmit_FS((uint8_t*)"\r\n", 2);
    CDC_Transmit_FS((uint8_t*)"========================================\r\n", 40);
    CDC_Transmit_FS((uint8_t*)"  IGNIS ROCKET - X-CONFIGURATION FINS\r\n", 38);
    CDC_Transmit_FS((uint8_t*)"  STM32G473VET6 | MPU6500 | 4 Servos\r\n", 38);
    CDC_Transmit_FS((uint8_t*)"========================================\r\n", 40);

    // Initialize MPU6500
    CDC_Transmit_FS((uint8_t*)"\r\n[1/4] Initializing MPU6500...\r\n", 32);
    MPU6500_Init();

    // Initialize MS5611
    CDC_Transmit_FS((uint8_t*)"[2/4] Initializing MS5611...\r\n", 30);
    MS5611_Init();

    // Calibrate IMU
    CDC_Transmit_FS((uint8_t*)"[3/4] Calibrating IMU...\r\n", 26);
    CDC_Transmit_FS((uint8_t*)"      KEEP BOARD STILL!\r\n", 24);
    HAL_Delay(1000);
    MPU6500_Calibrate();
    calibrated = 1;

    // Initialize servos
    CDC_Transmit_FS((uint8_t*)"[4/4] Initializing Servos...\r\n", 30);
    Servo_Init();
    Servo_AllCenter();

    CDC_Transmit_FS((uint8_t*)"\r\n=== SYSTEM READY ===\r\n", 23);
    CDC_Transmit_FS((uint8_t*)"Commands: A=ARM, D=DISARM, C=CALIBRATE\r\n", 39);
    CDC_Transmit_FS((uint8_t*)"\r\n");
    CDC_Transmit_FS((uint8_t*)"Pitch,Roll,YawRate,PitchCmd,RollCmd,YawCmd,Fin1,Fin2,Fin3,Fin4,Armed\r\n", 68);

    last_control_time = HAL_GetTick();

    while (1)
    {
        uint32_t now = HAL_GetTick();
        uint32_t dt_ms = now - last_control_time;

        if (dt_ms >= CONTROL_LOOP_MS) {
            float dt = dt_ms / 1000.0f;

            // 1. Read sensors
            float ax, ay, az, gx, gy, gz;
            MPU6500_Get_Scaled(&ax, &ay, &az, &gx, &gy, &gz);

            // 2. Complementary filter
            ComplementaryFilter_Update(ax, ay, az, gx, gy, gz, dt);

            // 3. PID controllers (target = 0)
            if (armed) {
                pitch_cmd = PID_Update(0.0f, filtered_pitch, dt, PID_KP, PID_KI, PID_KD, &pid_pitch);
                roll_cmd  = PID_Update(0.0f, filtered_roll,  dt, PID_KP, PID_KI, PID_KD, &pid_roll);
                yaw_cmd   = PID_Update(0.0f, gz,             dt, PID_KP*0.5f, PID_KI*0.1f, PID_KD*0.05f, &pid_yaw);
            } else {
                pitch_cmd = 0;
                roll_cmd = 0;
                yaw_cmd = 0;
                // Reset PID states when disarmed
                pid_pitch.integral = 0;
                pid_roll.integral = 0;
                pid_yaw.integral = 0;
            }

            // 4. Mixing (X-configuration)
            Mixing_Update(pitch_cmd, roll_cmd, yaw_cmd);

            // 5. Update servos
            Servo_SetFin1(fin1_angle);
            Servo_SetFin2(fin2_angle);
            Servo_SetFin3(fin3_angle);
            Servo_SetFin4(fin4_angle);

            // 6. Send telemetry
            SendTelemetry();

            last_control_time = now;
        }

        // Check for USB commands
        if (usb_rx_len > 0) {
            Process_USB_Command(usb_rx_buffer, usb_rx_len);
            usb_rx_len = 0;
        }
    }
}

/* ==================== MPU6500 FUNCTIONS ==================== */

void MPU6500_Init(void)
{
    uint8_t whoami = MPU6500_Read_Byte(MPU6500_WHO_AM_I);
    char buf[64];

    sprintf(buf, "MPU6500 WHO_AM_I = 0x%02X (expected 0x70)\r\n", whoami);
    CDC_Transmit_FS((uint8_t*)buf, strlen(buf));

    // Wake up MPU6500 (disable sleep mode)
    uint8_t data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, (MPU6500_ADDR << 1), MPU6500_PWR_MGMT_1, 1, &data, 1, 100);

    // Configure accelerometer (±8g)
    data = 0x10;
    HAL_I2C_Mem_Write(&hi2c2, (MPU6500_ADDR << 1), MPU6500_ACCEL_CONFIG, 1, &data, 1, 100);

    // Configure gyroscope (±1000°/s)
    data = 0x10;
    HAL_I2C_Mem_Write(&hi2c2, (MPU6500_ADDR << 1), MPU6500_GYRO_CONFIG, 1, &data, 1, 100);
}

uint8_t MPU6500_Read_Byte(uint8_t reg)
{
    uint8_t data;
    HAL_I2C_Mem_Read(&hi2c2, (MPU6500_ADDR << 1), reg, 1, &data, 1, 100);
    return data;
}

void MPU6500_Read_Raw(int16_t *ax, int16_t *ay, int16_t *az,
                      int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t buf[14];
    HAL_I2C_Mem_Read(&hi2c2, (MPU6500_ADDR << 1), MPU6500_ACCEL_XOUT_H, 1, buf, 14, 100);

    *ax = (int16_t)((buf[0] << 8) | buf[1]);
    *ay = (int16_t)((buf[2] << 8) | buf[3]);
    *az = (int16_t)((buf[4] << 8) | buf[5]);
    *gx = (int16_t)((buf[8] << 8) | buf[9]);
    *gy = (int16_t)((buf[10] << 8) | buf[11]);
    *gz = (int16_t)((buf[12] << 8) | buf[13]);
}

void MPU6500_Calibrate(void)
{
    int32_t sum_accel[3] = {0, 0, 0};
    int32_t sum_gyro[3]  = {0, 0, 0};
    const int samples = 500;
    char buf[100];

    for (int i = 0; i < samples; i++) {
        int16_t ax, ay, az, gx, gy, gz;
        MPU6500_Read_Raw(&ax, &ay, &az, &gx, &gy, &gz);
        sum_accel[0] += ax;
        sum_accel[1] += ay;
        sum_accel[2] += az;
        sum_gyro[0]  += gx;
        sum_gyro[1]  += gy;
        sum_gyro[2]  += gz;
        HAL_Delay(2);
    }

    accel_offset[0] = sum_accel[0] / samples;
    accel_offset[1] = sum_accel[1] / samples;
    accel_offset[2] = (sum_accel[2] / samples) - 16384;  // Expect 1g on Z

    gyro_offset[0] = sum_gyro[0] / samples;
    gyro_offset[1] = sum_gyro[1] / samples;
    gyro_offset[2] = sum_gyro[2] / samples;

    sprintf(buf, "Offsets: A(%d,%d,%d) G(%d,%d,%d)\r\n",
            accel_offset[0], accel_offset[1], accel_offset[2],
            gyro_offset[0], gyro_offset[1], gyro_offset[2]);
    CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
}

void MPU6500_Get_Scaled(float *ax, float *ay, float *az,
                        float *gx, float *gy, float *gz)
{
    int16_t rax, ray, raz, rgx, rgy, rgz;

    MPU6500_Read_Raw(&rax, &ray, &raz, &rgx, &rgy, &rgz);

    // Apply calibration offsets
    rax -= accel_offset[0];
    ray -= accel_offset[1];
    raz -= accel_offset[2];
    rgx -= gyro_offset[0];
    rgy -= gyro_offset[1];
    rgz -= gyro_offset[2];

    // Convert to physical units
    // Accel: ±8g = 4096 LSB/g (since we set ±8g range)
    *ax = rax / 4096.0f;
    *ay = ray / 4096.0f;
    *az = raz / 4096.0f;

    // Gyro: ±1000 deg/s = 32.8 LSB/(deg/s)
    *gx = rgx / 32.8f;
    *gy = rgy / 32.8f;
    *gz = rgz / 32.8f;
}

/* ==================== MS5611 FUNCTIONS ==================== */

void MS5611_Init(void)
{
    uint8_t reset = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, (MS5611_ADDR << 1), MS5611_RESET, 1, &reset, 1, 100);
    HAL_Delay(10);

    MS5611_Read_Calibration();
}

void MS5611_Read_Calibration(void)
{
    uint8_t buf[2];
    char msg[64];

    CDC_Transmit_FS((uint8_t*)"Reading MS5611 calibration coefficients...\r\n", 42);

    for (int i = 1; i <= 6; i++) {
        uint8_t cmd = 0xA0 + (i * 2);
        HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
        HAL_I2C_Master_Receive(&hi2c2, (MS5611_ADDR << 1), buf, 2, 100);
        C[i] = ((uint16_t)buf[0] << 8) | buf[1];

        sprintf(msg, "  C[%d] = %u\r\n", i, C[i]);
        CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
    }

    CDC_Transmit_FS((uint8_t*)"Calibration done.\r\n", 19);
}

void MS5611_Read(int32_t *temp, int32_t *pres)
{
    uint8_t buf[3];
    uint32_t D1, D2;

    // Pressure conversion (OSR=4096)
    uint8_t cmd = MS5611_CONV_D1 + 0x08;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_Delay(10);

    cmd = MS5611_READ_ADC;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2, (MS5611_ADDR << 1), buf, 3, 100);
    D1 = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];

    // Temperature conversion (OSR=4096)
    cmd = MS5611_CONV_D2 + 0x08;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_Delay(10);

    cmd = MS5611_READ_ADC;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2, (MS5611_ADDR << 1), buf, 3, 100);
    D2 = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];

    // Calculate dT and temperature
    int32_t dT = D2 - ((uint32_t)C[5] << 8);
    int32_t TEMP = 2000 + ((int64_t)dT * C[6]) / 8388608;

    // Calculate OFF and SENS
    int64_t OFF = (int64_t)C[2] * 65536 + ((int64_t)C[4] * dT) / 128;
    int64_t SENS = (int64_t)C[1] * 32768 + ((int64_t)C[3] * dT) / 256;

    // Second-order correction for low temperature
    if (TEMP < 2000) {
        int32_t T2 = ((int64_t)dT * dT) / 0x800000;
        int64_t OFF2 = 5 * (TEMP - 2000) * (TEMP - 2000) / 2;
        int64_t SENS2 = 5 * (TEMP - 2000) * (TEMP - 2000) / 4;

        TEMP -= T2;
        OFF -= OFF2;
        SENS -= SENS2;
    }

    // Calculate pressure
    int64_t P = (D1 * SENS / 2097152 - OFF) / 32768;

    *temp = TEMP;
    *pres = (int32_t)P;
}

/* ==================== SERVO FUNCTIONS ==================== */

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void Servo_SetFin1(uint8_t angle)
{
    uint16_t pulse = ANGLE_TO_PULSE(angle);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse);
}

void Servo_SetFin2(uint8_t angle)
{
    uint16_t pulse = ANGLE_TO_PULSE(angle);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
}

void Servo_SetFin3(uint8_t angle)
{
    uint16_t pulse = ANGLE_TO_PULSE(angle);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
}

void Servo_SetFin4(uint8_t angle)
{
    uint16_t pulse = ANGLE_TO_PULSE(angle);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, pulse);
}

void Servo_AllCenter(void)
{
    Servo_SetFin1(90);
    Servo_SetFin2(90);
    Servo_SetFin3(90);
    Servo_SetFin4(90);
}

/* ==================== COMPLEMENTARY FILTER ==================== */

void ComplementaryFilter_Update(float ax, float ay, float az,
                                 float gx, float gy, float gz,
                                 float dt)
{
    // Calculate pitch and roll from accelerometer
    float accel_pitch = atan2f(-ax, sqrtf(ay*ay + az*az)) * 57.29578f;
    float accel_roll  = atan2f(ay, -az) * 57.29578f;

    // Gyro integration
    float gyro_pitch = filtered_pitch + gx * dt;
    float gyro_roll  = filtered_roll  + gy * dt;

    // Complementary filter
    filtered_pitch = FILTER_ALPHA * gyro_pitch + (1.0f - FILTER_ALPHA) * accel_pitch;
    filtered_roll  = FILTER_ALPHA * gyro_roll  + (1.0f - FILTER_ALPHA) * accel_roll;
}

/* ==================== PID CONTROLLER ==================== */

float PID_Update(float setpoint, float measurement, float dt,
                 float Kp, float Ki, float Kd,
                 PIDState *state)
{
    float error = setpoint - measurement;

    // Proportional
    float P = Kp * error;

    // Integral (with anti-windup)
    state->integral += error * dt;
    if (state->integral > OUTPUT_MAX) state->integral = OUTPUT_MAX;
    if (state->integral < OUTPUT_MIN) state->integral = OUTPUT_MIN;
    float I = Ki * state->integral;

    // Derivative
    float derivative = (error - state->last_error) / dt;
    float D = Kd * derivative;
    state->last_error = error;

    // Output
    float output = P + I + D;
    if (output > OUTPUT_MAX) output = OUTPUT_MAX;
    if (output < OUTPUT_MIN) output = OUTPUT_MIN;

    return output;
}

/* ==================== X-CONFIGURATION MIXING ==================== */

void Mixing_Update(float pitch_in, float roll_in, float yaw_in)
{
    // Convert to deflection (±90° from neutral)
    float pitch_deflect = pitch_in * 0.9f;
    float roll_deflect  = roll_in  * 0.9f;
    float yaw_deflect   = yaw_in   * 0.9f;

    // X-configuration mixing matrix
    float fin1_deflect =  pitch_deflect + roll_deflect + yaw_deflect;   // Front-Right
    float fin2_deflect =  pitch_deflect - roll_deflect - yaw_deflect;   // Front-Left
    float fin3_deflect = -pitch_deflect - roll_deflect + yaw_deflect;   // Rear-Left
    float fin4_deflect = -pitch_deflect + roll_deflect - yaw_deflect;   // Rear-Right

    // Convert to servo angles (90° = neutral)
    int16_t f1 = 90 + (int16_t)fin1_deflect;
    int16_t f2 = 90 + (int16_t)fin2_deflect;
    int16_t f3 = 90 + (int16_t)fin3_deflect;
    int16_t f4 = 90 + (int16_t)fin4_deflect;

    // Clamp
    if (f1 > 180) f1 = 180; if (f1 < 0) f1 = 0;
    if (f2 > 180) f2 = 180; if (f2 < 0) f2 = 0;
    if (f3 > 180) f3 = 180; if (f3 < 0) f3 = 0;
    if (f4 > 180) f4 = 180; if (f4 < 0) f4 = 0;

    fin1_angle = (uint8_t)f1;
    fin2_angle = (uint8_t)f2;
    fin3_angle = (uint8_t)f3;
    fin4_angle = (uint8_t)f4;
}

/* ==================== TELEMETRY ==================== */

void SendTelemetry(void)
{
    char buf[256];
    snprintf(buf, sizeof(buf),
             "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d\r\n",
             filtered_pitch, filtered_roll, 0.0f,  // yaw_rate placeholder
             pitch_cmd, roll_cmd, yaw_cmd,
             fin1_angle, fin2_angle, fin3_angle, fin4_angle,
             armed);
    CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
}

/* ==================== USB COMMANDS ==================== */

void Process_USB_Command(uint8_t *data, uint8_t len)
{
    if (len == 0) return;

    char cmd = data[0];

    switch (cmd) {
        case 'A':
        case 'a':
            armed = 1;
            CDC_Transmit_FS((uint8_t*)"\r\n>>> ARMED <<<\r\n", 18);
            break;

        case 'D':
        case 'd':
            armed = 0;
            Servo_AllCenter();
            CDC_Transmit_FS((uint8_t*)"\r\n>>> DISARMED <<<\r\n", 20);
            break;

        case 'C':
        case 'c':
            CDC_Transmit_FS((uint8_t*)"\r\nCalibrating... KEEP BOARD STILL!\r\n", 36);
            MPU6500_Calibrate();
            CDC_Transmit_FS((uint8_t*)"Calibration complete\r\n", 21);
            break;

        default:
            CDC_Transmit_FS((uint8_t*)"\r\nUnknown command\r\n", 19);
            break;
    }
}

/* ==================== HARDWARE INITIALIZATION ==================== */

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 36;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) Error_Handler();
}

/**
  * @brief I2C2 Initialization
  */
static void MX_I2C2_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x10E32674;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK) Error_Handler();

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK) Error_Handler();
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) Error_Handler();
}

/**
  * @brief TIM2 Initialization (from CubeMX generated code)
  */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 1439;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) Error_Handler();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) Error_Handler();

    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) Error_Handler();

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) Error_Handler();

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) Error_Handler();
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) Error_Handler();
}

/**
  * @brief GPIO Initialization
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* I2C2 SCL on PA8 */
    GPIO_InitStruct.Pin = I2C2_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(I2C2_SCL_PORT, &GPIO_InitStruct);

    /* I2C2 SDA on PC4 */
    GPIO_InitStruct.Pin = I2C2_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(I2C2_SDA_PORT, &GPIO_InitStruct);

    /* TIM2 PWM Pins: PA0, PA1, PA2, PA3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    while (1) { }
}