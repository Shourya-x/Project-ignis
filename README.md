# Project IGNIS — STM32G473VET6 Rocket Flight Computer & Active Fin / TVC Avionics Suite

[![MCU: STM32G473VET6](https://img.shields.io/badge/MCU-STM32G473VET6%20(170MHz%20Cortex--M4F)-03234B?style=flat-square)](IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6)
[![Control Loop: 100 Hz](https://img.shields.io/badge/Control%20Loop-100%20Hz%20PID%20%2B%20X--Fin%20Mixing-d97706?style=flat-square)](IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/Flight)
[![ECAD: KiCad 9/10](https://img.shields.io/badge/ECAD-KiCad%20Custom%20PCBs-314CB0?style=flat-square)](IGNIS_CORE/Hardware/Project_IGNIS_CORE)
[![MCAD: FreeCAD](https://img.shields.io/badge/MCAD-FreeCAD%20Parametric%20Airframe-dc2626?style=flat-square)](Rocket_Assembly)
[![License: MIT / CERN-OHL](https://img.shields.io/badge/License-MIT%20%2F%20CERN--OHL-15803d?style=flat-square)](#license)

> **Open-source high-power model rocket flight computer, 4-channel X-configuration active fin / TVC attitude controller, 3-stage high-current MOSFET pyro igniter system, dual-EEPROM blackbox recorder, and 433 MHz LoRa telemetry ground station.**

---

## 1. Executive Summary

**Project IGNIS** is a vertically integrated aerospace avionics and mechanical control platform engineered from custom bare-metal `STM32G473VET6` HAL firmware and 2-layer KiCad PCBs to a parametric FreeCAD airframe and a PyQt6/OpenGL ground station.

Unlike hobby flight controllers built around legacy `STM32F4` drone boards, **IGNIS-CORE** is purpose-built around the **`STM32G473VET6`** (LQFP-100, 170 MHz ARM Cortex-M4F with hardware **CORDIC** trigonometric and **FMAC** filter co-processors, 512 KB ECC Flash, 128 KB SRAM, 6 internal PGA op-amps, and 7 ultra-fast 16 ns comparators).

### Key Subsystem Capabilities

| Subsystem | Hardware Implementation | Key Specifications |
| :--- | :--- | :--- |
| **Flight Computer (`IGNIS_CORE`)** | Custom KiCad PCB + **`STM32G473VET6`** (LQFP-100) | 144 MHz PLL (`8 MHz` HSE `X1` + `32.768 kHz` LSE `X2`), 512 KB ECC Flash, 128 KB SRAM |
| **Inertial & Barometric Suite (`I2C2`)** | **`MPU-6500`** (`0x68`) + **`MS5611-01BA03`** (`0x77`) + **`HMC5883L` / `BMM150`** (`0x1E`) | $\pm 8\text{g}$ Accel ($4096\text{ LSB/g}$), $\pm 1000^\circ/\text{s}$ Gyro ($32.8\text{ LSB}/^\circ/\text{s}$), 24-bit $\Delta\Sigma$ Baro ($\text{OSR}=4096$, $\sim 10\text{ cm}$ resolution) |
| **Attitude & X-Fin / TVC Actuation** | 4× High-Speed Digital Servos on **`TIM2_CH1..4`** (`PA0–PA3`) | `100 Hz` Complementary Filter ($\alpha=0.98$) + 3-Axis PID ($K_p=1.2, K_i=0.05, K_d=0.08$) + 4-Fin X-Mixing |
| **3-Channel Pyro Igniter Stage** | 2× **`IRLZ44N`** (`PC7`, `PC8`) + 1× **`IRF520`** w/ Dual-`BC547` Level Shifter (`PC9`) | Motor Ignition (`PC7`), Stage Separation (`PC8`), Parachute Ejection (`PC9`) with `1N4007` flyback clamp diodes |
| **Telemetry & Navigation** | **`Ai-Thinker Ra-02 (SX1278)`** (`SPI1`) + **`u-blox NEO-7M`** (`USART1`) | 433 MHz LoRa long-range link (`>5 km` LOS), contiguous LQFP-100 pin routing (`Pins 90–95`) + 10 Hz GPS |
| **Dual-Tier Flight Storage** | Onboard **`24LC16B`** EEPROM (`0x50–0x57`) + **`microSD`** (`SPI2`) + **`Aux Data Logger PCB`** | Hot-start state & calibration recovery + `ATmega328P` / Dual `24LC1025` (256 KB) / `DS1337S+` RTC blackbox |
| **Diagnostics & Ground Interface** | **`WS2812B`** DMA Engine (`PB9`) + 5V NPN Buzzer (`PC6`) + **`USB-C CDC`** (`PA11/PA12`) | Zero-CPU `TIM4_CH4` + `DMA1_Channel1` 800 kHz NeoPixel driver + 100 Hz CSV telemetry stream & CLI |

---

## 2. System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                  PROJECT IGNIS AVIONICS STACK                                   │
├─────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                                 │
│  ┌───────────────────────────────────────┐        ┌──────────────────────────────────────────┐  │
│  │        I2C2 SENSOR & NVM BUS          │        │        TELEMETRY, GPS & STORAGE          │  │
│  │        (SCL = PC4, SDA = PA8)         │        │                                          │  │
│  │  • MPU-6500 6-Axis IMU       [0x68]   │        │  • SX1278 Ra-02 LoRa (SPI1: PB3–PB8)     │  │
│  │  • MS5611-01BA03 Barometer   [0x77]   │        │  • u-blox NEO-7M GPS (USART1: PA9/PA10)  │  │
│  │  • HMC5883L / BMM150 Mag     [0x1E]   │        │  • microSD Slot      (SPI2: PB12–PB15)   │  │
│  │  • 24LC16B Hot-Start EEPROM  [0x50]   │        │  • USB-C Full-Speed  (CDC: PA11/PA12)    │  │
│  └──────────────────┬────────────────────┘        └────────────────────┬─────────────────────┘  │
│                     │                                                  │                        │
│                     ▼                                                  ▼                        │
│  ┌───────────────────────────────────────────────────────────────────────────────────────────┐  │
│  │                        IGNIS-CORE FLIGHT COMPUTER (STM32G473VET6)                         │  │
│  │            170 MHz Cortex-M4F (144 MHz PLL) • CORDIC + FMAC • 512 KB ECC Flash            │  │
│  │     100 Hz Attitude Loop: Complementary Filter (α=0.98) ──► 3-Axis PID ──► X-Fin Mixer    │  │
│  └──────────────────┬──────────────────────────────────────────────────┬─────────────────────┘  │
│                     │                                                  │                        │
│                     ▼ TIM2 50Hz PWM (PA0–PA3)                          ▼ High-Current Switching │
│  ┌───────────────────────────────────────┐        ┌──────────────────────────────────────────┐  │
│  │     IGNIS-CTRL ACTUATION STAGE        │        │      3-CHANNEL PYRO & DIAGNOSTICS        │  │
│  │  • Fin 1 (45°  / Q1: +X,+Y) : PA0 (CH1)│       │  • PC7 (PYRO_MOTOR) : IRLZ44N Logic FET  │  │
│  │  • Fin 2 (135° / Q2: -X,+Y) : PA1 (CH2)│       │  • PC8 (PYRO_SEP)   : IRLZ44N Logic FET  │  │
│  │  • Fin 3 (225° / Q3: -X,-Y) : PA2 (CH3)│       │  • PC9 (PYRO_CHUTE) : 2x BC547 + IRF520  │  │
│  │  • Fin 4 (315° / Q4: +X,-Y) : PA3 (CH4)│       │  • PC6 (BUZZER)     : BC547 + 1N4007     │  │
│  │  • 2-Axis TVC Gimbal Assembly         │        │  • PB9 (WS2812B)    : TIM4_CH4 + DMA     │  │
│  └───────────────────────────────────────┘        └──────────────────────────────────────────┘  │
│                                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Repository Structure

```text
Project-ignis/
├── IGNIS_CORE/
│   ├── Firmware/
│   │   ├── HAL STM32/IGNIS_CORE_STM32G473VET6/  # Primary STM32CubeMX + CMake + Modular HAL Firmware
│   │   │   ├── Core/                            # CubeMX-generated peripheral init (gpio, dma, i2c, spi, tim, usart)
│   │   │   ├── Modules/                         # 11 CubeMX-safe hardware & GNC modules:
│   │   │   │   ├── Flight/                      # 100 Hz Complementary filter, 3-axis PID & X-fin mixing
│   │   │   │   ├── IMU/                         # MPU-6500 register driver & 500-sample offset calibration
│   │   │   │   ├── Barometer/                   # MS5611 PROM C1-C6 reader & 2nd-order temp-compensated ADC
│   │   │   │   ├── Sensors/                     # I2C2 bus scanner (0x08-0x77) & HMC5883L 3-axis magnetometer
│   │   │   │   ├── Servos/                      # TIM2 4-channel 50 Hz hardware PWM (1000-2000 µs)
│   │   │   │   ├── WS2812/                      # TIM4_CH4 + DMA1_Channel1 zero-CPU NeoPixel pulse engine
│   │   │   │   ├── EEPROM/                      # 24LC16B 8-block (0x50-0x57) hot-start & calibration storage
│   │   │   │   ├── LoRa/                        # SX1278 (Ra-02) SPI1 radio driver
│   │   │   │   ├── GPS/                         # u-blox NEO-7M USART1 NMEA parser
│   │   │   │   ├── Logger/ & Storage/           # SPI2 microSD & blackbox telemetry logging
│   │   │   │   └── USB/                         # USB-C Full-Speed CDC 100 Hz CSV stream & interactive CLI
│   │   │   ├── IGNIS_CORE_STM32G473VET6.ioc     # STM32CubeMX hardware pinout & clock definition
│   │   │   ├── CMakeLists.txt                   # CMake + Ninja build definition
│   │   │   └── new_module.sh                    # Automated HAL module generator script
│   │   └── PlatFormio/                          # Legacy prototype firmware archive
│   ├── Hardware/
│   │   ├── Project_IGNIS_CORE/                  # Main Flight Computer KiCad 9/10 Schematic, PCB & STEP
│   │   ├── 3D_Models/ & Footprints.pretty/      # Custom SMD/THT footprints & 3D component models
│   │   └── Documentation/                       # Exported schematics.pdf & BOM.xlsx
│   └── Aux Hardware/Data Logger/data logger/    # Standalone ATmega328P + 2x 24LC1025 + DS1337S RTC PCB
│
├── IGNIS_CTRL/
│   └── Mechanical_Design/                       # TVC gimbal STEP/STL CAD files (thrust plate, linkages, mounts)
│
├── IGNIS_NET/
│   ├── IGNIS-NET-SIM/                           # PyQt6 + PyQtGraph + PyOpenGL Ground Station & Flight Replayer
│   ├── Firmware/                                # Ground station receiver firmware
│   └── Documentation/                           # Ground station user manual & API docs
│
├── Rocket_Assembly/
│   ├── Airframe And Parts/                      # Parametric FreeCAD (.FCStd), STEP & STL rocket body, fins, nosecone
│   ├── Avionics_Bay/                            # Modular avionics bay (.FCStd, .gltf, .stl), shock mounts & core mount
│   └── Full_Assembly/                           # Complete rocket assembly & cutaway STEP models
│
├── Documentation/                               # Comprehensive engineering & hardware documentation
│   ├── System_Overview.md                       # Architecture, G4 vs F4 trade study, control loop & power tree
│   ├── Wiring_Guide.md                          # Complete LQFP-100 pinout, Pyro MOSFET & Buzzer schematics
│   ├── Communication_Protocol.md                # USB CDC 100 Hz CSV, CLI commands & LoRa telemetry frame specs
│   ├── Calibration_Guide.md                     # IMU 500-sample bias, MS5611 PROM, Magnetometer & Servo centering
│   └── Troubleshooting.md                       # Hardware bringup, WS2812 DMA timing, I2C2 mux & PCB DRC notes
│
└── ignis-website/                               # Astro Technical Datasheet & 3D Interactive Avionics Showcase
```

---

## 4. Complete `STM32G473VET6` (LQFP-100) Pinout Matrix

| LQFP-100 Pin | GPIO Pin | Peripheral / Mode | Signal Label | Connected Hardware & Engineering Notes |
| :---: | :---: | :--- | :--- | :--- |
| **Pin 12 / 13** | `PH0 / PH1` | `RCC_OSC_IN / OUT` | `HSE_8MHZ` | `X1` 8.000 MHz HC-49US crystal $\rightarrow$ PLL $\times 36 / 2 = \mathbf{144\text{ MHz}}$ `SYSCLK` |
| **Pin 8 / 9** | `PC14 / PC15` | `RCC_OSC32_IN / OUT` | `LSE_32K` | `X2` 32.768 kHz RTC crystal |
| **Pin 30** | **`PC4`** | `I2C2_SCL` (AF4) | `I2C2_SCL` | `MPU-6500` (`0x68`), `MS5611` (`0x77`), `HMC5883L` (`0x1E`), `24LC16B` (`0x50–0x57`) |
| **Pin 69** | **`PA8`** | `I2C2_SDA` (AF4) | `I2C2_SDA` | Shared `I2C2` bus (`4.7 kΩ` pull-ups to `+3.3V`) |
| **Pin 22** | **`PA0`** | `TIM2_CH1` (AF1) | `SERVO_FIN1` | **Fin 1 (45° / Q1: +X, +Y)** 50 Hz PWM (`1000–2000 µs`, $90^\circ = 1500\,\mu\text{s}$) |
| **Pin 23** | **`PA1`** | `TIM2_CH2` (AF1) | `SERVO_FIN2` | **Fin 2 (135° / Q2: -X, +Y)** 50 Hz PWM (`1000–2000 µs`) |
| **Pin 24** | **`PA2`** | `TIM2_CH3` (AF1) | `SERVO_FIN3` | **Fin 3 (225° / Q3: -X, -Y)** 50 Hz PWM (`1000–2000 µs`) |
| **Pin 25** | **`PA3`** | `TIM2_CH4` (AF1) | `SERVO_FIN4` | **Fin 4 (315° / Q4: +X, -Y)** 50 Hz PWM (`1000–2000 µs`) |
| **Pin 90** | **`PB3`** | `SPI1_SCK` (AF5) | `LORA_SCK` | `Ai-Thinker Ra-02 (SX1278)` Clock (Consecutive LQFP row `Pins 90–95`) |
| **Pin 91** | **`PB4`** | `SPI1_MISO` (AF5) | `LORA_MISO` | `Ai-Thinker Ra-02 (SX1278)` MISO |
| **Pin 92** | **`PB5`** | `SPI1_MOSI` (AF5) | `LORA_MOSI` | `Ai-Thinker Ra-02 (SX1278)` MOSI |
| **Pin 93** | **`PB6`** | `GPIO_Output` | `LORA_RST` | `Ai-Thinker Ra-02 (SX1278)` Active-Low Reset (Default `HIGH`) |
| **Pin 94** | **`PB7`** | `GPIO_Output` | `LORA_CS` | `Ai-Thinker Ra-02 (SX1278)` Chip Select (`NSS`, Default `HIGH`) |
| **Pin 95** | **`PB8`** | `GPIO_EXTI8` | `LORA_DIO0` | `Ai-Thinker Ra-02 (SX1278)` Packet Tx/Rx Done Interrupt |
| **Pin 96** | **`PB9`** | `TIM4_CH4` + `DMA1_CH1` | `WS2812_DI` | `WS2812B` Addressable Status LED (`800 kHz`, 2 preamble + 24 data + 240 reset slots) |
| **Pin 70** | **`PA9`** | `USART1_TX` (AF7) | `GPS_RXD` | `u-blox NEO-7M` GPS Receiver (`9600` baud) |
| **Pin 71** | **`PA10`** | `USART1_RX` (AF7) | `GPS_TXD` | `u-blox NEO-7M` GPS Transmitter (`9600` baud) |
| **Pin 51** | **`PB12`** | `GPIO_Output` | `SD_CS` | `microSD` Card SPI Chip Select (Default `HIGH`) |
| **Pin 52** | **`PB13`** | `SPI2_SCK` (AF5) | `SD_SCK` | `microSD` Card SPI2 Clock |
| **Pin 53** | **`PB14`** | `SPI2_MISO` (AF5) | `SD_MISO` | `microSD` Card SPI2 MISO |
| **Pin 54** | **`PB15`** | `SPI2_MOSI` (AF5) | `SD_MOSI` | `microSD` Card SPI2 MOSI |
| **Pin 65** | **`PC6`** | `GPIO_Output` | `BUZZER` | 5V Piezo Buzzer via `BC547` NPN low-side driver + `1N4007` flyback diode |
| **Pin 66** | **`PC7`** | `GPIO_Output` | `PYRO_MOTOR` | **Pyro Channel 1 (Motor Igniter)** — Direct 3.3V logic gate drive to `IRLZ44N` |
| **Pin 67** | **`PC8`** | `GPIO_Output` | `PYRO_SEP` | **Pyro Channel 2 (Stage Separation)** — Direct 3.3V logic gate drive to `IRLZ44N` |
| **Pin 68** | **`PC9`** | `GPIO_Output` | `PYRO_CHUTE` | **Pyro Channel 3 (Parachute Ejection)** — Dual-`BC547` NPN level-shifter $\rightarrow$ `IRF520` (`V_GS = V_BAT`) |
| **Pin 72 / 73** | **`PA11 / PA12`** | `USB_DM / USB_DP` | `USB_CDC` | USB-C Full-Speed Virtual COM Port (`100 Hz` telemetry & interactive command console) |
| **Pin 76 / 79** | **`PA13 / PA14`** | `SYS_JTMS / JTCK` | `SWDIO / SWCLK` | ST-Link V2 Serial Wire Debug & Flash Programming |

---

## 5. Flight Control Loop & X-Configuration Fin Mixing

The attitude control loop in [`Flight.c`](IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/Flight/Flight.c) executes deterministically at **`100 Hz` ($\Delta t = 10\text{ ms}$)**:

1. **Inertial Attitude Estimation (Complementary Filter, $\alpha = 0.98$)**:
   $$\theta_{\text{accel}} = \text{atan2}\!\left(-a_x, \sqrt{a_y^2 + a_z^2}\right)\cdot\frac{180}{\pi}, \qquad \phi_{\text{accel}} = \text{atan2}\!\left(a_y, -a_z\right)\cdot\frac{180}{\pi}$$
   $$\theta_k = 0.98\left(\theta_{k-1} + \omega_x \Delta t\right) + 0.02\,\theta_{\text{accel}}, \qquad \phi_k = 0.98\left(\phi_{k-1} + \omega_y \Delta t\right) + 0.02\,\phi_{\text{accel}}$$

2. **3-Axis PID Attitude Controller** ($K_p = 1.20$, $K_i = 0.05$, $K_d = 0.08$, anti-windup clamped to $\pm 100^\circ$):
   - Pitch & Roll hold vertical attitude setpoint ($0.0^\circ$); Yaw damps angular rate ($\omega_z \rightarrow 0^\circ/\text{s}$).

3. **Orthogonal 4-Fin X-Configuration Aerodynamic Mixing Matrix**:
   $$\begin{bmatrix} \delta_1\ (45^\circ\text{ / Q1}) \\ \delta_2\ (135^\circ\text{ / Q2}) \\ \delta_3\ (225^\circ\text{ / Q3}) \\ \delta_4\ (315^\circ\text{ / Q4}) \end{bmatrix} = 90^\circ + \begin{bmatrix} -0.707 & +1.0 & -0.707 \\ +0.707 & +1.0 & -0.707 \\ +0.707 & +1.0 & +0.707 \\ -0.707 & +1.0 & +0.707 \end{bmatrix} \begin{bmatrix} u_{\text{pitch}} \\ u_{\text{roll}} \\ u_{\text{yaw}} \end{bmatrix}, \quad \delta_i \in [0^\circ, 180^\circ]$$

---

## 6. Quick Start — Firmware, Simulator & Website

### A. Build & Flash `IGNIS_CORE` (`STM32G473VET6`)

```bash
cd "IGNIS_CORE/Firmware/HAL STM32/IGNIS_CORE_STM32G473VET6"
cmake --preset Debug
cmake --build build/Debug -j

# Flash via ST-Link over SWD:
STM32_Programmer_CLI -c port=SWD -w build/Debug/IGNIS_CORE_STM32G473VET6.elf -v -rst
```

### B. Launch the `IGNIS-NET-SIM` Ground Station & Post-Flight Analyzer

```bash
cd IGNIS_NET/IGNIS-NET-SIM
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python3 src/main.py
```

### C. Run the Technical Datasheet & 3D Avionics Website

```bash
cd ignis-website
npm install
npm run dev      # Local dev server at http://localhost:4321
npm run build    # Static bundle output into ./dist/
```

---

## 7. Engineering Documentation Index

- **[System Overview & Architecture](Documentation/System_Overview.md)** — `STM32G473VET6` vs `STM32F4` trade study, power regulation architecture, state machine, and Aux Blackbox Data Logger design.
- **[Hardware Wiring & Circuit Guide](Documentation/Wiring_Guide.md)** — Pin-by-pin LQFP-100 connections, `IRLZ44N` & `2× BC547 + IRF520` Pyro Igniter schematics, `5V` Buzzer driver, and `24LC16B` wiring.
- **[Communication & Telemetry Protocol](Documentation/Communication_Protocol.md)** — `100 Hz` USB CDC CSV format, single-character CLI commands (`A`, `D`, `C`, `S`), and `SX1278` LoRa telemetry frames.
- **[Sensor & Actuator Calibration Guide](Documentation/Calibration_Guide.md)** — 500-sample IMU bias calibration, `MS5611` 2nd-order PROM compensation, `HMC5883L` heading calibration, and fin trim alignment.
- **[Hardware Bringup & Troubleshooting](Documentation/Troubleshooting.md)** — `WS2812B` `TIM4_CH4` DMA waveform verification, `I2C2` `PC4`/`PA8` pin mux notes, and `IGNIS_CORE` PCB rev-1 ERC/DRC audit checklist.

---

## 8. License

- **Firmware, Ground Station Software & Documentation**: Licensed under the **MIT License**.
- **Hardware Schematics, PCB Layouts & Mechanical CAD**: Released under the **CERN Open Hardware Licence v2 — Permissive (CERN-OHL-P v2)**.

---
*Designed and engineered by **Shourya Pandey**.*
