# Product Requirements Document (PRD)

## Project Name
Peripheral Serial Emulator, referred to as "PSE" or "the device".

## Purpose
To provide a configurable hardware emulator that spoofs recorded serial commands and emulates PTT functions.

## Target Users
- Earthmovers
- Heavy equipment operators
- Road crews
- Farm machinery operators
- Anyone who uses a UFH/CB radio and had their hands occupied with machine controls


---


## System Architecture

![Block Diagram](Docs/Assets/Block Designs/PSE3.png)

The block diagram above illustrates the main components of the Peripheral Serial Emulator (PSE).  

  - The ESP32 will be referred to as the MCU in this document and is the device's primary control mechanism.
  - In this instance the ESP32-PICO-KIT-1 is used. See more at [User Guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-pico-kit-1/user_guide.html#) or [User Guide](Docs/Assets/Datasheets/esp32 Dev Kit User Guide.pdf) 
  - 8 Chan IN. Each of the 8 lines is unknown and may have on it one of the following:
    - +11~13v
    - GND
    - TTL UART or other serial coms +5v or +3v
    - RS232 +/-12v
    - 2 x DG408 IC’s receive the 8 lines each in parallel. They will route serial data in/out and route +/-V & GND for reading. Dual supply +/- 15vDC
    - 1 DG403 will route the outputs of the DG408’s through the appropriate level shifter.
    - TL072 is an opamp with voltage divider and clamps to read each of the 8 lines in safely through the PIN READ pin ADC.
    - ULN2803 sinks “learnt” TX pin to GND when “Traditional Handpiece” is selected and PTT button activated. Referred to as “ULN” in code.
    - AM1DS-0515DH Dual supply DC DC converter to supply DG408’s, DG403 and TL072 with +/-15vDC
    - MAX3232 will provide level shifting for and RS232 or 5V UART signals - Referred to as “MAX” in code.
    - TXS0102 will provide level shifting for and 3.3 and 5V UART signals - Referred to as “TXS” in code.
    - PTT is a momentary switch that is pulled high through a resistor and activates TX when grounded.
    - MODE SWITCH is a SPST switch that selects between LEARN and RUN.
    - WIFI Web Host Interface - The MCU will host a website to use as a GUI and will be its primary interface. It will be accessed via the MCU’s onboard WIFI.

## Physical Interface

![Hardware Interface](Docs/Assets/Block Designs/Hardware Interface.png)

The diagram above illustrates how the device is connected to the existing radio.

  - The handpiece is unplugged from the radio and plugged into the device using its existing RJ45 connector.
  - The data cable coming from the device plugs into the radio with its RJ45 connector.
  - The device has a passthrough on all 8 channels so that the radio can still operate completely independently of the device. 
  - The channels become a bus that can be sniffed and have commands injected into.
  - The remote PTT button is connected by a long 2 wire protected cable.
  - 5v is provided to the device by a cigarette socket adapter.
  - Future iterations will provide the option to order a protected 12v - 5v USB adapter for a hard wired instals.
  - Any 3.3v - 5v rail requirements can be fulfilled at this stage by the ESP32-PICO-KIT-1. A properly protected regulator will be used in future iterations.


---


## Functional Requirements
- Emulate basic PTT functions by recoding over sampled serial commands and playing them back at the same baud rate.
- Handle ±15 V inputs safely via op-amp conditioning
- Visualize live input voltage and UART activity on a mobile and offline GUI
- Switch between user saved profiles of a number of different emulated devices
- Use ESP32 ADC in DMA mode for high speed analog sampling
- Use both cores of the esp32 with explicit priority hierarchy: 
  - One core to continually run inbuilt **8 channel bipolar voltage analyser and UART detector**. See section **2** of [**Interface Specification**](Docs\InterfaceSpec.md)
  - One core to run the **GUI and PTT emulator**. See section **3** of [**Interface Specification**](Docs\InterfaceSpec.md)
  - **The core running the GUI and emulator has explicit priority in the event of hardware requirement conflict (MUX1)** It can however, wait for the other core to finish its main loop. (The other core polling whether the GUI/emulator core requires access and the start of main loop and not continuing if access is required.)

## Firmware Requirements
- Set explicitly, tasks to their respective cores
- Support the following functional areas:
  - Voltage analysis and UART detection, see section **2** of [**Interface Specification**](Docs\InterfaceSpec.md).
  - GUI access via local WIFI Hotspot.
  - GUI, see section **3** of [**Interface Specification**](Docs\InterfaceSpec.md).
  - Learn, record, store and emulate TX Start/Stop functions, See functionality requirements in [**Firmware Specification**](Docs\FirmwareSpec.md).
  - Store radio "Profiles" to non volatile memory on the device.
- Operate robustly and efficiently under all expected conditions.
- Remain maintainable, modular, and easily understandable for future development.
- Respect hardware priority hierarchy

## Non-Functional Requirements
- Real-time response under 10 ms for low latency PTT
- Mobile-friendly display output GUI
- IP65 Enclosure
- Simple and fast to install and use


---


## Constraints
- MCU: ESP32
- Power: 5v USB & ±15 V available via AM1DS-0515DH. 
- Size: Small enclosure-friendly PCB, likely favouring long and thin so to be easily zip tied to something or hidden.

## Project Structure
- `hardware/` — KiCad schematics, PCB layout, and BOM
- `firmware/` — Microcontroller code and configs
- `docs/` — Documentation (PRD, specs, design notes)
- `tools/` — Scripts or simulators
- `test/` — Logs, validation results, QA

## Dependencies
- KiCad 7+
- VS Code - PlatformIO
- Python 3.x (for tooling)?
