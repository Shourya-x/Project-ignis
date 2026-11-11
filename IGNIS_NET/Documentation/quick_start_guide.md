# IGNIS-NET — Quick Start Guide

**Subsystem:** Ground Station Telemetry Receiver & `IGNIS-NET-SIM` Visualization Suite

## 1. Hardware Setup (LoRa Ground Receiver)
1. Connect the 433 MHz antenna to the `SX1278` (`Ra-02`) LoRa receiver module **before** applying power.
2. Plug the `IGNIS-NET` receiver unit into your laptop via USB (`115200` baud).
3. Verify serial reception of incoming telemetry frames from `IGNIS_CORE`.

## 2. Launching the `IGNIS-NET-SIM` Desktop Ground Station
```bash
cd IGNIS_NET/IGNIS-NET-SIM
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python3 src/main.py
```

## 3. Replaying Flight Logs & Exporting Google Earth KML
1. Click **Load CSV** in `IGNIS-NET-SIM` and select a flight log from `data/flight_logs/` (e.g., `sample_flight.csv`).
2. Use the playback transport bar (**Play / Pause / Reset**) and speed selector (`0.25x` to `4.0x`) to inspect synchronized altitude, velocity, 3D attitude, acceleration, and GPS trajectory plots.
3. Click **Export KML** to generate a 3D Google Earth flight path (`.kml`) with launch, apogee, and landing pins.
