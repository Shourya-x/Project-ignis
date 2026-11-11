#include "USB.h"
#include "usbd_cdc_if.h"
#include "Flight.h"
#include "IMU.h"
#include "Barometer.h"
#include "Sensors.h"
#include "WS2812.h"
#include <stdio.h>
#include <string.h>

uint8_t usb_rx_buffer[64];
volatile uint8_t usb_rx_len = 0;

void USB_Init(void)
{
    usb_rx_len = 0;
}

void USB_Print(const char *msg)
{
    if (msg == NULL) return;
    uint16_t len = (uint16_t)strlen(msg);
    if (len == 0) return;
    CDC_Transmit_FS((uint8_t *)msg, len);
}

void USB_SendTelemetry(void)
{
    char buf[256];
    snprintf(buf, sizeof(buf),
             "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d\r\n",
             flight_state.filtered_pitch,
             flight_state.filtered_roll,
             flight_state.yaw_rate,
             flight_state.pitch_cmd,
             flight_state.roll_cmd,
             flight_state.yaw_cmd,
             flight_state.fin1_angle,
             flight_state.fin2_angle,
             flight_state.fin3_angle,
             flight_state.fin4_angle,
             flight_state.armed);
    USB_Print(buf);
}

void USB_ProcessCommands(void)
{
    if (usb_rx_len == 0) return;

    char cmd = (char)usb_rx_buffer[0];
    usb_rx_len = 0;

    switch (cmd)
    {
        case 'A':
        case 'a':
            Flight_Arm();
            USB_Print("\r\n>>> ARMED <<<\r\n");
            break;

        case 'D':
        case 'd':
            Flight_Disarm();
            USB_Print("\r\n>>> DISARMED <<<\r\n");
            break;

        case 'C':
        case 'c':
            WS2812_SetRGB(255, 180, 0);
            USB_Print("\r\nCalibrating... KEEP BOARD STILL!\r\n");
            IMU_Calibrate();
            WS2812_SetRGB(flight_state.armed ? 255 : 0, flight_state.armed ? 0 : 255, 0);
            USB_Print("Calibration complete\r\n");
            break;

        case 'S':
        case 's':
        {
            char msg[128];
            int16_t mx, my, mz;
            int32_t temp, pres;
            USB_Print("\r\n=== SENSOR DIAGNOSTIC ===\r\n");
            I2C2_ScanBus();
            HMC5883L_ReadRaw(&mx, &my, &mz);
            Barometer_Read(&temp, &pres);
            snprintf(msg, sizeof(msg),
                     "MPU6500 WHO_AM_I: 0x%02X | HMC5883L: X=%d Y=%d Z=%d (%.1f deg) | MS5611: T=%.2fC P=%ldPa\r\n",
                     IMU_ReadByte(MPU6500_WHO_AM_I),
                     mx, my, mz, HMC5883L_GetHeadingDeg(),
                     temp / 100.0f, (long)pres);
            USB_Print(msg);
            break;
        }

        default:
            break;
    }
}
