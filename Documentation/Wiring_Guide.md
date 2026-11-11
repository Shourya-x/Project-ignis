# Project IGNIS — Complete Hardware Wiring & Circuit Guide

**Document Code:** `IGN-DOC-WIR-02`  
**Target Hardware:** `IGNIS_CORE` (`STM32G473VET6` LQFP-100) & `Aux Blackbox Data Logger` (`ATmega328P-AU`)

---

## 1. `STM32G473VET6` Master Pin Assignment Table

The pinout in [`IGNIS_CORE_STM32G473VET6.ioc`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/IGNIS_CORE_STM32G473VET6.ioc) and [`main.h`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Core/Inc/main.h) groups each bus onto contiguous physical pins of the 100-pin LQFP package to simplify PCB routing and benchtop harness assembly.

### 1.1 `I2C2` Sensor & Non-Volatile Memory Bus (`400 kHz` / `100 kHz`)
> [!IMPORTANT]
> **Hardware Pin Multiplexer Note**: On the `STM32G473VET6`, **`PC4` (Pin 30)** is hardware **`I2C2_SCL`** (`AF4`) and **`PA8` (Pin 69)** is hardware **`I2C2_SDA`** (`AF4`). Do not swap `PC4` and `PA8`—the hardware I2C peripheral cannot invert SCL/SDA roles on these pins.

| Bus Line | STM32 Pin | LQFP-100 Pin | Pull-Up | Connected Devices & 7-Bit I2C Addresses |
| :--- | :---: | :---: | :---: | :--- |
| **`I2C2_SCL`** | **`PC4`** | **Pin 30** | `4.7 kΩ` to `+3.3V` | `MPU-6500` (`0x68`), `MS5611` (`0x77`), `HMC5883L` (`0x1E`), `24LC16B` (`0x50–0x57`) |
| **`I2C2_SDA`** | **`PA8`** | **Pin 69** | `4.7 kΩ` to `+3.3V` | `MPU-6500` (`0x68`), `MS5611` (`0x77`), `HMC5883L` (`0x1E`), `24LC16B` (`0x50–0x57`) |

#### `24LC16B` EEPROM Pin Connections (8-Pin DIP / SOIC-8)
Unlike the `AT24C256`, the **`24LC16B`** (2048 bytes) internally maps its memory across **eight 256-byte blocks** at I2C addresses **`0x50` through `0x57`** (`A0`, `A1`, `A2` pins are ignored internally).

| `24LC16B` Pin | Pin Name | Connection | Notes |
| :---: | :---: | :--- | :--- |
| **Pins 1, 2, 3** | `A0, A1, A2` | `GND` | Ignored internally by `24LC16B` (uses block bits in control byte) |
| **Pin 4** | `VSS` | `GND` | System Ground |
| **Pin 5** | `SDA` | **`PA8`** (`I2C2_SDA`) | Shared `I2C2` Data line |
| **Pin 6** | `SCL` | **`PC4`** (`I2C2_SCL`) | Shared `I2C2` Clock line |
| **Pin 7** | `WP` | **`GND`** | **Must be tied to `GND`** to enable write operations |
| **Pin 8** | `VCC` | **`+3.3V`** | Decouple with `100 nF` ceramic capacitor close to Pin 8 |

---

### 1.2 `SPI1` Telemetry Radio — `Ai-Thinker Ra-02 (SX1278)` (Consecutive LQFP Pins 90–95)

| LQFP-100 Pin | STM32 Pin | CubeMX Label | `Ra-02 / SX1278` Pin | Direction & Configuration |
| :---: | :---: | :--- | :---: | :--- |
| **Pin 90** | **`PB3`** | `SPI1_SCK` (`AF5`) | `SCK` | SPI1 Master Clock |
| **Pin 91** | **`PB4`** | `SPI1_MISO` (`AF5`) | `MISO` | SPI1 Master In / Slave Out |
| **Pin 92** | **`PB5`** | `SPI1_MOSI` (`AF5`) | `MOSI` | SPI1 Master Out / Slave In |
| **Pin 93** | **`PB6`** | `LORA_RST` (`GPIO_Output`) | `RST` | Active-Low Hardware Reset (Initialized `HIGH`) |
| **Pin 94** | **`PB7`** | `LORA_CS` (`GPIO_Output`) | `NSS / CS` | Active-Low Chip Select (Initialized `HIGH`; `PA15` optional on rev-1 PCB) |
| **Pin 95** | **`PB8`** | `LORA_DIO0` (`GPIO_EXTI8`) | `DIO0` | Rising-Edge External Interrupt (`EXTI9_5_IRQn`) on Packet Done |
| — | `+3.3V` | Power | `3.3V` | **Strictly `+3.3V` (`SX1278` is NOT 5V-tolerant)** |

---

### 1.3 `SPI2` Onboard Blackbox — `microSD` Card Slot (Consecutive LQFP Pins 51–54)

| LQFP-100 Pin | STM32 Pin | CubeMX Label | `microSD` Pin | Notes |
| :---: | :---: | :--- | :---: | :--- |
| **Pin 51** | **`PB12`** | `SD_CS` (`GPIO_Output`) | `CS / DAT3` | Active-Low Chip Select (Initialized `HIGH`) |
| **Pin 52** | **`PB13`** | `SPI2_SCK` (`AF5`) | `CLK` | SPI2 Clock |
| **Pin 53** | **`PB14`** | `SPI2_MISO` (`AF5`) | `DO / DAT0` | SPI2 MISO (`10 kΩ` pull-up recommended) |
| **Pin 54** | **`PB15`** | `SPI2_MOSI` (`AF5`) | `DI / CMD` | SPI2 MOSI |

---

### 1.4 `USART1` GPS Receiver (`u-blox NEO-7M`) & `USB-C` Telemetry

| Subsystem | LQFP-100 Pin | STM32 Pin | Peripheral Function | Connected To |
| :--- | :---: | :---: | :--- | :--- |
| **GPS (`NEO-7M`)** | **Pin 70** | **`PA9`** | `USART1_TX` (`AF7`) | `NEO-7M` **`RXD`** (`9600` baud 8N1) |
| **GPS (`NEO-7M`)** | **Pin 71** | **`PA10`** | `USART1_RX` (`AF7`) | `NEO-7M` **`TXD`** (`9600` baud 8N1) |
| **USB-C Full Speed** | **Pin 72** | **`PA11`** | `USB_DM` | USB-C Connector `D-` (`USBC1-AF-046`) |
| **USB-C Full Speed** | **Pin 73** | **`PA12`** | `USB_DP` | USB-C Connector `D+` (`USBC1-AF-046`) |

---

### 1.5 `TIM2` 4-Channel Active Fin / TVC Servo Outputs (Consecutive LQFP Pins 22–25)

`TIM2` is configured with `Prescaler = 144 - 1` ($1\text{ MHz}$ timer tick at $144\text{ MHz}$ `SYSCLK`) and `ARR = 20000 - 1` ($20\text{ ms}$ period = **`50 Hz`**).

| Channel | LQFP-100 Pin | STM32 Pin | Timer Channel | Airframe Fin Position | Pulse Width Range |
| :--- | :---: | :---: | :---: | :--- | :---: |
| **Fin 1** | **Pin 22** | **`PA0`** | `TIM2_CH1` (`AF1`) | **Quadrant I (45°: +X, +Y)** | $1000\,\mu\text{s}\ (0^\circ) \text{ – } 1500\,\mu\text{s}\ (90^\circ) \text{ – } 2000\,\mu\text{s}\ (180^\circ)$ |
| **Fin 2** | **Pin 23** | **`PA1`** | `TIM2_CH2` (`AF1`) | **Quadrant II (135°: -X, +Y)** | $1000\,\mu\text{s}\ (0^\circ) \text{ – } 1500\,\mu\text{s}\ (90^\circ) \text{ – } 2000\,\mu\text{s}\ (180^\circ)$ |
| **Fin 3** | **Pin 24** | **`PA2`** | `TIM2_CH3` (`AF1`) | **Quadrant III (225°: -X, -Y)** | $1000\,\mu\text{s}\ (0^\circ) \text{ – } 1500\,\mu\text{s}\ (90^\circ) \text{ – } 2000\,\mu\text{s}\ (180^\circ)$ |
| **Fin 4** | **Pin 25** | **`PA3`** | `TIM2_CH4` (`AF1`) | **Quadrant IV (315°: +X, -Y)** | $1000\,\mu\text{s}\ (0^\circ) \text{ – } 1500\,\mu\text{s}\ (90^\circ) \text{ – } 2000\,\mu\text{s}\ (180^\circ)$ |

---

## 2. Pyrotechnic Igniter & Buzzer Driver Circuits (`PC6–PC9`)

### 2.1 Channels 1 & 2 (`PC7` `PYRO_MOTOR` & `PC8` `PYRO_SEP`) — Direct `IRLZ44N` Logic-Level Drive
The **`IRLZ44N`** has a logic-level gate threshold ($V_{GS(\text{th})} = 1.0\text{–}2.0\text{ V}$) and achieves $R_{DS(\text{on})} \approx 0.035\,\Omega$ at $V_{GS} = 3.3\text{ V}$, allowing direct drive from `PC7` and `PC8` via a $100\,\Omega$ gate series resistor and a $10\text{ k}\Omega$ pull-down resistor (ensuring the igniter cannot fire while the MCU pins are high-Z during reset):

```text
                                 +V_BAT (7.4V–11.1V LiPo)
                                          │
                                    ┌─────┴─────┐
                                    │           │
                             [Pyro Igniter]   ──┴── 1N4007 Flyback Diode (D1/D2)
                                    │         ──┬── (Cathode to +V_BAT)
                                    └─────┬─────┘
                                          │
                                  Pin 2: Drain (D)
PC7 / PC8 ───[ 100 Ω ]───┬──────── Pin 1: Gate  (G)   [Q1 / Q2: IRLZ44N (TO-220)]
                         │        Pin 3: Source (S)
                     [ 10 kΩ ]            │
                         │                │
                        GND───────────────┴────────── GND
```

### 2.2 Channel 3 (`PC9` `PYRO_CHUTE`) — Dual-`BC547` NPN Level-Shifter + `IRF520`
Unlike `IRLZ44N`, the standard **`IRF520`** MOSFET has $V_{GS(\text{th})} = 2.0\text{–}4.0\text{ V}$ and requires $V_{GS} \ge 7.4\text{–}10\text{ V}$ for full saturation ($R_{DS(\text{on})} = 0.27\,\Omega$). Driving an `IRF520` directly from `3.3 V` leaves it in the linear region.

To guarantee **fail-safe OFF** when `PC9 = 0 V` (or floating during reset) and **full `+V_BAT` gate drive** when `PC9 = 3.3 V`, Channel 3 uses a cascaded 2-stage **`BC547` NPN non-inverting level shifter** (verified in LTspice / Qucs-S simulation):
- **Standby (`PC9 = 0 V`)**: `Q_A` (`BC547`) is OFF $\rightarrow$ `R2` (`4.7 kΩ`) saturates `Q_B` (`BC547`) ON $\rightarrow$ `IRF520` Gate is clamped to **`0 V` (Strictly OFF)**.
- **Fire (`PC9 = 3.3 V`)**: `Q_A` turns ON, pulling `Q_B` Base to `0 V` $\rightarrow$ `Q_B` turns OFF $\rightarrow$ `R3` (`1 kΩ`) pulls `IRF520` Gate to **`+V_BAT` (`7.4V–12V`, Fully Saturated ON)**.

```text
                                      +V_BAT (7.4V–12V)
                                       │             │
                                    [1 kΩ] (R3)  [Chute E-Match (+)]
                                       │         [Chute E-Match (-)]
                  +3.3V                ├─────────────┤  (with parallel 1N4007 D3)
                    │                  │             │
                 [4.7 kΩ] (R2)         │         Pin 2: Drain (D)
                    │                  │             │
                    ├───► Base (Q_B) Collector   Pin 1: Gate  (G)  [Q3: IRF520]
                    │     (BC547)      (Q_B)         │
                Collector              │         Pin 3: Source (S)
PC9 ──[4.7 kΩ]── Base (Q_A: BC547)   Emitter         │
      (R1)      Emitter                │             │
                    │                  │             │
                   GND─────────────────┴─────────────┴─── GND
```

### 2.3 `5V` Buzzer Driver (`PC6` — LQFP Pin 65)
To isolate `PC6` (`3.3 V`, $20\text{ mA}$ max) from the `5 V` piezo/electromagnetic buzzer current ($30\text{–}80\text{ mA}$) and inductive kickback:

```text
          +5.0V Rail
            │
        ┌───┴───┐
        │       │
   [Buzzer +]  ─┴─ 1N4007 / 1N4148 Flyback Diode (Cathode to +5.0V)
   [Buzzer -]  ─┬─
        └───┬───┘
            │
        Collector (C)
PC6 ──[1 kΩ]── Base (B)   [BC547 NPN]
        Emitter (E)
            │
           GND
```

### 2.4 `WS2812B` Addressable Status LED (`PB9` — LQFP Pin 96)
- **`VDD`**: Connect to `+5.0V` (or `+3.3V`) with a `100 nF` decoupling capacitor across `VDD`–`GND`.
- **`DIN`**: Connect to **`PB9`** (`TIM4_CH4`) through a **`220 Ω–470 Ω` series protection resistor**.
  > [!WARNING]
  > **Never leave `WS2812B` `VDD` disconnected while `PB9` is active**: Driving `3.3 V` into `DIN` while `VDD` is unpowered back-powers the LED through its internal ESD clamp diode (clamping `PB9` to $\sim 0.72\text{ V}$) and can permanently damage the `WS2812B` input stage.

---

## 3. Auxiliary Blackbox Data Logger PCB Connections

Located in `IGNIS_CORE/Aux Hardware/Data Logger/data logger/`:
- **MCU (`U4`)**: `ATmega328P-AU` (TQFP-32) with `16 MHz` crystal (`Y2`) and `22 pF` load capacitors (`C2`, `C3`).
- **Dual EEPROM Banks (`U1`, `U2`)**: 2× `24LC1025` (SOIC-8, $128\text{ KB}$ each) on the hardware I2C bus (`PC4/SDA`, `PC5/SCL`) with `4.7 kΩ` pull-up resistors (`R3`, `R4`).
- **Real-Time Clock (`U3`)**: `DS1337S+` (SOIC-8) with `32.768 kHz` crystal (`Y1`) and coin-cell backup holder (`BAT1`).
