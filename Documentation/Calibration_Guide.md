# Project IGNIS — Sensor & Actuator Calibration Guide

**Document Code:** `IGN-DOC-CAL-04`  
**Applicable Firmware:** `IGNIS_CORE_STM32G473VET6` (`Modules/IMU`, `Modules/Barometer`, `Modules/Sensors`, `Modules/Servos`)

---

## 1. `MPU-6500` 6-Axis IMU Bias Calibration

### 1.1 Scale Factors
In [`IMU_Init()`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/IMU/IMU.c), the `MPU-6500` is configured for rocket boost dynamics:
- **Accelerometer (`ACCEL_CONFIG = 0x10`)**: $\pm 8\text{g}$ full scale $\rightarrow \mathbf{4096\text{ LSB/g}}$
- **Gyroscope (`GYRO_CONFIG = 0x10`)**: $\pm 1000^\circ/\text{s}$ full scale $\rightarrow \mathbf{32.8\text{ LSB}/(^\circ/\text{s})}$

### 1.2 500-Sample Stationary Bias Procedure
On startup (when the `WS2812B` status LED on `PB9` turns **Amber** `(255, 180, 0)`) or when sending **`C`** over USB CDC:
1. Keep the rocket / avionics bay completely stationary in its vertical pad orientation for **$1.0\text{ second}$** ($500\text{ samples} \times 2\text{ ms}$).
2. Firmware computes the arithmetic mean of all 6 axes and removes the $1\text{g}$ gravity vector ($4096\text{ LSB}$) from the vertical axis:
   $$\text{offset}_{a,x} = \frac{1}{500}\sum a_x, \quad \text{offset}_{a,y} = \frac{1}{500}\sum a_y, \quad \text{offset}_{a,z} = \left(\frac{1}{500}\sum a_z\right) - 4096$$
   $$\text{offset}_{\omega,i} = \frac{1}{500}\sum \omega_i \quad (i \in \{x,y,z\})$$
3. Verify over USB serial (`S` command or startup banner) that steady-state `Pitch` and `Roll` read within $\pm 0.2^\circ$ of $0.00^\circ$ and `YawRate` reads within $\pm 0.05^\circ/\text{s}$.

---

## 2. `MS5611-01BA03` Barometric Pressure & Pad Zeroing

1. **Factory PROM Readout**: On boot, [`Barometer_ReadCalibration()`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/Barometer/Barometer.c) reads the six 16-bit factory calibration words `C[1]` through `C[6]` from registers `0xA2–0xAC`.
2. **Second-Order Temperature Compensation**:
   - First-order temperature `TEMP` ($0.01^\circ\text{C}$ units) and pressure `P` ($\text{Pa}$) are computed from 24-bit $\Delta\Sigma$ readings `D1` and `D2` ($\text{OSR} = 4096$).
   - When $\text{TEMP} < 20.00^\circ\text{C}$ (`TEMP < 2000`), second-order non-linearity terms `T2`, `OFF2`, and `SENS2` are automatically subtracted.
3. **Pad Reference Altitude ($h_{\text{AGL}} = 0\text{ m}$)**:
   - Record baseline pad pressure $P_0$ prior to arming to compute Above-Ground-Level (AGL) altitude via the hypsometric equation:
     $$h_{\text{AGL}} = 44330 \times \left[1 - \left(\frac{P}{P_0}\right)^{0.190295}\right]$$

---

## 3. `HMC5883L` 3-Axis Magnetometer Calibration

1. **Register Ordering**: Note that `HMC5883L` data registers (`0x03–0x08`) output axes in **`X, Z, Y`** order—handled automatically in [`HMC5883L_ReadRaw()`](../IGNIS_CORE/Firmware/HAL%20STM32/IGNIS_CORE_STM32G473VET6/Modules/Sensors/Sensors.c).
2. **Hard-Iron Offset Removal**: Rotate the avionics bay through $360^\circ$ on all three axes away from ferromagnetic launch rail structures, record $(\min, \max)$ on each axis, and subtract the midpoint bias:
   $$B_{\text{offset},i} = \frac{B_{\max,i} + B_{\min,i}}{2}$$

---

## 4. X-Configuration Fin / TVC Servo Centering (`PA0–PA3`)

1. **Neutral Pulse Width**: Disarm the flight computer (`D` command over USB; `WS2812B` turns **Green**). All four `TIM2` PWM channels (`PA0–PA3`) output $90^\circ = \mathbf{1500\,\mu\text{s}}$ at $50\text{ Hz}$.
2. **Mechanical Horn Alignment**: Mount the 4 fin servo horns (or 2-axis TVC gimbal linkages) as close to $0^\circ$ mechanical incidence as possible while the servos are actively driven at $1500\,\mu\text{s}$.
3. **Direction Verification**:
   - Tilt the nose of the rocket **down ($-\text{Pitch}$)** while Armed (`A` command) and verify that `Fin 1 (FR)` and `Fin 2 (FL)` deflect to generate a restoring nose-up pitching moment.
   - Roll the airframe clockwise and verify all four X-fins deflect differentially to oppose the roll rate.
