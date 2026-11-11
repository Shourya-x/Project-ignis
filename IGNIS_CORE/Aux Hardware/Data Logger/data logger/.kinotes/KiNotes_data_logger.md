# Auxiliary Blackbox Data Logger PCB — Engineering Design Notes

**Project Path:** `IGNIS_CORE/Aux Hardware/Data Logger/data logger/`  
**Target MCU:** `ATmega328P-AU` (`U4`, TQFP-32 @ `16 MHz`)

---

## 1. Overview
The **IGNIS Auxiliary Data Logger** is a standalone, self-timed non-volatile flight data recorder ("Blackbox") designed to operate alongside `IGNIS_CORE`. By pairing an independent `ATmega328P-AU` microcontroller with dual `24LC1025` CMOS Serial EEPROMs and a battery-backed `DS1337S+` Real-Time Clock, high-rate flight telemetry is preserved in solid-state EEPROM even if the primary flight battery disconnects upon hard landing.

## 2. Key Circuit Blocks & Bill of Materials

| Reference | Part / Value | Footprint | Function |
| :--- | :--- | :--- | :--- |
| **`U4`** | `ATmega328P-AU` | `TQFP-32` | 8-bit AVR flight logger controller (`16 MHz` external crystal `Y2` with `22 pF` caps `C2`, `C3`) |
| **`U1`, `U2`** | `24LC1025` (×2) | `SOIC-8` | $2 \times 1\text{ Mbit}$ ($2 \times 128\text{ KB} = 256\text{ KB}$ total) I2C Serial EEPROM array |
| **`U3`** | `DS1337S+` | `SOIC-8` | I2C Real-Time Clock (`0x68`) with dual programmable alarms & `32.768 kHz` crystal `Y1` |
| **`BAT1`** | Coin Cell Holder | SMD / THT | Backup power source for `DS1337S+` timekeeping retention |
| **`R3`, `R4`** | `4.7 kΩ` | `0805` | I2C `SDA` / `SCL` bus pull-up resistors |
| **`D1`, `D2`** | Status LEDs | `0805` | Power and EEPROM write-activity indicators (with `330 Ω` current-limiting resistors `R5`, `R7`) |
| **`C1, C4, C5`** | `100 nF` (`0.1 µF`) | `0805` | Local `VCC` bypass decoupling capacitors for `U1`, `U2`, `U3`, `U4` |
| **`H1–H4`** | `MountingHole` | M3 | 4-corner mechanical standoffs for Avionics Bay sled integration |

## 3. I2C Memory Map (`ATmega328P` `PC4/SDA` & `PC5/SCL`)
- **`U1` (`24LC1025` Bank 0)**: `0x50` (Lower 64 KB, `B0=0`) and `0x54` (Upper 64 KB, `B0=1`)
- **`U2` (`24LC1025` Bank 1)**: `0x51` (Lower 64 KB, `B0=0`) and `0x55` (Upper 64 KB, `B0=1`)
- **`U3` (`DS1337S+` RTC)**: `0x68`
