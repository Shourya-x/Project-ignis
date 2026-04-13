#!/bin/bash

# IGNIS Project - Create empty file structure with directory checks
# Run this script from your IGNIS project root

echo "🔥 Creating IGNIS project files..."

# Function to create directory if it doesn't exist
create_dir() {
    if [ ! -d "$1" ]; then
        mkdir -p "$1"
        echo "  📁 Created directory: $1"
    else
        echo "  📁 Directory already exists: $1"
    fi
}

# Function to create file if it doesn't exist
create_file() {
    if [ ! -f "$1" ]; then
        touch "$1"
        echo "    📄 Created file: $1"
    else
        echo "    📄 File already exists: $1"
    fi
}

# Create all directories
echo ""
echo "📂 Creating directories..."
create_dir "data"
create_dir "include"
create_dir "src"
create_dir "src/output_devices"
create_dir "src/telemetry"
create_dir "src/utils"

# Create files
echo ""
echo "📝 Creating files..."

create_file "include/config.h"
create_file "src/main.cpp"

# Output devices
echo "  [Output Devices]"
create_file "src/output_devices/led_status.h"
create_file "src/output_devices/led_status.cpp"
create_file "src/output_devices/buzzer.h"
create_file "src/output_devices/buzzer.cpp"

# Telemetry
echo "  [Telemetry]"
create_file "src/telemetry/sd_card.h"
create_file "src/telemetry/sd_card.cpp"
create_file "src/telemetry/lora_receive.h"
create_file "src/telemetry/lora_receive.cpp"
create_file "src/telemetry/lora_send.h"
create_file "src/telemetry/lora_send.cpp"

# Utils
echo "  [Utils]"
create_file "src/utils/serial_comm.h"
create_file "src/utils/serial_comm.cpp"
create_file "src/utils/serial_sender.h"
create_file "src/utils/serial_sender.cpp"
create_file "src/utils/serial_receiver.h"
create_file "src/utils/serial_receiver.cpp"

# README
create_file "README.md"

echo ""
echo "✅ All files created successfully!"
echo ""
echo "📁 Final project structure:"

# Show tree if available
if command -v tree &> /dev/null; then
    tree -L 3 --dirsfirst 2>/dev/null
else
    echo "src/"
    ls -la src/
    echo ""
    echo "include/"
    ls -la include/
fi

# Count files and directories
echo ""
echo "📊 Summary:"
echo "  Directories: $(find . -type d | wc -l)"
echo "  Files: $(find . -type f | wc -l)"