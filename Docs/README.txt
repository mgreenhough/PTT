# Peripheral Serial Emulator

A hardware-based peripheral serial emulator designed to simulate RS-232/TTL/UART-compatible devices for development, testing, and automation. 

## Features
- Emulates UART communication up to 115200 baud
- ±15 V tolerant inputs with voltage scaling and protection
- ADS1115 (or ESP32 ADC) for live voltage monitoring
- Configurable through serial terminal or app interface

## Project Structure
- `hardware/` — KiCad schematics, PCB layout, and BOM
- `firmware/` — Microcontroller code and configs
- `docs/` — Documentation (PRD, specs, design notes)
- `tools/` — Scripts or simulators
- `test/` — Logs, validation results, QA

## Dependencies
- KiCad 7+
- PlatformIO / Arduino IDE
- Python 3.x (for tooling)

## Getting Started
1. Clone the repo
2. Open `/hardware/` in KiCad
3. Flash firmware from `/firmware/`
4. Read `/docs/PRD.md` and `/docs/SystemSpec.md` for details

