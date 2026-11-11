# Project IGNIS — Communication & Telemetry Protocol Specification

**Document Code:** `IGN-DOC-COM-03`  
**Interfaces:** Full-Speed USB CDC (`12 Mbps`), `SX1278` 433 MHz LoRa (`SPI1`), `NEO-7M` GPS (`USART1`), `I2C2` Sensor Bus

---

## 1. USB-C Full-Speed CDC Interface (`PA11` / `PA12`)

The `IGNIS_CORE` flight computer enumerates as a standard USB Communications Device Class (CDC) Virtual COM Port (`/dev/ttyACM0` on Linux, `COMx` on Windows). Implemented in [`Modules/USB/USB.c`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/USB/USB.c).

### 1.1 `100 Hz` Real-Time Attitude & Fin Telemetry Stream
Every $10\text{ ms}$ ($100\text{ Hz}$), [`Flight_Update()`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/Flight/Flight.c) invokes `USB_SendTelemetry()`, transmitting a `\r\n`-terminated 11-column CSV frame:

```csv
Pitch,Roll,YawRate,PitchCmd,RollCmd,YawCmd,Fin1,Fin2,Fin3,Fin4,Armed
0.14,-0.08,0.02,-0.17,0.10,-0.01,90,90,90,90,0
```

| Field Index | Name | Format | Units / Range | Description |
| :---: | :--- | :---: | :---: | :--- |
| `0` | **`Pitch`** | `%.2f` | Degrees ($-90^\circ\text{ to }+90^\circ$) | Complementary-filtered pitch angle ($\alpha = 0.98$) |
| `1` | **`Roll`** | `%.2f` | Degrees ($-180^\circ\text{ to }+180^\circ$) | Complementary-filtered roll angle ($\alpha = 0.98$) |
| `2` | **`YawRate`** | `%.2f` | $\text{deg/s}$ ($\pm 1000^\circ/\text{s}$) | Bias-compensated Z-axis angular rate ($\omega_z$) |
| `3` | **`PitchCmd`** | `%.2f` | $[-100.0, +100.0]$ | Pitch PID controller command output |
| `4` | **`RollCmd`** | `%.2f` | $[-100.0, +100.0]$ | Roll PID controller command output |
| `5` | **`YawCmd`** | `%.2f` | $[-100.0, +100.0]$ | Yaw rate damping PID output |
| `6` | **`Fin1`** | `%d` | $[0^\circ, 180^\circ]$ ($90^\circ$ center) | **Fin 1 (45° / Q1: +X,+Y)** servo angle (`PA0` / `TIM2_CH1`) |
| `7` | **`Fin2`** | `%d` | $[0^\circ, 180^\circ]$ ($90^\circ$ center) | **Fin 2 (135° / Q2: -X,+Y)** servo angle (`PA1` / `TIM2_CH2`) |
| `8` | **`Fin3`** | `%d` | $[0^\circ, 180^\circ]$ ($90^\circ$ center) | **Fin 3 (225° / Q3: -X,-Y)** servo angle (`PA2` / `TIM2_CH3`) |
| `9` | **`Fin4`** | `%d` | $[0^\circ, 180^\circ]$ ($90^\circ$ center) | **Fin 4 (315° / Q4: +X,-Y)** servo angle (`PA3` / `TIM2_CH4`) |
| `10` | **`Armed`** | `%d` | `0` or `1` | `0` = Disarmed (fins locked at $90^\circ$), `1` = Armed (active control) |

### 1.2 Interactive Single-Character USB Command Console
Send any of the following ASCII commands over the USB serial terminal (`USB_ProcessCommands()`):

| Command | Action Performed | `WS2812B` LED State (`PB9`) | Serial Console Response |
| :---: | :--- | :--- | :--- |
| **`A`** or **`a`** | **Arm Flight Loop**: Enables PID integrators and active 4-fin X-mixing | **Solid Red** `(255, 0, 0)` | `>>> ARMED <<<` |
| **`D`** or **`d`** | **Disarm Flight Loop**: Zeroes PID integrators and centers all 4 fins to $90^\circ$ ($1500\,\mu\text{s}$) | **Solid Green** `(0, 255, 0)` | `>>> DISARMED <<<` |
| **`C`** or **`c`** | **Re-Calibrate IMU**: Captures 500 stationary `MPU-6500` samples ($1.0\text{ s}$) to recompute bias offsets | **Amber** `(255, 180, 0)` during cal | `Calibrating... KEEP BOARD STILL!` $\rightarrow$ `Offsets: A(...) G(...)` |
| **`S`** or **`s`** | **Bus & Sensor Diagnostic**: Runs full `I2C2` bus scan (`0x08–0x77`) and prints `MPU6500`, `HMC5883L`, and `MS5611` readings | Unchanged | `=== SENSOR DIAGNOSTIC ===` + device list & live readings |

---

## 2. Ground Station (`IGNIS-NET-SIM`) Telemetry CSV Schema

The `IGNIS-NET-SIM` desktop ground station (`IGNIS_NET/IGNIS-NET-SIM/src/data/data_loader.py`) ingests 17-column telemetry logs from LoRa downlink or SD blackbox files:

```csv
timestamp,altitude,velocity,pitch,roll,yaw,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,lat,lon,gps_alt,pressure,temperature
```

| Column | Units | Source Sensor | Description |
| :--- | :---: | :--- | :--- |
| `timestamp` | $\text{s}$ | `HAL_GetTick()` / `DS1337S+` | Mission elapsed time in seconds |
| `altitude` | $\text{m}$ AGL | `MS5611-01BA03` | Barometric altitude above pad reference pressure $P_0$ |
| `velocity` | $\text{m/s}$ | Baro + Accel Z | Vertical ascent/descent velocity |
| `pitch`, `roll`, `yaw` | $\text{deg}$ | `MPU-6500` + `HMC5883L` | 3-axis vehicle Euler orientation |
| `accel_x`, `accel_y`, `accel_z` | $\text{m/s}^2$ | `MPU-6500` | Calibrated 3-axis linear acceleration |
| `gyro_x`, `gyro_y`, `gyro_z` | $\text{deg/s}$ | `MPU-6500` | Calibrated 3-axis angular velocity |
| `lat`, `lon`, `gps_alt` | $\text{deg}$, $\text{m}$ | `u-blox NEO-7M` | WGS-84 GPS latitude, longitude, and MSL altitude |
| `pressure`, `temperature` | $\text{hPa}$, $^\circ\text{C}$ | `MS5611-01BA03` | Second-order temperature-compensated pressure & die temperature |

---

## 3. `I2C2` Internal Register Protocols

| Device | 7-Bit Addr | Key Registers | Protocol Summary |
| :--- | :---: | :--- | :--- |
| **`MPU-6500`** | `0x68` | `WHO_AM_I` (`0x75` = `0x70`), `PWR_MGMT_1` (`0x6B`), `ACCEL_XOUT_H` (`0x3B`) | 14-byte burst read from `0x3B` (`Accel X/Y/Z`, `Temp`, `Gyro X/Y/Z`) |
| **`MS5611-01BA03`** | `0x77` | `RESET` (`0x1E`), `CONV_D1_4096` (`0x48`), `CONV_D2_4096` (`0x58`), `PROM` (`0xA2–0xAC`) | 10 ms $\text{OSR}=4096$ conversion followed by 24-bit `ADC_READ` (`0x00`) |
| **`HMC5883L`** | `0x1E` | `CONFIG_A` (`0x00` = `0x70`), `CONFIG_B` (`0x01` = `0x20`), `DATA_X_M` (`0x03`) | 6-byte burst read from `0x03` (**Register order is `X, Z, Y`**) |
| **`24LC16B`** | `0x50–0x57` | Control byte encodes 256-byte block (`0x50 | ((addr >> 8) & 0x07)`) | 16-byte page write buffer with $5\text{ ms}$ internal write-cycle polling |
