# Project IGNIS — System Overview & Avionics Architecture

**Document Code:** `IGN-DOC-SYS-01`  
**Target Platform:** `IGNIS_CORE` (`STM32G473VET6` LQFP-100) + `Aux Blackbox` (`ATmega328P-AU`)

---

## 1. Architectural Philosophy

**Project IGNIS** is a modular, fault-tolerant model rocket guidance, navigation, control (GNC), and recovery avionics suite. The architecture separates flight-critical attitude stabilization and pyrotechnic staging (`IGNIS_CORE`) from redundant non-volatile flight recording (`Aux Hardware Data Logger`), mechanical thrust/fin actuation (`IGNIS_CTRL` & `Rocket_Assembly`), and ground telemetry visualization (`IGNIS_NET`).

### Why `STM32G473VET6` Instead of `STM32F405` / `STM32F411`?

During the architecture trade study (transitioning from an early `STM32F411` prototype to the production `IGNIS_CORE` flight computer), the **`STM32G473VET6`** (170 MHz ARM Cortex-M4F, 512 KB ECC Flash, 128 KB SRAM, LQFP-100) was selected for five rocketry-specific hardware advantages:

1. **Hardware CORDIC & FMAC Co-Processors**:
   - **FMAC (Filter Math Accelerator)**: Offloads high-rate IIR biquad notch and low-pass filtering of raw `MPU-6500` accelerometer/gyroscope samples via DMA with zero CPU cycle overhead, rejecting high-frequency solid rocket motor structural resonance before attitude integration.
   - **CORDIC Co-Processor**: Hardware-accelerated $\sin$, $\cos$, $\text{atan2}$, and $\sqrt{x}$ units accelerate Euler/quaternion attitude updates and apogee estimation.
2. **Integrated Mixed-Signal Analog Front-End for Pyro & Battery Diagnostics**:
   - Includes **6 internal Rail-to-Rail Programmable Gain Op-Amps (PGA)**, **7 ultra-fast 16 ns Analog Comparators**, and **5× 4 MSPS 12-bit ADCs** (up to 16-bit via hardware oversampling).
   - Enables hardware e-match continuity sensing and `<20 ns` comparator-driven MOSFET overcurrent shutoff without external comparator ICs.
3. **Capacitor-Less Internal `VCORE` Regulator & Built-In `VREFBUF`**:
   - Unlike `STM32F405` (which requires external `2.2 µF` `VCAP_1` / `VCAP_2` capacitors), the `STM32G473VET6` `1.28 V` core regulator is fully internal, and Pin 21 (`VREF+`) provides a programmable internal precision voltage reference (`2.048 V` / `2.5 V` / `2.9 V`).
4. **ECC Flash & SRAM Parity for High-Vibration Flight Reliability**:
   - 512 KB dual-bank Flash with hardware single-error correction / double-error detection (SECDED ECC) and 32 KB CCM-SRAM with hardware parity check.
5. **100-Pin LQFP Package with Contiguous Peripheral Routing**:
   - Dedicates separate hardware buses to every subsystem (`I2C2` for sensors/EEPROM, `SPI1` for `SX1278` LoRa on consecutive pins `90–95`, `SPI2` for `microSD` on pins `51–54`, `USART1` for `NEO-7M` GPS on pins `70–71`, `TIM2` 4-channel PWM on pins `22–25`, and `USB_OTG_FS` on pins `72–73`).

---

## 2. Subsystem Breakdown

### 2.1 `IGNIS_CORE` Main Flight Computer PCB
- **Path**: `IGNIS_CORE/Hardware/Project_IGNIS_CORE/`
- **Microcontroller**: `U3` (`STM32G473VET6`), clocked from an `8.000 MHz` HSE crystal (`X1`) multiplied by the internal PLL ($\times 36 / 2$) to **`144 MHz` `SYSCLK`**, with a `32.768 kHz` LSE crystal (`X2`) for real-time timekeeping.
- **Sensor Suite (`I2C2` Bus — `PC4` SCL, `PA8` SDA)**:
  - **`U7` (`MPU-6500` — `0x68`)**: 6-axis MEMS IMU configured for $\pm 8\text{g}$ full-scale acceleration ($4096\text{ LSB/g}$) and $\pm 1000^\circ/\text{s}$ angular rate ($32.8\text{ LSB}/(^\circ/\text{s})$).
  - **`U8` (`MS5611-01BA03` — `0x77`)**: 24-bit $\Delta\Sigma$ barometric pressure sensor operating at $\text{OSR} = 4096$ with factory PROM coefficients `C[1..6]` and second-order low-temperature compensation.
  - **`U9` (`BMM150` / `HMC5883L` — `0x1E`)**: 3-axis digital compass for pad azimuth and roll-stabilized heading reference ($\pm 1.3\text{ Gauss}$, $1090\text{ LSB/Gauss}$).
  - **`24LC16B` (`0x50–0x57`)**: 16-Kbit (2048-byte) I2C Serial EEPROM storing calibrated sensor bias offsets, flight state, and pad altitude across power cycles.
- **Radio & Navigation**:
  - **`U10` (`Ai-Thinker Ra-02 / SX1278`)**: 433 MHz LoRa spread-spectrum transceiver on `SPI1` (`PB3–PB8`).
  - **`u-blox NEO-7M` GPS**: Connected to `USART1` (`PA9` TX, `PA10` RX) for 3D position and recovery coordinates.
- **Pyrotechnic Ignition & Actuation**:
  - **4× Fin / TVC Servo Outputs (`TIM2_CH1..4`, `PA0–PA3`)**: 50 Hz hardware PWM channels driving four active aerodynamic fins in an X-configuration (or 2-axis TVC gimbal).
  - **3× High-Current Pyro Channels (`Q1–Q3`, `PC7–PC9`)**:
    - `PYRO_MOTOR` (`PC7`) & `PYRO_SEP` (`PC8`): Direct logic-level `IRLZ44N` N-MOSFET low-side switches.
    - `PYRO_CHUTE` (`PC9`): `IRF520` N-MOSFET driven via a non-inverting dual-`BC547` NPN level-shifter pulling the gate to `+V_BAT` (`7.4V–12V`), with `1N4007` flyback diodes (`D1–D3`) across all igniter loads.

### 2.2 Auxiliary Blackbox Data Logger PCB
- **Path**: `IGNIS_CORE/Aux Hardware/Data Logger/data logger/`
- **Architecture**: Self-contained secondary flight recorder board designed to survive main battery brownouts:
  - **Controller**: `U4` (`ATmega328P-AU`, clocked via `16 MHz` crystal `Y2`).
  - **Memory Array**: Dual `24LC1025` (`U1`, `U2`) I2C CMOS Serial EEPROMs providing $2 \times 1\text{ Mbit} = 256\text{ KB}$ of non-volatile page-write logging storage.
  - **Timebase**: `U3` (`DS1337S+` Real-Time Clock with `32.768 kHz` crystal `Y1` and backup coin-cell holder `BAT1`).

### 2.3 Firmware Modular Architecture (`Modules/`)
The `STM32G473VET6` firmware (`IGNIS_CORE/Firmware/HAL STM32/IGNIS_CORE_STM32G473VET6/`) is structured so that STM32CubeMX can regenerate `.ioc` peripheral initialization code at any time without touching application logic:

| Module | Source Files | Responsibility |
| :--- | :--- | :--- |
| **`Flight`** | `Modules/Flight/Flight.c`, `.h` | `100 Hz` attitude control loop, complementary filter ($\alpha=0.98$), 3-axis PID, and 4-fin X-mixing |
| **`IMU`** | `Modules/IMU/IMU.c`, `.h` | `MPU-6500` register config, burst 14-byte `I2C2` read, and 500-sample startup offset calibration |
| **`Barometer`** | `Modules/Barometer/Barometer.c`, `.h` | `MS5611` PROM `C[1..6]` readout, $\text{OSR}=4096$ `D1`/`D2` conversions, and 64-bit integer pressure math |
| **`Sensors`** | `Modules/Sensors/Sensors.c`, `.h` | Automated `I2C2` bus scanner (`0x08–0x77`) and `HMC5883L` 3-axis magnetometer driver |
| **`Servos`** | `Modules/Servos/Servos.c`, `.h` | `TIM2` Channels 1–4 50 Hz PWM output generation (`ANGLE_TO_PULSE` mapping $0^\circ\text{–}180^\circ \rightarrow 1000\text{–}2000\,\mu\text{s}$) |
| **`WS2812`** | `Modules/WS2812/WS2812.c`, `.h` | `TIM4_CH4` (`PB9`) + `DMA1_Channel1` 800 kHz bitstream generator with preamble & active-low latch fix |
| **`USB`** | `Modules/USB/USB.c`, `.h` | Full-Speed USB CDC virtual serial port streaming `100 Hz` CSV telemetry & processing `A`/`D`/`C`/`S` commands |
| **`EEPROM`** | `Modules/EEPROM/EEPROM.c`, `.h` | `24LC16B` (`0x50–0x57`) multi-block read/write driver for hot-start state and sensor calibration persistence |
| **`LoRa`** | `Modules/LoRa/LoRa.c`, `.h` | `SX1278` (`Ra-02`) `SPI1` packet radio interface (`PB3–PB8`) |
| **`GPS`** | `Modules/GPS/GPS.c`, `.h` | `u-blox NEO-7M` `USART1` (`PA9`/`PA10`) receiver interface |
| **`Logger` / `Storage`** | `Modules/Logger/`, `Modules/Storage/` | High-rate flight data formatting and `SPI2` (`PB12–PB15`) microSD card storage |

---

## 3. Flight State Machine & Visual Diagnostics

| State | `WS2812B` RGB Status (`PB9`) | Servo Behaviour (`PA0–PA3`) | PID Integrators | Entry Trigger |
| :--- | :--- | :--- | :--- | :--- |
| **1. `BOOT_CALIBRATE`** | **Amber / Yellow** `(255, 180, 0)` | Centered at $90^\circ$ ($1500\,\mu\text{s}$) | Reset (`0.0f`) | Power-on reset or USB `'C'` command (500 IMU samples over 1.0 s) |
| **2. `DISARMED_READY`** | **Solid Green** `(0, 255, 0)` | Locked at center $90^\circ$ ($1500\,\mu\text{s}$) | Held at `0.0f` | Boot calibration complete or USB `'D'` command |
| **3. `ARMED_ACTIVE`** | **Solid Red** `(255, 0, 0)` | Active `100 Hz` X-fin / TVC deflection | Active ($\pm 100$ clamp) | USB `'A'` command or pad launch umbilical arming |

---

## 4. Power Distribution Architecture

```
[2S/3S LiPo Battery (VIN)] ──► [U4: LM2596 Step-Down Buck] ──┬──► [U2: AMS1117-5.0] ──► +5.0V Servo & Buzzer Rail
                                                             │
[USB-C VBUS (+5.0V)] ──► [U5/U6: 1N5819 Schottky OR-ing] ────┴──► [U1: AMS1117-3.3] ──► +3.3V MCU, IMU, Baro, Mag, LoRa, EEPROM
```
- **Schottky Diode Isolation (`1N5819`)**: Prevents reverse current from the battery regulator into the host PC's USB-C port during bench testing with live servo/pyro power connected.
- **Decoupling Array**: 22× `100 nF` (`0805`) ceramic decoupling capacitors (`C1–C23`) distributed across `STM32G473VET6` `VDD`/`VDDA` pins and sensor supplies.
