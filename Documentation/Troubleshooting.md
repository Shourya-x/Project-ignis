# Project IGNIS — Hardware Bringup & Troubleshooting Guide

**Document Code:** `IGN-DOC-TRB-05`  
**Scope:** `IGNIS_CORE` (`STM32G473VET6`), `WS2812B` DMA Engine, `I2C2` Sensor Bus, Pyro Drivers & Rev-1 PCB Audit Notes

---

## 1. `I2C2` Bus Diagnostics (`MPU-6500` / `MS5611` / `HMC5883L` Reading `0`)

### Symptom
`MPU6500 WHO_AM_I` returns `0x00` or `[WARN] No I2C devices responded on I2C2!` during boot.

### Root Causes & Fixes
1. **`PC4` (`SCL`) vs `PA8` (`SDA`) Pin Multiplexer Swap**:
   - On the `STM32G473VET6`, **`PC4` (LQFP Pin 30)** only supports hardware **`I2C2_SCL`** (`AF4`), and **`PA8` (LQFP Pin 69)** only supports hardware **`I2C2_SDA`** (`AF4`). Verify `SCL` goes to `PC4` and `SDA` goes to `PA8`.
2. **Missing Pull-Up Resistors**:
   - Ensure `4.7 kΩ` pull-up resistors are connected from both `PC4` and `PA8` to `+3.3V`.
3. **Live Bus Scan**:
   - Open the USB CDC serial monitor (`115200` baud) and press **`S`**. You should see:
     - `0x1E -> HMC5883L Magnetometer`
     - `0x50..0x57 -> 24LC16B EEPROM`
     - `0x68 -> MPU6500 IMU` (`WHO_AM_I = 0x70`)
     - `0x77 -> MS5611 Barometer`

---

## 2. `WS2812B` Status LED (`PB9` — `TIM4_CH4` + DMA) Diagnostics

### Symptom A: LED Stays Dark or Latches Only 23 of 24 Bits
- **Cause 1 (HAL DMA Stop Behavior)**: Calling `HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_4)` in STM32G4 HAL clears `TIM_CCER_CC4E`, floating `PB9` high between frames so the `WS2812B` never sees a $>50\,\mu\text{s}$ `0 V` reset pulse.
  - **Fix (Implemented in [`WS2812.c`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/WS2812/WS2812.c))**: Keep `CCR4 = 0` and re-assert `TIM4->CCER |= TIM_CCER_CC4E` inside `HAL_TIM_PWM_PulseFinishedCallback()` so `PB9` is actively driven to `0.0 V`.
- **Cause 2 (First-Bit DMA Preload Drop)**: Enabling `TIM_DMA_CC4` before timer overflow consumes `pwmData[0]` immediately.
  - **Fix**: `PREAMBLE_SLOTS = 2` prepends two zero-duty slots before the 24 color bits, followed by `RESET_SLOTS = 240` ($300\,\mu\text{s}$ low latch).

### Symptom B: Pulse Train Disappears (`~720 mV` Flatline on Oscilloscope) When LED Is Connected
- **Cause**: The `WS2812B`'s `VDD` (`+5V`) pin is disconnected or has a cold solder joint. The `3.3 V` signal from `PB9` into `DIN` forward-biases the LED's internal ESD protection diode from `DIN` $\rightarrow$ `VDD`, clamping `PB9` to $\sim 0.72\text{ V}$ and parasitically blinking the blue die dimly.
- **Fix**: Verify solid `+5V` continuity on `WS2812B` `VDD` before applying signals to `DIN`, and place a `220 Ω–470 Ω` resistor in series with `DIN`.

---

## 3. Pyro Channel 3 (`IRF520`) Fails to Saturate at `3.3V`

- **Cause**: `IRF520` is a standard-threshold MOSFET ($V_{GS(\text{th})} = 2.0\text{–}4.0\text{ V}$, rated $R_{DS(\text{on})}$ at $V_{GS} = 10\text{ V}$). Driving its gate directly from a `3.3 V` STM32 GPIO pin only passes $\sim 200\text{–}500\text{ mA}$ in the linear region.
- **Fix**: Use logic-level **`IRLZ44N`** MOSFETs (Channels 1 & 2 on `PC7`, `PC8`) or the **dual-`BC547` NPN non-inverting level shifter** on `PC9` (pulling the `IRF520` gate to `+V_BAT` via a `1 kΩ` pull-up resistor when `PC9 = 3.3 V`).

---

## 4. `IGNIS_CORE` PCB Rev-1 Fabrication ERC/DRC Checklist

Before submitting [`ProPrj_Project Ignis_2026-06-03.kicad_pcb`](../IGNIS_CORE/Hardware/Project_IGNIS_CORE/ProPrj_Project%20Ignis_2026-06-03.kicad_pcb) to JLCPCB / LionCircuits, verify the four items identified during the `kicad-cli` automated DRC audit:

1. **`U2` (`AMS1117-5.0`) `VIN` to `5V` Track Clearance**:
   - Reroute the `VIN` track on `F.Cu` at `(123.06 mm, 110.54 mm)` so it does not cross `U2` Pad 2 (`5V` output).
2. **`X2` (`32.768 kHz` Crystal) Pad 1 (`/OSCOUT32`)**:
   - Extend the `/OSCOUT32` trace at `(154.67 mm, 100.84 mm)` by $0.6\text{ mm}$ to land cleanly on `X2` Pad 1.
3. **`GND` Copper Zones & `USB1` `Edge.Cuts`**:
   - Add top (`F.Cu`) and bottom (`B.Cu`) `GND` copper pours to connect the 40 `GND` stitching vias, and trim the overlapping `USB1` (`USBC1-AF-046`) `Edge.Cuts` line segment at `(151.84 mm, 120.06 mm)`.
4. **Power Rail Overvoltage Protection Recommendation**:
   - Because `AMS1117-3.3` can fail short (`VIN` $\rightarrow$ `VOUT`) if overvoltaged, add a `500 mA` resettable PTC polyfuse and a `3.6 V` Zener/TVS crowbar diode (`BZT52C3V6` / `SMBJ3.3A`) across the `+3.3V` rail.
