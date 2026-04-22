
<div align="center">
  
  # 🔥 IGNIS
  
  ### Open-Source Thrust Vector Control (TVC) System for Model Rockets
  
  [![PlatformIO](https://img.shields.io/badge/PlatformIO-STM32-orange)](https://platformio.org/)
  [![Astro](https://img.shields.io/badge/Astro-Website-purple)](https://astro.build/)
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
  [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)
  
  *"Make advanced rocketry accessible to everyone"*
  
  [Website](https://project-ignis.shourya.page) • [Documentation](docs/) • [Contributing](CONTRIBUTING.md) • [Discord](https://discord.gg/ignis)
  
</div>

---

## 📌 Overview

**IGNIS** (Latin for "FIRE") is an open-source thrust vector control system designed for model rocketry enthusiasts, students, and researchers. It uses an **STM32F411** flight computer, **MPU6500** IMU, **MS5611** barometer, and **NEO-6M** GPS to actively steer a rocket's engine nozzle, keeping it stable during flight.

| Feature | Description |
|---------|-------------|
| 🎯 **Precision Control** | Real-time attitude control with complementary filter and PID |
| 📡 **LoRa Telemetry** | Long-range telemetry up to 5km with SX1278 |
| 🛰️ **GPS Tracking** | NEO-6M for position tracking, speed, and landing location |
| ⚙️ **Modular Design** | Separate modules for flight computer, logger, and TVC mechanism |
| 🖥️ **Flight Simulator** | Python-based post-flight analysis and visualization |
| 🌐 **Website** | Full project documentation at [project-ignis.shourya.page](https://project-ignis.shourya.page) |

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              IGNIS SYSTEM                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────┐    ┌─────────────────────────┐                 │
│  │      IGNIS-CORE         │    │       IGNIS-NET         │                 │
│  │   (Flight Computer)     │    │     (Data Logger)       │                 │
│  │                         │    │                         │                 │
│  │  • STM32F411CEU6        │    │  • Arduino Nano         │                 │
│  │  • MPU6500 (IMU)        │◄───►│  • SD Card              │                 │
│  │  • MS5611 (Barometer)   │UART│  • LoRa (SX1278)        │                 │
│  │  • NEO-6M (GPS)         │    │  • GPS (optional)       │                 │
│  │  • 2x Servos (PWM)      │    │                         │                 │
│  └───────────┬─────────────┘    └─────────────────────────┘                 │
│              │                                                              │
│              │ PWM                                                          │
│              ▼                                                              │
│  ┌─────────────────────────┐                                                │
│  │      IGNIS-CTRL         │                                                │
│  │   (Mechanical TVC)      │                                                │
│  │                         │                                                │
│  │  • 2x High-torque servos│                                                │
│  │  • 3D printed gimbal    │                                                │
│  │  • Linkage mechanism    │                                                │
│  └─────────────────────────┘                                                │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 📁 Repository Structure

```
IGNIS/
├── IGNIS_CORE/                    # Flight computer firmware
│   └── Firmware/
│       ├── src/
│       │   ├── main.cpp           # Orchestrator (setup/loop only)
│       │   ├── sensors/           # MPU6500, MS5611, I2C scanner
│       │   ├── control/           # Complementary filter, PID, TVC
│       │   ├── telemetry/         # UART protocol
│       │   └── utils/             # Serial comm, helpers
│       ├── include/config.h       # Pin definitions, constants
│       └── platformio.ini
│
├── IGNIS_CTRL/                    # Mechanical TVC design
│   └── Mechanical_Design/
│       ├── CAD_Files/             # STEP, STL, FreeCAD files
│       ├── Manufacturing/         # 3D printing, CNC files
│       └── Assembly/              # Assembly guides
│
├── IGNIS_NET/                     # Data logger firmware
│   └── Firmware/
│       ├── src/                   # Nano code (SD, LoRa, GPS)
│       └── platformio.ini
│
├── IGNIS_NET_SIM/                 # Python flight simulator
│   ├── src/
│   │   ├── main.py                # PyQt6 GUI
│   │   ├── data/data_loader.py   # CSV parser
│   │   ├── gui/main_window.py    # Main window
│   │   └── utils/                 # KML export, math, geo
│   ├── data/flight_logs/         # Sample CSV files
│   └── requirements.txt
│
├── ignis-website/                 # Astro project website
│   ├── src/
│   │   ├── layouts/Layout.astro
│   │   ├── pages/                 # index, about, hardware, etc.
│   │   ├── components/            # Header, Footer, ModelViewer
│   │   └── styles/global.css
│   ├── public/models/             # GLB/GLTF 3D models
│   └── astro.config.mjs
│
├── docs/                          # Documentation
│   ├── getting-started.md
│   ├── wiring-guide.md
│   ├── calibration.md
│   └── troubleshooting.md
│
├── hardware/                      # PCB designs (KiCad)
│   ├── IGNIS_CORE_PCB/
│   └── IGNIS_NET_PCB/
│
└── README.md                      # You are here
```

---

## 🔧 Hardware Requirements

### IGNIS-CORE (Flight Computer)

| Component | Model | Quantity | Approx. Cost |
|-----------|-------|----------|--------------|
| Microcontroller | STM32F411CEU6 Black Pill | 1 | $8 |
| IMU | MPU6500 (6-axis) | 1 | $10 |
| Barometer | MS5611 | 1 | $12 |
| GPS | NEO-6M | 1 | $15 |
| Servos | High-torque (9g-20g) | 2 | $10-20 |
| NeoPixel | WS2812 (single LED) | 1 | $1 |
| Buzzer | 5V piezo | 1 | $1 |
| **Total** | | | **~$55-65** |

### IGNIS-NET (Data Logger)

| Component | Model | Quantity | Approx. Cost |
|-----------|-------|----------|--------------|
| Microcontroller | Arduino Nano | 1 | $4 |
| SD Card Module | SPI | 1 | $3 |
| LoRa Module | SX1278 (868/915MHz) | 1 | $10 |
| **Total** | | | **~$17** |

### IGNIS-CTRL (Mechanical)

| Component | Material | Quantity | Approx. Cost |
|-----------|----------|----------|--------------|
| Filament | PLA/PETG | 200g | $5 |
| Screws | M2/M3 | 10 | $2 |
| **Total** | | | **~$7** |

**Grand Total: ~$80-90**

---

## 🔌 Wiring Diagram

### STM32F411 Black Pill Connections

| Peripheral | STM32 Pin | Notes |
|------------|-----------|-------|
| **MPU6500 (I2C)** | PB6 (SCL), PB7 (SDA) | Pull-ups: 4.7kΩ to 3.3V |
| **MS5611 (I2C)** | PB6 (SCL), PB7 (SDA) | Same bus as MPU6500 |
| **NEO-6M (UART)** | PA9 (TX), PA10 (RX) | 9600 baud |
| **Servo X (Pitch)** | PA0 (PWM) | TIM2_CH1 |
| **Servo Y (Yaw)** | PA1 (PWM) | TIM2_CH2 |
| **WS2812 NeoPixel** | PB8 | Single LED |
| **Buzzer** | PA8 | Active buzzer |
| **UART to Nano** | PA9 (TX), PA10 (RX) | 115200 baud |

### Arduino Nano Connections

| Peripheral | Nano Pin | Notes |
|------------|----------|-------|
| **UART from STM32** | D0 (RX), D1 (TX) | 115200 baud |
| **SD Card (SPI)** | D10(CS), D11(MOSI), D12(MISO), D13(SCK) | Use separate CS |
| **LoRa (SPI)** | D4(CS), D11-13 shared | Different CS pin |
| **GPS (UART, optional)** | D2 (RX), D3 (TX) | 9600 baud |

> ⚠️ **CRITICAL WARNING:** The WeAct Black Pill's 5V pin is DIRECTLY connected to USB 5V. **Never power the 5V pin and USB simultaneously.** The 3.3V regulator is fragile; if it dies, the board still works via USB but cannot power sensors externally.

---

## 💻 Software Setup

### Prerequisites

| Tool | Purpose | Installation |
|------|---------|--------------|
| **VS Code** | IDE | [code.visualstudio.com](https://code.visualstudio.com/) |
| **PlatformIO** | STM32 build/upload | VS Code extension |
| **Python 3.9+** | Simulator | [python.org](https://python.org/) |
| **Git** | Version control | [git-scm.com](https://git-scm.com/) |
| **Node.js** | Website | [nodejs.org](https://nodejs.org/) |

### Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/IGNIS.git
cd IGNIS
```

### Build and Upload STM32 Firmware

```bash
cd IGNIS_CORE/Firmware
pio run
pio run --target upload
pio device monitor
```

### Run the Flight Simulator

```bash
cd IGNIS_NET_SIM
python -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate
pip install -r requirements.txt
python src/main.py
```

### Run the Website Locally

```bash
cd ignis-website
npm install
npm run dev
```

---

## 🚀 Current Status

| Component | Status | Notes |
|-----------|--------|-------|
| **I2C Communication** | ✅ Working | MPU6500 (0x68), MS5611 (0x77) detected |
| **MPU6500 Driver** | ✅ Working | Raw accel/gyro data |
| **MS5611 Driver** | ✅ Working | Pressure, temperature, altitude |
| **Complementary Filter** | ⏳ Not yet | Pitch/roll from accel+gyro |
| **PID Control** | ⏳ Not yet | Servo control algorithm |
| **Servo Control** | ⏳ Not yet | Hardware PWM |
| **UART Protocol** | ⏳ Not yet | STM32 ↔ Nano communication |
| **SD Card Logging** | ⏳ Not yet | Nano side |
| **LoRa Telemetry** | ⏳ Not yet | Nano side |
| **Flight Simulator** | 🟡 Partial | Graphs work, live playback needs fixes |
| **Website** | ✅ Live | [project-ignis.shourya.page](https://project-ignis.shourya.page) |
| **PCB Design** | ⏳ Not started | KiCad |
| **CAD Models** | ⏳ Not started | FreeCAD/Fusion 360 |

---

## 🤝 How to Contribute

We welcome contributions of all kinds! See [CONTRIBUTING.md](CONTRIBUTING.md) for full guidelines.

### Quick Start for Contributors

1. **Fork the repository**
2. **Clone your fork**
3. **Create a feature branch**
4. **Make your changes**
5. **Submit a Pull Request**

### Areas Needing Help

| Area | Skill Level | Files |
|------|-------------|-------|
| Complementary filter | Intermediate | `src/control/tvc.cpp` |
| PID controller | Intermediate | `src/control/tvc.cpp` |
| Flight simulator live graphs | Intermediate | `IGNIS_NET_SIM/src/gui/` |
| UART protocol | Intermediate | `src/telemetry/uart_protocol.cpp` |
| 3D models (CAD) | Beginner/Intermediate | `IGNIS_CTRL/Mechanical_Design/` |
| PCB design (KiCad) | Advanced | `hardware/` |
| Documentation | Beginner | `docs/` |
| Website content | Beginner | `ignis-website/src/pages/` |

### Use the AI Assistant

Copy [this master prompt](docs/AI_ASSISTANT_PROMPT.md) into any AI (ChatGPT, Claude, DeepSeek) to get instant, context-aware help with IGNIS.

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [Getting Started](docs/getting-started.md) | First-time setup guide |
| [Wiring Guide](docs/wiring-guide.md) | Detailed pin connections |
| [Calibration Guide](docs/calibration.md) | Sensor calibration procedure |
| [Troubleshooting](docs/troubleshooting.md) | Common issues and fixes |
| [API Reference](docs/api-reference.md) | Function documentation |

---

## 🛠️ Tool Stack

| Category | Tools |
|----------|-------|
| **Firmware** | PlatformIO, STM32CubeMX, Arduino IDE |
| **PCB** | KiCad, LTspice |
| **CAD** | FreeCAD, Fusion 360 |
| **Simulation** | Ansys Student, GNU Octave, Python |
| **Website** | Astro, Netlify |
| **Version Control** | Git, GitHub |

All tools are **cross-platform** (Windows/macOS/Linux) and **free/open-source** or have free tiers.

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

Hardware designs are under **CERN Open Hardware License**.

---

## 🙏 Acknowledgments

- **Shourya** - Project creator and maintainer
- **Open source community** - For libraries and tools
- **You** - For contributing!

---

## 📞 Contact & Community

| Platform | Link |
|----------|------|
| **Website** | [project-ignis.shourya.page](https://project-ignis.shourya.page) |
| **GitHub** | [github.com/shouryapandey/IGNIS](https://github.com/shouryapandey/IGNIS) |
| **Discord** | [discord.gg/ignis](https://discord.gg/ignis) |
| **Email** | ignis.project@example.com |

---

<div align="center">
  
  **🔥 IGNIS - Make your rocket fly straight 🔥**
  
  *Built with passion, open for everyone*
  
</div>
```

---

## ✅ What's Included

| Section | Status |
|---------|--------|
| Badges + Overview | ✅ |
| Architecture diagram | ✅ |
| Repository structure | ✅ |
| Hardware requirements + cost | ✅ |
| Wiring diagram summary | ✅ |
| Software setup | ✅ |
| Current status table | ✅ |
| How to contribute | ✅ |
| Documentation links | ✅ |
| Tool stack | ✅ |
| License | ✅ |
| Contact/Community | ✅ |
