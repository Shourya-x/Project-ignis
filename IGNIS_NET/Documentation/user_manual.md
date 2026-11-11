# IGNIS-NET — Ground Station & Telemetry User Manual

## 1. Architecture Overview
`IGNIS-NET` comprises the ground-side `SX1278` (`433 MHz`) LoRa telemetry receiver and the **`IGNIS-NET-SIM`** Python/PyQt6 desktop mission control application (`IGNIS_NET/IGNIS-NET-SIM/`).

## 2. Desktop Ground Station Features (`IGNIS-NET-SIM`)
- **Multi-Panel Real-Time Telemetry Plots (`PyQtGraph`)**:
  - Altitude AGL ($\text{m}$) and Vertical Velocity ($\text{m/s}$)
  - 3-Axis Euler Attitude (`Pitch`, `Roll`, `Yaw` in degrees)
  - 3-Axis Linear Acceleration ($a_x, a_y, a_z$ in $\text{m/s}^2$) and Gyroscope Rates ($\omega_x, \omega_y, \omega_z$ in $^\circ/\text{s}$)
- **3D OpenGL Vehicle Attitude Indicator**:
  - Real-time orientation rendering of the rocket body driven by live or replayed Euler angles.
- **Post-Flight KML Trajectory Exporter (`src/utils/kml_exporter.py`)**:
  - Converts logged `lat`, `lon`, and `altitude` arrays into extruded 3D `LineString` KML paths for Google Earth inspection.
