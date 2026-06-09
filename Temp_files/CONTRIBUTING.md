# Contributing to IGNIS

First off, thank you for considering contributing to IGNIS! 🎉 It's people like you that make open-source rocketry accessible to everyone.

## 📌 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Your First Contribution](#your-first-contribution)
- [Pull Request Process](#pull-request-process)
- [Style Guidelines](#style-guidelines)
- [Development Setup](#development-setup)
- [Contributor Access Levels](#contributor-access-levels-who-can-touch-what)
- [Getting Help](#getting-help)

---

## 📜 Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [ignis.project@example.com](mailto:ignis.project@example.com).

---

## 🤔 How Can I Contribute?

### 🎯 Choose Your Path

| Area | What You Can Do | Skill Level | Time Estimate |
|------|----------------|-------------|---------------|
| **Firmware** | Complementary filter, PID, sensor drivers | Intermediate | 5-20 hrs |
| **Hardware** | PCB design (KiCad), component selection | Intermediate | 10-30 hrs |
| **CAD** | 3D models (FreeCAD/Fusion), TVC mechanism | Beginner/Intermediate | 5-20 hrs |
| **Simulator** | PyQt6 GUI, matplotlib plots, HIL mode | Intermediate | 10-25 hrs |
| **Website** | Astro components, CSS, content, 3D models | Beginner/Intermediate | 2-10 hrs |
| **Documentation** | Guides, tutorials, translations | Beginner | 1-5 hrs |
| **Testing** | Hardware testing, bug reports | Beginner | 1-5 hrs |

### 🐛 Reporting Bugs

**Before submitting a bug report:**
- Check if the issue already exists in [Issues](https://github.com/shouryapandey/IGNIS/issues)
- Check the [Troubleshooting Guide](docs/troubleshooting.md)

**When submitting a bug report:**
- Use the **Bug Report** template
- Include **steps to reproduce**
- Include **expected vs actual behavior**
- Include **screenshots/videos** if applicable
- Include **hardware details** (board version, sensors, etc.)
- Include **software versions** (PlatformIO, Python, OS)

### ✨ Suggesting Enhancements

**Use the **Feature Request** template and include:**
- A clear description of the enhancement
- Why it's valuable to IGNIS
- Any implementation ideas you have
- If it's hardware, include component suggestions

### 📝 Improving Documentation

Documentation is **critical** for an open-source project. You can improve:

| File | Location | Priority |
|------|----------|----------|
| README.md | `/` | High |
| Getting Started Guide | `docs/getting-started.md` | High |
| Wiring Guide | `docs/wiring-guide.md` | High |
| Calibration Guide | `docs/calibration.md` | Medium |
| Troubleshooting Guide | `docs/troubleshooting.md` | Medium |
| API Reference | `docs/api-reference.md` | Low |
| Translations | Any `.md` file | Medium |

---

## 🚀 Your First Contribution

### Step 1: Find an Issue

Look for issues labeled:
- `good-first-issue` - Perfect for beginners
- `help-wanted` - Actively seeking contributors
- `documentation` - Non-code contributions
- `bug` - Confirmed issues to fix

### Step 2: Fork and Clone

```bash
# Fork the repository on GitHub first, then:
git clone https://github.com/YOUR_USERNAME/IGNIS.git
cd IGNIS
git remote add upstream https://github.com/shouryapandey/IGNIS.git
```

### Step 3: Create a Branch

```bash
# Use a descriptive branch name
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-you-are-fixing
```

### Step 4: Make Your Changes

Follow the [Style Guidelines](#style-guidelines) below.

### Step 5: Test Your Changes

```bash
# For firmware
cd IGNIS_CORE/Firmware
pio run

# For simulator
cd IGNIS_NET_SIM
python src/main.py

# For website
cd ignis-website
npm run dev
```

### Step 6: Commit and Push

```bash
git add .
git commit -m "Brief description of what you changed"
git push origin feature/your-feature-name
```

### Step 7: Open a Pull Request

Go to GitHub and click **"Compare & pull request"**. Fill out the template.

---

## 📋 Pull Request Process

### PR Checklist

Before submitting, ensure:

- [ ] Your code compiles without errors
- [ ] You have tested your changes on real hardware (if applicable)
- [ ] You have updated documentation (if applicable)
- [ ] You have followed style guidelines
- [ ] Your commit messages are clear
- [ ] You have linked any related issues

### PR Template

```markdown
## Description
Brief description of what this PR does.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Hardware design
- [ ] Other: _____

## Testing Performed
- [ ] Tested on real hardware
- [ ] Simulator tested
- [ ] Website tested locally

## Related Issues
Closes #(issue number)

## Screenshots (if applicable)
```

### After Submitting

- A maintainer will review within 2-3 days
- You may be asked to make changes
- Once approved, your PR will be merged

---

## 📐 Style Guidelines

### C++ (STM32 Firmware)

```cpp
// Use 4 spaces for indentation (no tabs)
// Use camelCase for variables
// Use PascalCase for classes
// Use UPPER_CASE for constants

// Good example
int sensorValue = 0;
const int I2C_ADDRESS = 0x68;

void readSensor() {
    for (int i = 0; i < 10; i++) {
        // do something
    }
}

// Comments explain WHY, not WHAT
// Use Doxygen style for functions
/**
 * @brief Calculates complementary filter angle
 * @param gyro_rate Rate from gyroscope (deg/s)
 * @param accel_angle Angle from accelerometer (deg)
 * @param dt Time delta (seconds)
 * @return Filtered angle (deg)
 */
float complementaryFilter(float gyro_rate, float accel_angle, float dt);
```

### Python (Simulator)

```python
# Use 4 spaces for indentation
# Use snake_case for variables and functions
# Use PascalCase for classes
# Use UPPER_CASE for constants

# Good example
sensor_value = 0
I2C_ADDRESS = 0x68

def read_sensor():
    for i in range(10):
        # do something
        pass

class SensorData:
    def __init__(self):
        self.pitch = 0.0
```

### Astro/JavaScript (Website)

```javascript
// Use 2 spaces for indentation
// Use camelCase for variables and functions
// Use PascalCase for components

// Good example
const sensorValue = 0;

function calculatePitch() {
    for (let i = 0; i < 10; i++) {
        // do something
    }
}

// Astro component frontmatter
---
import Layout from '../layouts/Layout.astro';
const title = "IGNIS";
---
```

### Markdown (Documentation)

```markdown
# Heading 1 (H1)
## Heading 2 (H2)
### Heading 3 (H3)

**Bold text**
*Italic text*
`inline code`

```code block```

- List item 1
- List item 2

1. Numbered item 1
2. Numbered item 2

[Link text](URL)
![Image alt](image-url)
```

---

## 💻 Development Setup

### Firmware (STM32)

```bash
# Prerequisites: VS Code + PlatformIO extension
cd IGNIS_CORE/Firmware
pio run                # Build
pio run --target upload # Upload to STM32
pio device monitor     # Serial monitor (115200 baud)
```

### Simulator (Python)

```bash
cd IGNIS_NET_SIM
python -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate
pip install -r requirements.txt
python src/main.py
```

### Website (Astro)

```bash
cd ignis-website
npm install
npm run dev    # Development server at localhost:4321
npm run build  # Production build to /dist
```

### PCB (KiCad)

```bash
# Install KiCad from https://www.kicad.org/
# Open hardware/IGNIS_CORE_PCB/IGNIS_CORE.pro
```

### CAD (FreeCAD)

```bash
# Install FreeCAD from https://www.freecad.org/
# Open IGNIS_CTRL/Mechanical_Design/CAD_Files/*.FCStd
```

---

## 🔐 Contributor Access Levels (Who Can Touch What)

Not every contributor should modify core files. Some changes affect everyone else's work. This section defines **who can touch what** and the **review requirements**.

### 📊 Access Matrix

| Area | File Path | Write Access | Required Reviewers | Notes |
|------|-----------|--------------|--------------------|-------|
| **Core Firmware** | `IGNIS_CORE/Firmware/src/control/` | Maintainers only | 2 maintainers | Affects flight stability |
| **Core Firmware** | `IGNIS_CORE/Firmware/src/sensors/mpu6500.cpp` | Maintainers only | 1 maintainer | Affects all sensor readings |
| **Core Firmware** | `IGNIS_CORE/Firmware/src/sensors/ms5611.cpp` | Maintainers only | 1 maintainer | Affects altitude data |
| **Core Firmware** | `IGNIS_CORE/Firmware/include/config.h` | Maintainers only | 1 maintainer | Changes pin mappings |
| **Peripheral Firmware** | `IGNIS_CORE/Firmware/src/telemetry/` | Anyone | 1 maintainer | Can't break control loop |
| **Peripheral Firmware** | `IGNIS_CORE/Firmware/src/utils/` | Anyone | 1 maintainer | Helper functions only |
| **Nano Firmware** | `IGNIS_NET/Firmware/src/` | Anyone | 1 maintainer | Logging only, not critical |
| **Simulator** | `IGNIS_NET_SIM/src/` | Anyone | 1 maintainer | No hardware impact |
| **Website** | `ignis-website/src/` | Anyone | 1 maintainer | No hardware impact |
| **Documentation** | `docs/`, `*.md` | Anyone | None (self-merge) | Typos, clarifications |
| **PCB Design** | `hardware/*.kicad_pcb` | Maintainers only | 1 maintainer | Changes physical board |
| **CAD Models** | `IGNIS_CTRL/Mechanical_Design/` | Anyone | 1 maintainer | 3D prints, can iterate |

### 👥 Contributor Roles

| Role | Criteria | Permissions | How to Get |
|------|----------|-------------|------------|
| **User** | Anyone | Read-only, file issues | Automatic |
| **Contributor** | 1+ accepted PR | Write to docs, website, simulator | Submit PR |
| **Peripheral Maintainer** | 3+ accepted PRs in same area | Write to peripheral firmware, Nano code | Nominated by maintainer |
| **Core Maintainer** | 5+ accepted PRs + hardware access | Write to core firmware, PCB, config | Invite only |
| **Project Lead** | Creator | Full access | N/A |

### 🚫 What Contributors CANNOT Do (Without Approval)

| Action | Why Restricted |
|--------|----------------|
| Change I2C pin assignments | Breaks all sensor connections |
| Modify MPU6500 I2C address | Breaks IMU detection |
| Change MS5611 I2C address | Breaks barometer detection |
| Modify control loop frequency | Affects rocket stability |
| Change PID constants without testing | Could cause oscillations |
| Modify UART protocol without versioning | Breaks STM32↔Nano communication |
| Change SD card SPI pins | Breaks logging for everyone |
| Modify config.h without review | Affects all users |

### ✅ Safe Areas for New Contributors

**These files are SAFE to modify without breaking others:**

```bash
# Documentation (anyone can edit)
docs/*.md
README.md
CONTRIBUTING.md

# Website content (anyone can edit)
ignis-website/src/pages/*.astro
ignis-website/src/components/*.astro
ignis-website/src/styles/*.css

# Simulator (anyone can edit)
IGNIS_NET_SIM/src/gui/*.py
IGNIS_NET_SIM/src/utils/*.py

# Nano firmware (anyone can edit - logging only)
IGNIS_NET/Firmware/src/*.cpp

# 3D CAD models (anyone can edit - mechanical only)
IGNIS_CTRL/Mechanical_Design/CAD_Files/*.stl
IGNIS_CTRL/Mechanical_Design/CAD_Files/*.step
```

### ⚠️ Restricted Areas (Maintainers Only)

**These files require maintainer approval:**

```bash
# Core control logic
IGNIS_CORE/Firmware/src/control/tvc.cpp
IGNIS_CORE/Firmware/src/control/tvc.h

# Sensor drivers (critical)
IGNIS_CORE/Firmware/src/sensors/mpu6500.cpp
IGNIS_CORE/Firmware/src/sensors/mpu6500.h
IGNIS_CORE/Firmware/src/sensors/ms5611.cpp
IGNIS_CORE/Firmware/src/sensors/ms5611.h

# Configuration (affects all)
IGNIS_CORE/Firmware/include/config.h

# PCB designs (physical hardware)
hardware/*.kicad_pcb
hardware/*.kicad_sch
```

### 🔄 Pull Request Review Requirements by Type

| PR Type | Minimum Reviewers | Auto-Merge Allowed? | Wait Time |
|---------|-------------------|---------------------|-----------|
| **Documentation fix** | 0 (self-merge) | ✅ Yes | Instant |
| **Website content** | 0 (self-merge) | ✅ Yes | Instant |
| **Simulator improvement** | 1 maintainer | ❌ No | 24 hours |
| **Nano firmware** | 1 maintainer | ❌ No | 24 hours |
| **CAD model** | 1 maintainer | ❌ No | 48 hours |
| **Peripheral firmware** | 1 maintainer | ❌ No | 48 hours |
| **Core firmware** | 2 maintainers | ❌ No | 72 hours |
| **PCB design** | 2 maintainers | ❌ No | 72 hours |
| **Config.h change** | Project Lead | ❌ No | 72 hours |

### 📝 How to Request Core Access

If you've made valuable contributions and want to work on core files:

1. **Accumulate 3+ accepted PRs** in peripheral areas
2. **Open an issue** titled "Request for Core Access"
3. **List your contributions** with links to PRs
4. **Demonstrate hardware access** (you can test on real STM32)
5. **Wait for maintainer vote** (requires 2 maintainers' approval)

### 🧪 Testing Requirements Before Core PRs

**If you have core access, you MUST test on REAL HARDWARE before submitting:**

| Change Type | Required Tests |
|-------------|----------------|
| I2C pin change | Run I2C scanner, verify both sensors detected |
| MPU6500 change | Read raw accel/gyro, verify values make sense |
| MS5611 change | Read pressure/temp, verify within expected range |
| PID change | Bench test with servos, log response |
| Config.h change | Build, upload, run basic functionality |

**"It compiles" is NOT enough for core changes.**

### 🚨 Emergency Rollback Procedure

If a core change breaks functionality:

1. **Project Lead** reverts the PR immediately
2. **Author** is notified with explanation
3. **Fix must be tested** before re-submission
4. **Broken PR is flagged** for future reference

### 💡 Summary Table

| Your Role | Can Edit Docs | Can Edit Website | Can Edit Simulator | Can Edit Nano | Can Edit CAD | Can Edit Peripheral FW | Can Edit Core FW | Can Edit PCB |
|-----------|---------------|------------------|--------------------|---------------|--------------|------------------------|------------------|--------------|
| **User** | ❌ (file issue) | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| **Contributor** | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ |
| **Peripheral Maintainer** | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ |
| **Core Maintainer** | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ |
| **Project Lead** | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

### 🎯 Bottom Line

- **New contributors:** Start with documentation, website, or simulator
- **Intermediate:** Move to Nano firmware or CAD models
- **Advanced:** Request peripheral firmware access
- **Experts only:** Core firmware and PCB (requires hardware testing)

**This protects the project while remaining open to all.** 🔥

---

## 🆘 Getting Help

### Use the AI Assistant

Copy [this master prompt](docs/AI_ASSISTANT_PROMPT.md) into any AI (ChatGPT, Claude, DeepSeek) for instant context-aware help.

### Ask Questions

| Platform | Where |
|----------|-------|
| **GitHub Issues** | For bugs and feature requests |
| **Discord** | For real-time chat and quick questions |
| **Email** | For private inquiries |

### Common Issues

| Issue | Solution |
|-------|----------|
| STM32 won't upload | Hold BOOT0, press NRST, release BOOT0 |
| I2C sensors not detected | Check pull-up resistors (4.7kΩ to 3.3V) |
| Simulator crashes | Ensure Python 3.9+ and all dependencies installed |
| Website won't build | Run `npm install` again, clear `node_modules` |

---

## 🌟 Recognition

All contributors will be added to [CONTRIBUTORS.md](CONTRIBUTORS.md). Significant contributions may be featured on the [website](https://project-ignis.shourya.page).

---

## 📄 License

By contributing, you agree that your contributions will be licensed under the **MIT License** (code) and **CERN OHL** (hardware designs).

---

<div align="center">
  
  **Thank you for helping make IGNIS better! 🔥**
  
  *Every contribution matters, no matter how small.*
  
</div>
```
