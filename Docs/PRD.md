# Product Requirements Document (PRD)

## Project Name
Peripheral Serial Emulator (PSE)

## Purpose
To provide a configurable hardware emulator that spoofs recorded serial commands and emulates PTT functions.

## Target Users
- Earthmovers
- Heavy equipment operators
- Road crews
- Farm machinery operators
- Anyone who uses a UFH/CB radio and had their hands occupied with machine controls

## System Architecture

![Block Diagram](Supporting Docs/PSE3.pdf)

The block diagram above illustrates the main components of the Peripheral Serial Emulator (PSE).  

  - The ESP32 C3 will be referred to as the MCU in this document and is the device's primary control mechanism.
  - 8 Chan IN. Each of the 8 lines is unknown and may have on it one of the following:
    - +11~13v
    - GND
    - TTL UART or other serial coms +5v or +3v
    - RS232 +/-12v
    - 2 x DG408 IC’s will receive the 8 lines each in parallel. They will route serial data in/out and route +/-V & GND for reading. Dual supply +/- 15vDC
    - 1 DG403 will route the outputs of the DG408’s through the appropriate level shifter.
    - TL072 is an opamp with voltage divider and clamps to read each of the 8 lines in safely through the PIN READ pin ADC.
    - ULN2803 sinks “learnt” TX pin to GND when “Traditional Handpeice” is selected and PTT button activated. Referred to as “ULN” in code.
    - AM1DS-0515DH Dual supply DC DC converter to supply DG408’s, DG403 and TL072 with +/-15vDC
    - MAX3232 will provide level shifting for and RS232 or 5V UART signals - Referred to as “MAX” in code.
    - TXS0102 will provide level shifting for and 3.3 and 5V UART signals - Referred to as “TXS” in code.
    - PTT is a momentary switch that is pulled high through a resistor and activates TX when grounded.
    - MODE SWITCH is a SPST switch that selects between LEARN and RUN.
    - WIFI Web Host Interface - The MCU will host a website to use as a GUI and will be its primary interface. It will be accessed via the MCU’s onboard WIFI.



## Functional Requirements
- Emulate basic PTT functions by recoding over sampled serial commands and playing them back at the same baud rate.
- Handle ±15 V inputs safely via op-amp conditioning
- Visualize live input voltage and UART activity on a mobile and offline interface
- Switch between user saved profiles of a number of different emulated devices
- Use ESP32 ADC in DMA mode for high speed analog sampling
- Use both cores of the esp32: 
  
  - One core to continually run inbuilt 8 channel bipolar voltage analyser and UART detector with the below strategy:
    
    ADC Sampling Strategy for UART Activity Detection

    Objective:
    - Detect UART activity across 8 bipolar voltage channels using a DG408 analog multiplexer level shifted by TL072 into ESP32 ADC.
    - Maintain GUI updates at a user-friendly rate (10-30 Hz per channel).
    - Reliably detect UART activity within a 12 byte window without needing to decode entire UART packets or detect start bits specifically.
    - Provide a live graphical and numeric voltage readout for each channel.
    - Provide per-channel visual indicators for UART activity via retriggerable monostable code (2 sec indicator when UART detected).
    - Provide per-channel UART activity trigger count. (How many times has the aforementioned indicator been triggered)
    - Support offset and gain calibration on a per-channel basis.
    - Allow user to trigger calibration routine from GUI.
    - Store calibration data in non-volatile memory.
  
    Key Parameters:
    - Highest UART baud rate target: 115,200 baud.
    - Each UART bit duration: ~8.68 µs.
    - 12 byte window is ~833us
    - Target: At least 4 ADC reads per channel, per 12 byte window.
    - Total channels: 8.

    Sampling Timing Budget:
    - Total reads per channel: 4.
    - MUX and Buffer settling time: ~20 us.
    - Settling time per sample: ~20 µs.
    - Total dwell time per channel: ~100 µs.
    - Total full scan cycle (8 channels): ~800~ µs.

    Transmission Window Considerations:
    - A 12-byte UART packet takes ~833µs to transmit at 115,200 baud.
    - The full 8-channel scan cycle is faster (800µs), ensuring at least 1 visit to each channel during any 12-byte transmission.
    - This minimizes the risk of missing any UART activity bursts on any channel.
    - There is a non zero risk of detecting 4 random 1's and in turn not triggering detection logic.

    Detection Logic:
    - Focus on detecting any voltage transitions during sampled bits (rising or falling edges).
    - No need to detect UART framing or start bits.
    - Use difference thresholds between consecutive ADC samples to flag activity — only count a change if it exceeds a predefined noise threshold.
    - Implement hysteresis or debounce logic in firmware to avoid rapid toggling due to noise around the threshold level.
    - Stepper pulses are typically longer and easier to detect with the same sampling strategy.

    Firmware Implications:
    - Implement rapid ADC sampling bursts per channel while DG408 is parked.
    - Buffer ADC samples temporarily to analyze for signal transitions.
    - Update GUI at a decimated rate (e.g., 30 Hz per channel) independent of ADC sampling frequency.

    Summary:
    - This approach balances reliable UART and stepper pulse activity detection with smooth GUI updates.
    - Sampling timing and channel switching are optimized to ensure no UART activity bursts >/=12 bytes are missed.
    - The design avoids complexity of full UART decoding and leverages efficient sampling for event detection.
  
  - One core to run the emulator, PTT and GUI
    
    ### GUI
      - The GUI is a simple, attractive and intuitive website that is hosted locally on the MCU so as to be available offline. The GUI will be optimised for use on mobile devices.
      - When prompting the user to manipulate the transmit button, a large and clear notification needs to be shown on screen.
      - Working from the top, down: 
      - Always: 
        - The state of the MODE SWITCH will be displayed boldly and the page will change depending on whether LEARN or RUN is selected.
        - There will be a "Debug" tab in the top right corner that will lead to a separate page and dysplay the voltage analyser and UART detector.
        
      #### LEARN:
      1. A drop down box titled “Profile Name” will display the current profile (blank if none).
      2. It will have an add button “+” next to it to add a new profile. Duplicates will not be allowed. User will be notified that name already exists.
      3. If a new profile is selected or learn procedure failed, the page will show a large “LEARN” button. If the profile has current settings stored, the large button will say “Re-Learn”.     There must also be a button to "Delete Profile"
      4. Upon the user clicking Learn or Re-Learn, the user will be prompted with 2 options: “Remote Handpiece" Or “Traditional Handpiece”. The code will interpret this and act accordingly. It does not change the learn function for the user.
      5. A box will open that displays the serial register. All future references in these documents to SP will display here serial.println().
      6. The user will be guided through the LEARN function of the device.
      7. If successful, the user will then be notified that the “Learn Procedure” has been successful, be prompted to switch the device to RUN and notified that WIFI connection to the device is no longer required unless changing/adding profiles.
      8. The page will return to 3. if mode switch is still in "LEARN" position.

      #### RUN
      1. A drop down box titled “Current Profile” will display the current profile.
      2. If the profile has had a successful calibration and its settings are stored on device, a large green “Status” box will say “RUNNING”. All pins stored under the current profile will be displayed in a list. A Serial Register will also be opened and display any serial.println() (SP) from the code.
      3. If the profile has NOT had a successful calibration and its settings are NOT stored on device, a large RED “Status” box will say “LEARNING REQUIRED” with sub text prompting user to change MODE SWITCH to LEARN and complete Learn Procedure.
    
    ### PTT
      - When the MODE SWITCH is moved to RUN, the device will simply wait for the user to press the PTT button.
      - Upon receiving the PTT action, the device will use the pins saved under the current profile to either:
      - If "traditional Handpiece” was selected in the LEARN procedure, the PTT pin saved under the current profile will be grounded through the ULN2803 until the user releases the PTT button.
      - If “Remote Handpiece" was selected in the LEARN procedure, the “TXstart” serial command saved under the current profile will be transmitted on the RX pin. When the user releases the PTT button the “TXstop” serial command saved under the current profile will be transmitted on the RX pin.


## Non-Functional Requirements
- Real-time response under 10 ms for low latency PTT
- Mobile-friendly display output
- IP65 Enclosure
- Simple and fast to install

## Constraints
- MCU: ESP32 or ATmega328P
- Power: ±15 V available, with onboard regulation
- Size: Small enclosure-friendly PCB

## Project Structure
- `hardware/` — KiCad schematics, PCB layout, and BOM
- `firmware/` — Microcontroller code and configs
- `docs/` — Documentation (PRD, specs, design notes)
- `tools/` — Scripts or simulators
- `test/` — Logs, validation results, QA

## Dependencies
- KiCad 7+
- PlatformIO / Arduino IDE
- Python 3.x (for tooling)?
