# IGNIS-NET Receiver Firmware

Ground-station receiver and auxiliary telemetry forwarder firmware for Project IGNIS. Receives 433 MHz `SX1278` LoRa telemetry packets from `IGNIS_CORE`, validates packet framing, logs to local SD storage, and streams CSV telemetry over USB Serial (`115200` baud) to `IGNIS-NET-SIM`.
