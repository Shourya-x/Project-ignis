# IGNIS-NET-SIM — Rocket Telemetry Ground Station & Post-Flight Simulator

**`IGNIS-NET-SIM`** is a cross-platform Python + `PyQt6` + `PyQtGraph` + `PyOpenGL` mission control and post-flight analysis suite for the **Project IGNIS** rocket flight computer.

## Features
- **Synchronized Telemetry Playback**: Load 17-channel CSV flight logs (`data/flight_logs/sample_flight.csv`) with adjustable playback speed (`0.25x`–`4.0x`).
- **Live & Post-Flight Plotting**: High-rate altitude, vertical velocity, 3-axis Euler orientation (`Pitch`, `Roll`, `Yaw`), and IMU accelerometer/gyroscope time-series graphs.
- **Google Earth 3D KML Export**: Export 3D GPS flight trajectories with automatic Launch, Apogee, and Touchdown markers (`src/utils/kml_exporter.py`).
- **Unit Test Suite**: Automated `pytest` verification for CSV parsing (`tests/test_data_loader.py`), geodesic distance/bearing math (`tests/test_geo_utils.py`), and attitude utilities (`tests/test_math_utils.py`).

## Quick Start
```bash
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python3 src/main.py
```
