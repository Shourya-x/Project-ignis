export interface PinEntry {
  lqfpPin: string;
  gpio: string;
  peripheral: string;
  signal: string;
  subsystem: 'Power/Clock' | 'I2C2 Sensors' | 'SPI1 LoRa' | 'SPI2 Storage' | 'TIM2 Servos' | 'Pyro/Audio/LED' | 'GPS/USB/SWD';
  targetDevice: string;
  notes: string;
}

export interface BomItem {
  board: 'IGNIS-CORE' | 'AUX-DATALOGGER';
  ref: string;
  qty: number;
  value: string;
  package: string;
  category: 'MCU & Logic' | 'Sensors' | 'RF & Navigation' | 'Power & Pyro' | 'Memory & RTC' | 'Passives & Crystals' | 'Connectors';
  description: string;
}

export interface FirmwareModule {
  name: string;
  path: string;
  peripheral: string;
  rateOrBus: string;
  summary: string;
  keyFunctions: string[];
}

export interface BuildSession {
  id: string;
  phase: string;
  date: string;
  steps: number;
  title: string;
  subsystem: string;
  summary: string;
  highlights: string[];
}

export const SYSTEM_METRICS = [
  { label: 'FLIGHT MCU', value: 'STM32G473VET6', sub: '170 MHz Cortex-M4F (144 MHz PLL) • LQFP-100' },
  { label: 'MATH CO-PROCESSORS', value: 'CORDIC + FMAC', sub: 'Hardware Trig & IIR Biquad Filter Engine' },
  { label: 'GNC CONTROL LOOP', value: '100 Hz (10 ms)', sub: 'Complementary (α=0.98) + 3-Axis PID + X-Mix' },
  { label: 'FIN / TVC ACTUATION', value: '4× 50 Hz PWM', sub: 'TIM2_CH1..4 (PA0–PA3) • 1000–2000 µs' },
  { label: 'PYRO IGNITER STAGES', value: '3× MOSFET', sub: '2× IRLZ44N (PC7/8) + 2×BC547/IRF520 (PC9)' },
  { label: 'TELEMETRY & STORAGE', value: '433 MHz + Dual NVM', sub: 'Ra-02 SX1278 + 24LC16B + Aux 256 KB Blackbox' },
];

export const STM32_PINOUT: PinEntry[] = [
  { lqfpPin: '12, 13', gpio: 'PH0, PH1', peripheral: 'RCC_OSC_IN / OUT', signal: 'HSE_8MHZ', subsystem: 'Power/Clock', targetDevice: 'X1 (8.000 MHz HC-49US)', notes: 'PLL ×36 / 2 = 144 MHz SYSCLK, 48 MHz USB clock' },
  { lqfpPin: '8, 9', gpio: 'PC14, PC15', peripheral: 'RCC_OSC32_IN / OUT', signal: 'LSE_32K', subsystem: 'Power/Clock', targetDevice: 'X2 (32.768 kHz ABS05)', notes: 'Low-drift RTC & hardware timestamp crystal' },
  { lqfpPin: '30', gpio: 'PC4', peripheral: 'I2C2_SCL (AF4)', signal: 'I2C2_SCL', subsystem: 'I2C2 Sensors', targetDevice: 'MPU-6500 / MS5611 / HMC5883L / 24LC16B', notes: 'Hardware I2C2_SCL only (4.7 kΩ pull-up to +3.3V)' },
  { lqfpPin: '69', gpio: 'PA8', peripheral: 'I2C2_SDA (AF4)', signal: 'I2C2_SDA', subsystem: 'I2C2 Sensors', targetDevice: 'MPU-6500 / MS5611 / HMC5883L / 24LC16B', notes: 'Hardware I2C2_SDA only (4.7 kΩ pull-up to +3.3V)' },
  { lqfpPin: '22', gpio: 'PA0', peripheral: 'TIM2_CH1 (AF1)', signal: 'SERVO_FIN1', subsystem: 'TIM2 Servos', targetDevice: 'Fin 1 Servo (45° / Q1: +X, +Y)', notes: '50 Hz PWM: -0.707·P - 0.707·Y + R mix (90° = 1500 µs)' },
  { lqfpPin: '23', gpio: 'PA1', peripheral: 'TIM2_CH2 (AF1)', signal: 'SERVO_FIN2', subsystem: 'TIM2 Servos', targetDevice: 'Fin 2 Servo (135° / Q2: -X, +Y)', notes: '50 Hz PWM: +0.707·P - 0.707·Y + R mix (90° = 1500 µs)' },
  { lqfpPin: '24', gpio: 'PA2', peripheral: 'TIM2_CH3 (AF1)', signal: 'SERVO_FIN3', subsystem: 'TIM2 Servos', targetDevice: 'Fin 3 Servo (225° / Q3: -X, -Y)', notes: '50 Hz PWM: +0.707·P + 0.707·Y + R mix (90° = 1500 µs)' },
  { lqfpPin: '25', gpio: 'PA3', peripheral: 'TIM2_CH4 (AF1)', signal: 'SERVO_FIN4', subsystem: 'TIM2 Servos', targetDevice: 'Fin 4 Servo (315° / Q4: +X, -Y)', notes: '50 Hz PWM: -0.707·P + 0.707·Y + R mix (90° = 1500 µs)' },
  { lqfpPin: '90', gpio: 'PB3', peripheral: 'SPI1_SCK (AF5)', signal: 'LORA_SCK', subsystem: 'SPI1 LoRa', targetDevice: 'Ai-Thinker Ra-02 (SX1278)', notes: 'Contiguous LQFP-100 row (Pins 90–95)' },
  { lqfpPin: '91', gpio: 'PB4', peripheral: 'SPI1_MISO (AF5)', signal: 'LORA_MISO', subsystem: 'SPI1 LoRa', targetDevice: 'Ai-Thinker Ra-02 (SX1278)', notes: 'Master In / Slave Out from SX1278' },
  { lqfpPin: '92', gpio: 'PB5', peripheral: 'SPI1_MOSI (AF5)', signal: 'LORA_MOSI', subsystem: 'SPI1 LoRa', targetDevice: 'Ai-Thinker Ra-02 (SX1278)', notes: 'Master Out / Slave In to SX1278' },
  { lqfpPin: '93', gpio: 'PB6', peripheral: 'GPIO_Output', signal: 'LORA_RST', subsystem: 'SPI1 LoRa', targetDevice: 'Ai-Thinker Ra-02 (SX1278)', notes: 'Active-low reset line (initialized HIGH)' },
  { lqfpPin: '94', gpio: 'PB7 / PA15', peripheral: 'GPIO_Output', signal: 'LORA_CS', subsystem: 'SPI1 LoRa', targetDevice: 'Ai-Thinker Ra-02 (SX1278)', notes: 'SPI1 NSS chip select (initialized HIGH)' },
  { lqfpPin: '95', gpio: 'PB8', peripheral: 'GPIO_EXTI8', signal: 'LORA_DIO0', subsystem: 'SPI1 LoRa', targetDevice: 'Ai-Thinker Ra-02 (SX1278)', notes: 'Rising-edge interrupt (EXTI9_5_IRQn) on Tx/Rx done' },
  { lqfpPin: '96', gpio: 'PB9', peripheral: 'TIM4_CH4 + DMA1_CH1', signal: 'WS2812_DI', subsystem: 'Pyro/Audio/LED', targetDevice: 'WS2812B 5050 RGB LED', notes: '800 kHz PWM+DMA (2 preamble + 24 bit + 240 reset slots)' },
  { lqfpPin: '65', gpio: 'PC6', peripheral: 'GPIO_Output', signal: 'BUZZER', subsystem: 'Pyro/Audio/LED', targetDevice: '5V Piezo Buzzer via BC547', notes: '1 kΩ base resistor → BC547 NPN low-side + 1N4007 flyback' },
  { lqfpPin: '66', gpio: 'PC7', peripheral: 'GPIO_Output', signal: 'PYRO_MOTOR', subsystem: 'Pyro/Audio/LED', targetDevice: 'Q1: IRLZ44N (Motor Igniter)', notes: 'Direct 3.3V logic gate drive (100 Ω series, 10 kΩ pull-down)' },
  { lqfpPin: '67', gpio: 'PC8', peripheral: 'GPIO_Output', signal: 'PYRO_SEP', subsystem: 'Pyro/Audio/LED', targetDevice: 'Q2: IRLZ44N (Stage Separation)', notes: 'Direct 3.3V logic gate drive (100 Ω series, 10 kΩ pull-down)' },
  { lqfpPin: '68', gpio: 'PC9', peripheral: 'GPIO_Output', signal: 'PYRO_CHUTE', subsystem: 'Pyro/Audio/LED', targetDevice: 'Q3: IRF520 (Chute Deployment)', notes: 'Dual-BC547 NPN level-shifter pulling gate to +V_BAT via 1 kΩ' },
  { lqfpPin: '51', gpio: 'PB12', peripheral: 'GPIO_Output', signal: 'SD_CS', subsystem: 'SPI2 Storage', targetDevice: 'microSD Card Slot', notes: 'Active-low SPI2 chip select (initialized HIGH)' },
  { lqfpPin: '52', gpio: 'PB13', peripheral: 'SPI2_SCK (AF5)', signal: 'SD_SCK', subsystem: 'SPI2 Storage', targetDevice: 'microSD Card Slot', notes: 'High-speed SPI2 clock' },
  { lqfpPin: '53', gpio: 'PB14', peripheral: 'SPI2_MISO (AF5)', signal: 'SD_MISO', subsystem: 'SPI2 Storage', targetDevice: 'microSD Card Slot', notes: 'SPI2 MISO (DAT0)' },
  { lqfpPin: '54', gpio: 'PB15', peripheral: 'SPI2_MOSI (AF5)', signal: 'SD_MOSI', subsystem: 'SPI2 Storage', targetDevice: 'microSD Card Slot', notes: 'SPI2 MOSI (CMD)' },
  { lqfpPin: '70, 71', gpio: 'PA9, PA10', peripheral: 'USART1_TX / RX (AF7)', signal: 'GPS_RXD / TXD', subsystem: 'GPS/USB/SWD', targetDevice: 'u-blox NEO-7M GPS', notes: '9600 baud NMEA position & UTC timing stream' },
  { lqfpPin: '72, 73', gpio: 'PA11, PA12', peripheral: 'USB_DM / USB_DP', signal: 'USB_CDC', subsystem: 'GPS/USB/SWD', targetDevice: 'USB1 (USBC1-AF-046)', notes: '12 Mbps USB CDC streaming 100 Hz CSV telemetry & CLI' },
  { lqfpPin: '76, 79', gpio: 'PA13, PA14', peripheral: 'SYS_JTMS / JTCK', signal: 'SWDIO / SWCLK', subsystem: 'GPS/USB/SWD', targetDevice: 'ST-Link V2 Debug Header', notes: 'SWD flashing & live hardware register inspection' },
];

export const BOM_ITEMS: BomItem[] = [
  { board: 'IGNIS-CORE', ref: 'U3', qty: 1, value: 'STM32G473VET6', package: 'LQFP-100 (14×14mm)', category: 'MCU & Logic', description: '170 MHz ARM Cortex-M4F MCU, 512 KB ECC Flash, 128 KB SRAM, CORDIC & FMAC' },
  { board: 'IGNIS-CORE', ref: 'U7', qty: 1, value: 'MPU-6500', package: 'QFN-24 (3×3mm)', category: 'Sensors', description: '6-Axis MEMS Accelerometer (±8g) + Gyroscope (±1000°/s) on I2C2 (0x68)' },
  { board: 'IGNIS-CORE', ref: 'U8', qty: 1, value: 'MS5611-01BA03', package: 'LGA-8 (5×3mm)', category: 'Sensors', description: '24-bit ΔΣ High-Resolution Barometric Pressure Altimeter on I2C2 (0x77)' },
  { board: 'IGNIS-CORE', ref: 'U9', qty: 1, value: 'BMM150 / HMC5883L', package: 'WLCSP-12 / LCC-16', category: 'Sensors', description: '3-Axis Geomagnetic Sensor for Roll-Stabilized Heading on I2C2 (0x1E)' },
  { board: 'IGNIS-CORE', ref: 'U10', qty: 1, value: 'Ai-Thinker Ra-02 (SX1278)', package: 'SMD-16 Module', category: 'RF & Navigation', description: '433 MHz LoRa Spread-Spectrum Transceiver Module on SPI1 (>5 km LOS)' },
  { board: 'IGNIS-CORE', ref: 'MOD1', qty: 1, value: 'u-blox NEO-7M', package: 'External / Header', category: 'RF & Navigation', description: '56-Channel GPS/GNSS Receiver on USART1 (PA9/PA10, 9600 baud)' },
  { board: 'IGNIS-CORE', ref: 'U_EEP', qty: 1, value: '24LC16B', package: 'SOIC-8 / DIP-8', category: 'Memory & RTC', description: '16-Kbit (2048 B) I2C Serial EEPROM (8×256B blocks at 0x50–0x57) for Hot-Start & Cal' },
  { board: 'IGNIS-CORE', ref: 'Q1, Q2', qty: 2, value: 'IRLZ44N', package: 'TO-220-3', category: 'Power & Pyro', description: 'Logic-Level Power N-MOSFET (55V, 47A, Rds(on)=0.035Ω @ 3.3V) for Pyro Ch 1 & 2' },
  { board: 'IGNIS-CORE', ref: 'Q3', qty: 1, value: 'IRF520 + 2× BC547', package: 'TO-220-3 + TO-92', category: 'Power & Pyro', description: '100V Power N-MOSFET driven via Dual-BC547 NPN 3.3V-to-V_BAT Level Shifter (Pyro Ch 3)' },
  { board: 'IGNIS-CORE', ref: 'D1–D3', qty: 3, value: '1N4007', package: 'DO-41 / SMA', category: 'Power & Pyro', description: '1000V 1A Flyback Clamp Diodes across Pyro Igniter Channels 1–3' },
  { board: 'IGNIS-CORE', ref: 'U4', qty: 1, value: 'LM2596 DC-DC', package: 'TO-263 / Module', category: 'Power & Pyro', description: '3A Step-Down Switching Regulator Stage from 2S/3S LiPo Battery' },
  { board: 'IGNIS-CORE', ref: 'U1', qty: 1, value: 'AMS1117-3.3', package: 'SOT-89-3 / SOT-223', category: 'Power & Pyro', description: '3.3V Low-Dropout Linear Regulator for STM32G473, Sensors & LoRa' },
  { board: 'IGNIS-CORE', ref: 'U2', qty: 1, value: 'AMS1117-5.0', package: 'SOT-89-3 / SOT-223', category: 'Power & Pyro', description: '5.0V Linear Regulator for Auxiliary Logic & Buzzer Stage' },
  { board: 'IGNIS-CORE', ref: 'U5, U6', qty: 2, value: '1N5819', package: 'DO-41 / SOD-123', category: 'Power & Pyro', description: '40V 1A Low-Vf Schottky Power-ORing Diodes isolating USB VBUS from Battery' },
  { board: 'IGNIS-CORE', ref: 'X1', qty: 1, value: '8.000 MHz', package: 'HC-49US', category: 'Passives & Crystals', description: 'High-Speed External (HSE) Primary PLL Reference Crystal' },
  { board: 'IGNIS-CORE', ref: 'X2', qty: 1, value: '32.768 kHz', package: 'ABS05 SMD', category: 'Passives & Crystals', description: 'Low-Speed External (LSE) Real-Time Clock Crystal' },
  { board: 'IGNIS-CORE', ref: 'C1–C23', qty: 22, value: '100 nF', package: '0805', category: 'Passives & Crystals', description: 'X7R Ceramic High-Frequency Decoupling Capacitors across VDD/VDDA & Sensors' },
  { board: 'IGNIS-CORE', ref: 'R1–R23', qty: 18, value: '4.7 kΩ / 10 kΩ / 100 Ω', package: '0805', category: 'Passives & Crystals', description: 'I2C Pull-Ups (4.7 kΩ), MOSFET Gate Pull-Downs (10 kΩ), Gate Series Dampers (100 Ω)' },
  { board: 'IGNIS-CORE', ref: 'USB1', qty: 1, value: 'USBC1-AF-046 (TE 2340901-1)', package: 'USB-C SMD', category: 'Connectors', description: 'USB Type-C Receptacle for 12 Mbps Full-Speed CDC Telemetry & CLI' },
  { board: 'IGNIS-CORE', ref: 'J1–J3, H1–H2', qty: 5, value: 'Pin Headers (2.00mm / 2.54mm)', package: 'THT Vertical', category: 'Connectors', description: '4-Ch Servo Bus, Pyro Screw Terminals, SWD Programming & Aux Bus Headers' },
  { board: 'AUX-DATALOGGER', ref: 'U4', qty: 1, value: 'ATmega328P-AU', package: 'TQFP-32', category: 'MCU & Logic', description: 'Dedicated 8-Bit AVR Blackbox Flight Recorder Controller @ 16 MHz' },
  { board: 'AUX-DATALOGGER', ref: 'U1, U2', qty: 2, value: '24LC1025', package: 'SOIC-8', category: 'Memory & RTC', description: 'Dual 1-Mbit (2 × 128 KB = 256 KB) I2C CMOS Serial EEPROM Array (0x50/0x51/0x54/0x55)' },
  { board: 'AUX-DATALOGGER', ref: 'U3', qty: 1, value: 'DS1337S+', package: 'SOIC-8', category: 'Memory & RTC', description: 'Low-Power I2C Real-Time Clock (0x68) with Dual Alarms & Coin-Cell Backup' },
  { board: 'AUX-DATALOGGER', ref: 'Y1, Y2', qty: 2, value: '32.768 kHz & 16.000 MHz', package: 'Cylindrical / HC-49', category: 'Passives & Crystals', description: 'Timekeeping Crystal for DS1337S+ (Y1) & System Clock Crystal for ATmega328P (Y2)' },
  { board: 'AUX-DATALOGGER', ref: 'C1–C5, R1–R7', qty: 12, value: '100 nF, 22 pF, 4.7 kΩ, 10 kΩ, 330 Ω', package: '0805', category: 'Passives & Crystals', description: 'Crystal Load Caps (22 pF), I2C Pull-Ups (4.7 kΩ), LED Ballasts (330 Ω) & Bypass Caps' },
];

export const FIRMWARE_MODULES: FirmwareModule[] = [
  {
    name: 'Flight',
    path: 'Modules/Flight/Flight.c',
    peripheral: 'SysTick + FPU',
    rateOrBus: '100 Hz (10 ms Deterministic)',
    summary: 'Executes the complementary attitude filter (α = 0.98), 3-axis PID controller (Kp=1.2, Ki=0.05, Kd=0.08) with ±100 integral windup clamping, and 4-fin X-configuration aerodynamic mixing.',
    keyFunctions: ['Flight_Init()', 'Flight_Update()', 'ComplementaryFilter_Update()', 'PID_Update()', 'Mixing_Update()']
  },
  {
    name: 'IMU',
    path: 'Modules/IMU/IMU.c',
    peripheral: 'I2C2 (PC4 / PA8)',
    rateOrBus: 'I2C2 @ 0x68 (MPU-6500)',
    summary: 'Configures MPU-6500 for ±8g (4096 LSB/g) and ±1000°/s (32.8 LSB/°/s), performs 14-byte burst register reads from 0x3B, and runs 500-sample stationary bias calibration on boot or USB command.',
    keyFunctions: ['IMU_Init()', 'IMU_ReadRaw()', 'IMU_Calibrate()', 'IMU_GetScaled()']
  },
  {
    name: 'Barometer',
    path: 'Modules/Barometer/Barometer.c',
    peripheral: 'I2C2 (PC4 / PA8)',
    rateOrBus: 'I2C2 @ 0x77 (MS5611-01BA03)',
    summary: 'Reads factory PROM calibration words C[1..6] (0xA2–0xAC), triggers OSR=4096 24-bit ΔΣ pressure/temperature conversions, and computes 64-bit second-order temperature-compensated pressure.',
    keyFunctions: ['Barometer_Init()', 'Barometer_ReadCalibration()', 'Barometer_Read()']
  },
  {
    name: 'Sensors',
    path: 'Modules/Sensors/Sensors.c',
    peripheral: 'I2C2 (PC4 / PA8)',
    rateOrBus: 'I2C2 Bus Scanner + HMC5883L (0x1E)',
    summary: 'Scans all 7-bit I2C2 addresses (0x08–0x77) on boot to verify sensor presence and drives the HMC5883L 3-axis digital compass (handling X, Z, Y register ordering) for magnetic heading.',
    keyFunctions: ['I2C2_ScanBus()', 'HMC5883L_Init()', 'HMC5883L_ReadRaw()', 'HMC5883L_GetHeadingDeg()']
  },
  {
    name: 'Servos',
    path: 'Modules/Servos/Servos.c',
    peripheral: 'TIM2 (CH1–CH4)',
    rateOrBus: '50 Hz Hardware PWM (PA0–PA3)',
    summary: 'Generates jitter-free 50 Hz hardware PWM signals across PA0–PA3 with 1 µs resolution (1000 µs = 0°, 1500 µs = 90° neutral, 2000 µs = 180°) for the 4 X-configuration control fins.',
    keyFunctions: ['Servo_Init()', 'Servo_SetFin1..4()', 'Servo_AllCenter()']
  },
  {
    name: 'WS2812',
    path: 'Modules/WS2812/WS2812.c',
    peripheral: 'TIM4_CH4 + DMA1_CH1',
    rateOrBus: '800 kHz Bitstream on PB9',
    summary: 'Zero-CPU-overhead NeoPixel driver using 2 leading zero preamble slots + 24 GRB duty slots (HIGH=120, LOW=60) + 240 reset slots, keeping CC4E active with CCR4=0 to prevent PB9 floating.',
    keyFunctions: ['WS2812_Init()', 'WS2812_SetRGB()', 'HAL_TIM_PWM_PulseFinishedCallback()']
  },
  {
    name: 'USB',
    path: 'Modules/USB/USB.c',
    peripheral: 'USB_OTG_FS (PA11 / PA12)',
    rateOrBus: '12 Mbps USB CDC (100 Hz Stream)',
    summary: 'Streams 11-column real-time attitude & fin deflection CSV telemetry at 100 Hz and processes interactive single-character ground commands (A=Arm, D=Disarm, C=Calibrate, S=Diagnostic Scan).',
    keyFunctions: ['USB_Init()', 'USB_Print()', 'USB_SendTelemetry()', 'USB_ProcessCommands()']
  },
  {
    name: 'EEPROM',
    path: 'Modules/EEPROM/EEPROM.c',
    peripheral: 'I2C2 (PC4 / PA8)',
    rateOrBus: 'I2C2 @ 0x50–0x57 (24LC16B)',
    summary: 'Implements 8-block 256-byte page-aware read/write routines with 16-byte page boundary splitting and CRC/checksum validation for hot-start flight state recovery.',
    keyFunctions: ['EEPROM_Init()', 'EEPROM_WriteBytes()', 'EEPROM_ReadBytes()', 'EEPROM_SaveHotStart()']
  },
  {
    name: 'LoRa & GPS',
    path: 'Modules/LoRa/ & Modules/GPS/',
    peripheral: 'SPI1 (PB3–PB8) & USART1 (PA9/PA10)',
    rateOrBus: '433 MHz LoRa & 9600 Baud NMEA',
    summary: 'Interfaces with the Ai-Thinker Ra-02 (SX1278) transceiver on contiguous pins PB3–PB8 and the u-blox NEO-7M GPS module on USART1 for long-range downlink and recovery coordinates.',
    keyFunctions: ['LoRa_Init()', 'GPS_Init()']
  },
  {
    name: 'Logger & Storage',
    path: 'Modules/Logger/ & Modules/Storage/',
    peripheral: 'SPI2 (PB12–PB15)',
    rateOrBus: 'SPI2 microSD Blackbox',
    summary: 'Manages high-rate onboard flight data framing and SPI2 microSD card storage alongside the external ATmega328P + Dual 24LC1025 Auxiliary Blackbox.',
    keyFunctions: ['Logger_Init()', 'Storage_Init()']
  }
];

export const BUILD_SESSIONS: BuildSession[] = [
  {
    id: 'a93b670a',
    phase: 'Session 01 • Toolchain Bringup',
    date: 'Oct 28–29, 2026',
    steps: 110,
    title: 'STM32CubeIDE 2.2.0, CubeMX 6.18.1 & Programmer 2.23.0 Setup on Fedora Linux',
    subsystem: 'Dev Environment & SWD Rules',
    summary: 'Installed the complete STMicroelectronics development suite on Fedora 44 Linux, configured ST-Link and SEGGER J-Link udev rules, and resolved the VS Code STM32 extension path discovery bug.',
    highlights: [
      'Installed STM32CubeIDE 2.2.0 (/opt/st/), STM32CubeMX 6.18.1, and STM32CubeProgrammer 2.23.0.',
      'Resolved stmicroelectronics.stm32cube-ide-core extension lookup by symlinking ~/.local/bin launchers and setting productSTM32CubeMX path.',
      'Configured USB udev rules (/etc/udev/rules.d/) for rootless ST-Link V2 SWD flashing and live GDB server debugging.'
    ]
  },
  {
    id: 'bc51b173',
    phase: 'Session 02–05 • Modular HAL Generator',
    date: 'Nov 2–3, 2026 (Morning)',
    steps: 181,
    title: 'CMake + Ninja + ST-Link GDB Pipeline & Automated 11-Module Scaffolding',
    subsystem: 'Build System & Firmware Scaffolding',
    summary: 'Eliminated a conflicting Makefile extension that broke CMake configuration, wired stlinkgdbtarget debugging in VS Code, and created new_module.sh to generate the 11 firmware modules.',
    highlights: [
      'Identified and removed travnsam.vscode-stm32cube-configurator which forced legacy Makefile checks on CMake presets.',
      'Verified live SWD connection to STM32G473VET6 (Device ID 0x469, 512 KB Flash, 3.31V target voltage).',
      'Built new_module.sh to scaffold Modules/{Barometer,EEPROM,Flight,GPS,IMU,Logger,LoRa,Sensors,Servos,Storage,USB,WS2812} and auto-register include/source paths in CMakeLists.txt.'
    ]
  },
  {
    id: '85b2fb21',
    phase: 'Session 06 • DMA Hardware Debugging',
    date: 'Nov 3, 2026 (Midday)',
    steps: 172,
    title: 'WS2812B NeoPixel TIM4_CH4 + DMA Waveform Debugging & Oscilloscope Diagnosis',
    subsystem: 'TIM4 PWM + DMA1_Channel1 (PB9)',
    summary: 'Debugged the 800 kHz WS2812B pulse engine on PB9 using live SWD register inspection (TIM4/GPIOB) and an OWON VDS1022 oscilloscope, uncovering two STM32G4 HAL DMA quirks and a parasitic back-powering fault.',
    highlights: [
      'Fixed STM32G4 HAL_TIM_PWM_Stop_DMA() bug where clearing TIM_CCER_CC4E left PB9 floating high between frames instead of latching LOW (0V) for >50 µs.',
      'Fixed 23-of-24 bit DMA pipeline shift by adding PREAMBLE_SLOTS = 2 leading zero slots before the 24 GRB bits and RESET_SLOTS = 240 trailing zero slots.',
      'Diagnosed flat 720 mV (0.72V) oscilloscope trace when connecting the LED: unpowered VDD pin caused PB9 (3.3V) to parasitically back-power the LED through its internal DIN ESD diode.'
    ]
  },
  {
    id: 'c6a548b1',
    phase: 'Session 07 • CubeMX-Safe Refactor',
    date: 'Nov 3, 2026 (Afternoon)',
    steps: 149,
    title: 'Refactoring Monolithic 739-Line Flight Code into CubeMX-Safe Modular Architecture',
    subsystem: 'IGNIS_CORE_STM32G473VET6 Firmware',
    summary: 'Migrated the complete flight control stack (100 Hz complementary filter, 3-axis PID, 4-fin X-mixing, MPU-6500, MS5611, and USB CDC CLI) into isolated Modules/ while preserving all CubeMX USER CODE blocks.',
    highlights: [
      'Split monolithic main.c into clean hardware abstraction modules with zero edits outside /* USER CODE BEGIN ... */ markers.',
      'Compiled firmware with 0 warnings and 0 errors: Flash usage 66,844 B (12.75% of 512 KB), RAM usage 10,384 B (7.92% of 128 KB).',
      'Integrated real-time 100 Hz CSV telemetry output and single-key USB commands (A=Arm, D=Disarm, C=Calibrate, S=Diagnostics).'
    ]
  },
  {
    id: 'b31b4658',
    phase: 'Session 08 • Pinout, I2C2 Fix & Pyro Stage',
    date: 'Nov 3, 2026 (Late Afternoon)',
    steps: 80,
    title: 'I2C2 PC4/PA8 Hardware Mux Fix, Contiguous LQFP-100 LoRa Routing & 3-Channel Pyro Design',
    subsystem: 'Hardware Pinout & Pyro Switching',
    summary: 'Resolved why MPU-6500 and MS5611 initially read 0x00, assigned contiguous LQFP-100 pins for SX1278 LoRa and SPI2 microSD, and engineered the 3-channel MOSFET pyro igniter and 5V buzzer circuits.',
    highlights: [
      'Caught the STM32G473VET6 hardware I2C2 pin mux constraint: PC4 (Pin 30) is strictly I2C2_SCL and PA8 (Pin 69) is strictly I2C2_SDA.',
      'Routed SX1278 LoRa across 6 consecutive physical pins on the LQFP-100 package (Pins 90–95: PB3 SCK, PB4 MISO, PB5 MOSI, PB6 RST, PB7 CS, PB8 DIO0 EXTI).',
      'Designed direct 3.3V logic-level drive for Pyro Ch 1 & 2 (IRLZ44N on PC7/PC8) and a fail-safe dual-BC547 NPN non-inverting 3.3V-to-V_BAT gate shifter for Pyro Ch 3 (IRF520 on PC9).'
    ]
  },
  {
    id: '9751e86f',
    phase: 'Session 09 • SPICE Circuit Verification',
    date: 'Nov 4, 2026',
    steps: 129,
    title: 'LTspice & Qucs-S (ngspice) Transient Simulation of BC547 + IRF520 Pyro Igniter Driver',
    subsystem: 'Analog / Power Electronics Simulation',
    summary: 'Added custom SPICE models for BC547 and IRF520 into LTspice and Qucs-S to simulate transient switching and verify full gate saturation under 3.3V MCU logic drive.',
    highlights: [
      'Injected BC547/BC547A NPN and IRF520 VDMOS SPICE subcircuit models into LTspice and configured Qucs-S with ngspice.',
      'Verified that a single NPN inverter pulls the IRF520 gate HIGH when the MCU pin is 0V (unsafe on boot), validating the 2-stage BC547 non-inverting topology that holds Gate at 0V during reset.'
    ]
  },
  {
    id: '915e3e3b',
    phase: 'Session 10 • G4 vs F4 & KiCad DRC Audit',
    date: 'Nov 7–9, 2026',
    steps: 65,
    title: 'STM32G473VET6 Rocket Avionics Trade Study & Automated KiCad ERC/DRC Board Audit',
    subsystem: 'IGNIS_CORE Custom KiCad PCB',
    summary: 'Evaluated STM32G473VET6 mixed-signal & CORDIC/FMAC advantages over STM32F405/F411, diagnosed WeAct LDO failure modes, and ran a full kicad-cli ERC/DRC audit across the 71-footprint IGNIS_CORE PCB.',
    highlights: [
      'Documented how STM32G473VET6 CORDIC + FMAC co-processors offload solid-motor structural vibration notch filtering and EKF trig without external DSP overhead.',
      'Audited ProPrj_Project Ignis_2026-06-03.kicad_pcb (71 footprints, 985 trace segments, 147 vias) with kicad-cli.',
      'Caught 4 critical pre-fabrication items: U2 (AMS1117-5.0) VIN-to-5V track overlap on F.Cu, X2 32.768 kHz Pad 1 (/OSCOUT32) 0.6 mm gap, 40 unpoured GND vias, and USB1 Edge.Cuts overlap.'
    ]
  }
];

export interface FlightSample {
  t: number;
  pitch: number;
  roll: number;
  yaw: number;
  alt: number;
  speed: number;
  pres: number;
}

export const SAMPLE_FLIGHT_DATA: FlightSample[] = [
  { t: 0, pitch: 0.0, roll: 0.0, yaw: 0.0, pres: 1013.25, alt: 0.0, speed: 0.0 },
  { t: 100, pitch: 2.1, roll: 0.8, yaw: 0.5, pres: 1012.8, alt: 10.5, speed: 45.0 },
  { t: 200, pitch: 4.2, roll: 1.5, yaw: 1.0, pres: 1011.9, alt: 25.8, speed: 98.0 },
  { t: 300, pitch: 6.8, roll: 2.1, yaw: 1.5, pres: 1010.5, alt: 45.2, speed: 142.0 },
  { t: 400, pitch: 9.5, roll: 1.8, yaw: 1.2, pres: 1008.6, alt: 70.5, speed: 185.0 },
  { t: 500, pitch: 12.5, roll: 1.2, yaw: 0.8, pres: 1006.2, alt: 100.2, speed: 225.0 },
  { t: 600, pitch: 15.8, roll: 0.5, yaw: 0.2, pres: 1003.3, alt: 135.8, speed: 258.0 },
  { t: 700, pitch: 18.2, roll: -0.2, yaw: -0.5, pres: 1000.0, alt: 175.5, speed: 285.0 },
  { t: 800, pitch: 20.5, roll: -0.8, yaw: -1.0, pres: 996.2, alt: 220.2, speed: 305.0 },
  { t: 900, pitch: 22.1, roll: -0.5, yaw: -0.8, pres: 992.0, alt: 268.5, speed: 318.0 },
  { t: 1000, pitch: 23.0, roll: 0.0, yaw: 0.0, pres: 987.5, alt: 320.8, speed: 325.0 },
  { t: 1100, pitch: 22.5, roll: 0.8, yaw: 0.5, pres: 982.8, alt: 376.5, speed: 322.0 },
  { t: 1200, pitch: 20.8, roll: 1.5, yaw: 1.0, pres: 978.0, alt: 435.2, speed: 310.0 },
  { t: 1300, pitch: 18.0, roll: 1.2, yaw: 0.8, pres: 973.2, alt: 496.5, speed: 290.0 },
  { t: 1400, pitch: 14.5, roll: 0.5, yaw: 0.2, pres: 968.5, alt: 560.2, speed: 265.0 },
  { t: 1500, pitch: 10.2, roll: -0.2, yaw: -0.3, pres: 964.0, alt: 625.8, speed: 238.0 },
  { t: 1600, pitch: 5.8, roll: -0.8, yaw: -1.0, pres: 959.8, alt: 693.5, speed: 208.0 },
  { t: 1700, pitch: 2.1, roll: -0.5, yaw: -0.6, pres: 955.9, alt: 762.2, speed: 178.0 },
  { t: 1800, pitch: -0.5, roll: 0.0, yaw: 0.0, pres: 952.4, alt: 832.5, speed: 148.0 },
  { t: 1900, pitch: -2.1, roll: 0.5, yaw: 0.6, pres: 949.3, alt: 903.8, speed: 118.0 },
  { t: 2000, pitch: -2.8, roll: 0.8, yaw: 1.0, pres: 946.6, alt: 975.5, speed: 95.0 },
  { t: 2100, pitch: -2.2, roll: 0.5, yaw: 0.6, pres: 944.3, alt: 1047.2, speed: 72.0 },
  { t: 2200, pitch: -1.0, roll: 0.0, yaw: 0.0, pres: 942.4, alt: 1118.5, speed: 52.0 },
  { t: 2300, pitch: 0.2, roll: -0.2, yaw: -0.2, pres: 940.9, alt: 1189.2, speed: 35.0 },
  { t: 2400, pitch: 0.8, roll: -0.3, yaw: -0.4, pres: 939.8, alt: 1259.0, speed: 18.0 },
  { t: 2500, pitch: 0.5, roll: 0.0, yaw: 0.0, pres: 939.2, alt: 1328.0, speed: 8.0 },
  { t: 2600, pitch: 0.0, roll: 0.2, yaw: 0.2, pres: 939.8, alt: 1259.0, speed: 12.0 },
  { t: 2700, pitch: -0.5, roll: 0.5, yaw: 0.5, pres: 940.9, alt: 1189.2, speed: 22.0 },
  { t: 2800, pitch: -0.2, roll: 0.3, yaw: 0.3, pres: 942.4, alt: 1118.5, speed: 35.0 },
  { t: 2900, pitch: 0.3, roll: 0.0, yaw: 0.0, pres: 944.3, alt: 1047.2, speed: 48.0 },
  { t: 3000, pitch: 0.8, roll: -0.2, yaw: -0.2, pres: 946.6, alt: 975.5, speed: 62.0 },
  { t: 3100, pitch: 1.2, roll: -0.4, yaw: -0.4, pres: 949.3, alt: 903.8, speed: 78.0 },
  { t: 3200, pitch: 0.8, roll: -0.2, yaw: -0.2, pres: 952.4, alt: 832.5, speed: 92.0 },
  { t: 3300, pitch: 0.2, roll: 0.0, yaw: 0.0, pres: 955.9, alt: 762.2, speed: 105.0 },
  { t: 3400, pitch: -0.4, roll: 0.3, yaw: 0.3, pres: 959.8, alt: 693.5, speed: 115.0 },
  { t: 3500, pitch: -0.8, roll: 0.5, yaw: 0.5, pres: 964.0, alt: 625.8, speed: 122.0 },
  { t: 3600, pitch: -0.3, roll: 0.2, yaw: 0.2, pres: 968.5, alt: 560.2, speed: 115.0 },
  { t: 3700, pitch: 0.2, roll: 0.0, yaw: 0.0, pres: 973.2, alt: 496.5, speed: 105.0 },
  { t: 3800, pitch: 0.6, roll: -0.2, yaw: -0.2, pres: 978.0, alt: 435.2, speed: 92.0 },
  { t: 3900, pitch: 0.3, roll: -0.1, yaw: -0.1, pres: 982.8, alt: 376.5, speed: 78.0 },
  { t: 4000, pitch: 0.0, roll: 0.0, yaw: 0.0, pres: 987.5, alt: 320.8, speed: 62.0 },
  { t: 4100, pitch: -0.2, roll: 0.1, yaw: 0.1, pres: 992.0, alt: 268.5, speed: 48.0 },
  { t: 4200, pitch: -0.1, roll: 0.0, yaw: 0.0, pres: 996.2, alt: 220.2, speed: 35.0 },
  { t: 4300, pitch: 0.1, roll: -0.1, yaw: -0.1, pres: 1000.0, alt: 175.5, speed: 22.0 },
  { t: 4400, pitch: 0.0, roll: 0.0, yaw: 0.0, pres: 1003.3, alt: 135.8, speed: 12.0 },
  { t: 4500, pitch: -0.1, roll: 0.0, yaw: 0.0, pres: 1006.2, alt: 100.2, speed: 5.0 },
  { t: 4600, pitch: 0.0, roll: 0.0, yaw: 0.0, pres: 1008.6, alt: 70.5, speed: 0.0 }
];
