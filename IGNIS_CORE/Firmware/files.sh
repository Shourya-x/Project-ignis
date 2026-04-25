#!/bin/bash

# IGNIS Project - Create empty file structure
# Run this script from your IGNIS project root

echo "🔥 Creating IGNIS project files..."

# Function to create directory if it doesn't exist
create_dir() {
    if [ ! -d "$1" ]; then
        mkdir -p "$1"
        echo "  Created directory: $1"
    fi
}

# Create all directories
create_dir "data"
create_dir "include"
create_dir "src"
create_dir "src/sensors"
create_dir "src/control"
create_dir "src/output_devices"
create_dir "src/telemetry"
create_dir "src/utils"

# Create files (force touch even if they exist)
echo "  Creating files..."

touch include/config.h
echo "    ✓ include/config.h"

touch src/main.cpp
echo "    ✓ src/main.cpp"

# Sensors
touch src/sensors/mpu6500.h src/sensors/mpu6500.cpp
touch src/sensors/ms5611.h src/sensors/ms5611.cpp
touch src/sensors/neo6m.h src/sensors/neo6m.cpp
echo "    ✓ src/sensors/ (6 files)"

# Control
touch src/control/tvc.h src/control/tvc.cpp
echo "    ✓ src/control/ (2 files)"

# Output devices
touch src/output_devices/led_status.h src/output_devices/led_status.cpp
touch src/output_devices/buzzer.h src/output_devices/buzzer.cpp
echo "    ✓ src/output_devices/ (4 files)"

# Telemetry
touch src/telemetry/sd_card.h src/telemetry/sd_card.cpp
touch src/telemetry/lora_receive.h src/telemetry/lora_receive.cpp
touch src/telemetry/lora_send.h src/telemetry/lora_send.cpp
echo "    ✓ src/telemetry/ (6 files)"

# Utils
touch src/utils/serial_comm.h src/utils/serial_comm.cpp
touch src/utils/serial_sender.h src/utils/serial_sender.cpp
touch src/utils/serial_receiver.h src/utils/serial_receiver.cpp
echo "    ✓ src/utils/ (6 files)"

# README
touch README.md
echo "    ✓ README.md"

echo ""
echo "✅ All files created successfully!"
echo ""
echo "📁 Project structure:"

# Show tree if available, otherwise use ls
if command -v tree &> /dev/null; then
    tree -L 3 --dirsfirst
else
    echo "src/"
    ls -la src/
    echo ""
    echo "include/"
    ls -la include/
fi