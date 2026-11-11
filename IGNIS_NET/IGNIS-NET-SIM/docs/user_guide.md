# IGNIS-NET-SIM — User Guide

## 1. Loading & Replaying Flight Telemetry
1. Run `python3 src/main.py` from `IGNIS_NET/IGNIS-NET-SIM/`.
2. Click **Open Flight Log** and choose `data/flight_logs/sample_flight.csv` (or any flight CSV exported from `IGNIS_CORE`).
3. Use the timeline scrubber or **Play / Pause** buttons to inspect attitude and fin/TVC stability throughout boost, coast, apogee, and recovery.

## 2. Running the Automated Test Suite
```bash
cd IGNIS_NET/IGNIS-NET-SIM
pytest tests/
```
