#!/bin/bash

# IGNIS-NET-SIM - Flight Simulator for IGNIS TVC System
# This script creates the complete project structure with EMPTY files

echo "🔥 Creating IGNIS-NET-SIM project structure..."

# Create main project directory
mkdir -p IGNIS-NET-SIM
cd IGNIS-NET-SIM

# Create directory structure
mkdir -p PC_Software/src/data
mkdir -p PC_Software/src/gui
mkdir -p PC_Software/src/receiver
mkdir -p PC_Software/src/utils
mkdir -p PC_Software/data/flight_logs
mkdir -p PC_Software/data/exports
mkdir -p PC_Software/data/simulation_outputs
mkdir -p PC_Software/tests
mkdir -p PC_Software/docs

echo "✅ Directory structure created"

# Create all Python files (empty)
touch PC_Software/src/__init__.py
touch PC_Software/src/data/__init__.py
touch PC_Software/src/data/data_loader.py
touch PC_Software/src/gui/__init__.py
touch PC_Software/src/gui/main_window.py
touch PC_Software/src/gui/plots.py
touch PC_Software/src/gui/widgets.py
touch PC_Software/src/receiver/__init__.py
touch PC_Software/src/receiver/serial_receiver.py
touch PC_Software/src/receiver/lora_parser.py
touch PC_Software/src/utils/__init__.py
touch PC_Software/src/utils/math_utils.py
touch PC_Software/src/utils/geo_utils.py
touch PC_Software/src/utils/kml_export.py
touch PC_Software/src/utils/pdf_report.py
touch PC_Software/src/main.py

# Create test files
touch PC_Software/tests/test_data_loader.py
touch PC_Software/tests/test_math_utils.py
touch PC_Software/tests/test_geo_utils.py

# Create requirements and README
touch PC_Software/requirements.txt
touch PC_Software/README.md
touch PC_Software/docs/user_guide.md
touch PC_Software/docs/api_reference.md

# Create sample data placeholder
touch PC_Software/data/flight_logs/sample_flight.csv
touch PC_Software/data/flight_logs/.gitkeep

echo "✅ All files created successfully!"
echo ""
echo "📁 Project structure:"
echo ""
echo "IGNIS-NET-SIM/
echo "    ├── src/"
echo "    │   ├── __init__.py"
echo "    │   ├── main.py"
echo "    │   ├── data/"
echo "    │   │   ├── __init__.py"
echo "    │   │   └── data_loader.py"
echo "    │   ├── gui/"
echo "    │   │   ├── __init__.py"
echo "    │   │   ├── main_window.py"
echo "    │   │   ├── plots.py"
echo "    │   │   └── widgets.py"
echo "    │   ├── receiver/"
echo "    │   │   ├── __init__.py"
echo "    │   │   ├── serial_receiver.py"
echo "    │   │   └── lora_parser.py"
echo "    │   └── utils/"
echo "    │       ├── __init__.py"
echo "    │       ├── math_utils.py"
echo "    │       ├── geo_utils.py"
echo "    │       ├── kml_export.py"
echo "    │       └── pdf_report.py"
echo "    ├── data/"
echo "    │   ├── flight_logs/"
echo "    │   ├── exports/"
echo "    │   └── simulation_outputs/"
echo "    ├── tests/"
echo "    ├── docs/"
echo "    ├── requirements.txt"
echo "    └── README.md"
echo ""
echo "✅ Done! Navigate to IGNIS-NET-SIM/PC_Software to start coding."
