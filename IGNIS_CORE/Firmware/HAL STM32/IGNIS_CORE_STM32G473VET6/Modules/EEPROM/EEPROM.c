#include "EEPROM.h"
#include "i2c.h"
#include "USB.h"
#include <string.h>
#include <stdio.h>

static uint32_t EEPROM_CalcChecksum(const HotStartData_t *hs)
{
    const uint8_t *bytes = (const uint8_t *)hs;
    uint32_t sum = 0xA5A55A5AUL;
    /* Checksum over all bytes before the checksum field */
    size_t len = sizeof(HotStartData_t) - sizeof(uint32_t);
    for (size_t i = 0; i < len; i++)
    {
        sum = (sum << 5) + sum + bytes[i];
    }
    return sum;
}

void EEPROM_Init(void)
{
    if (EEPROM_IsConnected())
    {
        USB_Print("24LC16B EEPROM detected at 0x50..0x57 (2048B)\r\n");
    }
    else
    {
        USB_Print("24LC16B EEPROM not detected on I2C2\r\n");
    }
}

uint8_t EEPROM_IsConnected(void)
{
    return (HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(EEPROM_24LC16B_BASE_ADDR << 1), 2, 10) == HAL_OK) ? 1U : 0U;
}

HAL_StatusTypeDef EEPROM_WriteByte(uint16_t mem_addr, uint8_t data)
{
    if (mem_addr >= EEPROM_24LC16B_TOTAL_SIZE) return HAL_ERROR;

    /* Upper 3 bits of 11-bit address select block 0..7 (0x50..0x57) */
    uint8_t dev_addr  = (uint8_t)(EEPROM_24LC16B_BASE_ADDR | ((mem_addr >> 8) & 0x07U));
    uint8_t word_addr = (uint8_t)(mem_addr & 0xFFU);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
        &hi2c2,
        (uint16_t)(dev_addr << 1),
        word_addr,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        50
    );

    if (status == HAL_OK)
    {
        /* Poll ACK for write-cycle completion (typically 3-5 ms) */
        HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(dev_addr << 1), 10, 10);
    }
    return status;
}

HAL_StatusTypeDef EEPROM_ReadByte(uint16_t mem_addr, uint8_t *data)
{
    if (mem_addr >= EEPROM_24LC16B_TOTAL_SIZE || data == NULL) return HAL_ERROR;

    uint8_t dev_addr  = (uint8_t)(EEPROM_24LC16B_BASE_ADDR | ((mem_addr >> 8) & 0x07U));
    uint8_t word_addr = (uint8_t)(mem_addr & 0xFFU);

    return HAL_I2C_Mem_Read(
        &hi2c2,
        (uint16_t)(dev_addr << 1),
        word_addr,
        I2C_MEMADD_SIZE_8BIT,
        data,
        1,
        50
    );
}

HAL_StatusTypeDef EEPROM_WriteBuffer(uint16_t mem_addr, const uint8_t *buf, uint16_t len)
{
    if (buf == NULL || (mem_addr + len) > EEPROM_24LC16B_TOTAL_SIZE) return HAL_ERROR;

    while (len > 0)
    {
        uint8_t dev_addr  = (uint8_t)(EEPROM_24LC16B_BASE_ADDR | ((mem_addr >> 8) & 0x07U));
        uint8_t word_addr = (uint8_t)(mem_addr & 0xFFU);

        /* Respect 16-byte page boundary inside 24LC16B */
        uint16_t page_offset = mem_addr % EEPROM_24LC16B_PAGE_SIZE;
        uint16_t chunk       = EEPROM_24LC16B_PAGE_SIZE - page_offset;
        if (chunk > len) chunk = len;

        HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
            &hi2c2,
            (uint16_t)(dev_addr << 1),
            word_addr,
            I2C_MEMADD_SIZE_8BIT,
            (uint8_t *)buf,
            chunk,
            50
        );
        if (status != HAL_OK) return status;

        /* Wait for internal EEPROM page write cycle to complete (ACK polling) */
        if (HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(dev_addr << 1), 10, 10) != HAL_OK)
        {
            HAL_Delay(5);
        }

        mem_addr += chunk;
        buf      += chunk;
        len      -= chunk;
    }

    return HAL_OK;
}

HAL_StatusTypeDef EEPROM_ReadBuffer(uint16_t mem_addr, uint8_t *buf, uint16_t len)
{
    if (buf == NULL || (mem_addr + len) > EEPROM_24LC16B_TOTAL_SIZE) return HAL_ERROR;

    while (len > 0)
    {
        uint8_t dev_addr  = (uint8_t)(EEPROM_24LC16B_BASE_ADDR | ((mem_addr >> 8) & 0x07U));
        uint8_t word_addr = (uint8_t)(mem_addr & 0xFFU);

        /* Respect 256-byte block boundary */
        uint16_t block_rem = 256U - word_addr;
        uint16_t chunk     = (len < block_rem) ? len : block_rem;

        HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
            &hi2c2,
            (uint16_t)(dev_addr << 1),
            word_addr,
            I2C_MEMADD_SIZE_8BIT,
            buf,
            chunk,
            50
        );
        if (status != HAL_OK) return status;

        mem_addr += chunk;
        buf      += chunk;
        len      -= chunk;
    }

    return HAL_OK;
}

HAL_StatusTypeDef EEPROM_SaveHotStart(const HotStartData_t *hs)
{
    if (hs == NULL) return HAL_ERROR;

    HotStartData_t copy = *hs;
    copy.magic    = EEPROM_HOTSTART_MAGIC;
    copy.checksum = EEPROM_CalcChecksum(&copy);

    return EEPROM_WriteBuffer(EEPROM_HOTSTART_ADDR, (const uint8_t *)&copy, sizeof(HotStartData_t));
}

uint8_t EEPROM_LoadHotStart(HotStartData_t *hs)
{
    if (hs == NULL) return 0U;

    HotStartData_t temp;
    if (EEPROM_ReadBuffer(EEPROM_HOTSTART_ADDR, (uint8_t *)&temp, sizeof(HotStartData_t)) != HAL_OK)
    {
        return 0U;
    }

    if (temp.magic != EEPROM_HOTSTART_MAGIC)
    {
        return 0U;
    }

    if (temp.checksum != EEPROM_CalcChecksum(&temp))
    {
        return 0U;
    }

    *hs = temp;
    return 1U;
}

HAL_StatusTypeDef EEPROM_ClearHotStart(void)
{
    uint32_t zero = 0;
    return EEPROM_WriteBuffer(EEPROM_HOTSTART_ADDR, (const uint8_t *)&zero, sizeof(zero));
}
