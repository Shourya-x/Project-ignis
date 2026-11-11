# IGNIS-NET — Troubleshooting Guide

## 1. No LoRa Packets Received at Ground Station
- **Frequency / Spreading Factor Mismatch**: Confirm both `IGNIS_CORE` (`SPI1` `Ra-02`) and the `IGNIS-NET` receiver are configured for `433E6` Hz (`433 MHz`), matching spreading factor (`SF7–SF10`), bandwidth (`125 kHz`), and sync word (`0x12`).
- **SPI Chip Select & Reset**: On `IGNIS_CORE` (`STM32G473VET6`), verify `LORA_SCK = PB3`, `LORA_MISO = PB4`, `LORA_MOSI = PB5`, `LORA_RST = PB6`, `LORA_CS = PB7` (or `PA15`), and `LORA_DIO0 = PB8`.

## 2. `IGNIS-NET-SIM` GUI Fails to Launch
- **Missing Virtual Environment Packages**: Install `PyQt6`, `pyqtgraph`, `PyOpenGL`, `numpy`, `pandas`, and `pyserial`:
  ```bash
  pip install -r IGNIS_NET/IGNIS-NET-SIM/requirements.txt
  ```
- **CSV Column Validation Error**: Ensure custom CSV flight logs include all 17 required columns (`timestamp,altitude,velocity,pitch,roll,yaw,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,lat,lon,gps_alt,pressure,temperature`).
