#ifndef EEPROM_H
#define EEPROM_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Microchip 24LC16B (16 Kbit = 2048 Bytes) I2C EEPROM Driver
 * - 8 blocks of 256 bytes each at I2C 7-bit addresses 0x50 .. 0x57
 * - 8-bit internal word address per block
 * - 16-byte page write buffer (5 ms max write cycle time)
 * - Pin 7 (WP) must be tied to GND to enable writes
 * ============================================================================ */
#define EEPROM_24LC16B_BASE_ADDR   0x50U
#define EEPROM_24LC16B_TOTAL_SIZE  2048U
#define EEPROM_24LC16B_PAGE_SIZE   16U
#define EEPROM_HOTSTART_MAGIC      0x49474E53UL  /* "IGNS" */
#define EEPROM_HOTSTART_ADDR       0x0000U       /* Block 0, offset 0 */

typedef struct {
    uint32_t magic;            /* EEPROM_HOTSTART_MAGIC when valid */
    uint32_t boot_count;       /* Incremented on each boot */
    uint8_t  flight_state_id;  /* 0=PAD/IDLE, 1=ARMED, 2=BOOST, 3=COAST, 4=DESCENT, 5=LANDED */
    uint8_t  armed;            /* Armed flag across brownout reset */
    uint16_t reserved;
    int32_t  ground_press_pa;  /* Baseline pad pressure (Pa) */
    int32_t  max_altitude_cm;  /* Peak altitude recorded (cm) */
    int16_t  accel_offset[3];  /* Saved IMU accelerometer calibration */
    int16_t  gyro_offset[3];   /* Saved IMU gyroscope calibration */
    float    last_pitch;       /* Last filtered pitch before reset */
    float    last_roll;        /* Last filtered roll before reset */
    uint32_t checksum;         /* XOR/additive checksum */
} HotStartData_t;

void EEPROM_Init(void);
uint8_t EEPROM_IsConnected(void);

HAL_StatusTypeDef EEPROM_WriteByte(uint16_t mem_addr, uint8_t data);
HAL_StatusTypeDef EEPROM_ReadByte(uint16_t mem_addr, uint8_t *data);

HAL_StatusTypeDef EEPROM_WriteBuffer(uint16_t mem_addr, const uint8_t *buf, uint16_t len);
HAL_StatusTypeDef EEPROM_ReadBuffer(uint16_t mem_addr, uint8_t *buf, uint16_t len);

HAL_StatusTypeDef EEPROM_SaveHotStart(const HotStartData_t *hs);
uint8_t EEPROM_LoadHotStart(HotStartData_t *hs);
HAL_StatusTypeDef EEPROM_ClearHotStart(void);

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_H */
