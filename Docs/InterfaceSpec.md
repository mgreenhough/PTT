

# Interface Specification – [GUI] & [8 Channel Bipolar Voltage Analyser and UART Detector]


---


## 1. Context & RAG 
These interfaces relate to the **Peripheral Serial Emulator (PSE)** defined in the [**README.md**](Docs/README.md)
It is further referenced directly in **Functional Requirements** of [**PRD.md**](docs/PRD.md).  
If applicable, refer to block designs [PSE3](Docs/Assets/Block Designs/PSE3.png) & [Hardware Interface](Docs/Assets/Block Designs/Hardware Interface.png).


---


## **2. ADC Sampling Strategy for 8 channel bipolar voltage analyser and UART detector**
  As referenced in [**Functional Requirements**](docs/PRD.md)    

    **8 channel bipolar voltage analysis & UART/serial activity detection** - In this case UART and serial are used interchangeably.

    **Objective:**
    - Analyse voltage and Detect UART activity across 8 bipolar voltage channels using a DG408 analog multiplexer level shifted by TL072 into ESP32 ADC.
    - Maintain GUI updates at a user-friendly rate (10-30 Hz per channel).
    - Reliably detect UART activity within a 12 byte window without needing to decode entire UART packets or detect start bits specifically.
    - Provide a live graphical and numeric voltage readout for each channel.
    - Provide per-channel visual indicators for UART activity via retriggerable monostable code (2 sec indicator when UART detected).
    - Provide per-channel UART activity trigger count. (How many times has the aforementioned indicator been triggered)
    - Support offset and gain calibration on a per-channel basis.
    - Allow user to trigger calibration routine from GUI.
    - Store calibration data in non-volatile memory.
  
    **Key Parameters:**
    - Highest UART baud rate target: 115,200 baud.
    - Each UART bit duration: ~8.68 µs.
    - 12 byte window is ~833us
    - Target: At least 4 ADC reads per channel, per 12 byte window.
    - Total channels: 8.

    **Sampling Timing Budget:**
    - Total reads per channel: 4.
    - MUX and Buffer settling time: ~20 us.
    - Settling time per sample: ~20 µs.
    - Total dwell time per channel: ~100 µs.
    - Total full scan cycle (8 channels): ~800~ µs.

    **Transmission Window Considerations:**
    - A 12-byte UART packet takes ~833µs to transmit at 115,200 baud.
    - The full 8-channel scan cycle is faster (800µs), ensuring at least 1 visit to each channel during any 12-byte transmission.
    - This minimizes the risk of missing any UART activity bursts on any channel.
    - There is a non zero risk of detecting 4 random 1's and in turn not triggering detection logic.

    **Detection Logic:**
    - Focus on detecting any voltage transitions during sampled bits (rising or falling edges).
    - No need to detect UART framing or start bits.
    - Use difference thresholds between consecutive ADC samples to flag activity — only count a change if it exceeds a predefined noise threshold.
    - Implement hysteresis or debounce logic in firmware to avoid rapid toggling due to noise around the threshold level.
    - Stepper pulses are typically longer and easier to detect with the same sampling strategy.

    **Firmware Implications:**
    - Implement rapid ADC sampling bursts per channel while DG408 is parked.
    - Buffer ADC samples temporarily to analyze for signal transitions.
    - Update GUI at a decimated rate (e.g., 30 Hz per channel) independent of ADC sampling frequency.

    **Summary:**
    - This approach balances reliable UART and stepper pulse activity detection with smooth GUI updates.
    - Sampling timing and channel switching are optimized to ensure no UART activity bursts >/=12 bytes are missed.
    - The design avoids complexity of full UART decoding and leverages efficient sampling for event detection.
  

---


## **3. GUI and PTT emulator**
  As referenced in [**Functional Requirements**](docs/PRD.md)

    ### **GUI**
      - The GUI is a simple, attractive and intuitive website that is hosted locally on the MCU so as to be available offline. The GUI will be optimised for use on mobile devices.
      - When prompting the user to manipulate the transmit button, a large and clear notification needs to be shown on screen.
      - Working from the top, down: 
      - Always: 
        - The state of the MODE SWITCH will be displayed boldly and the page will change depending on whether LEARN or RUN is selected.
        - There will be a "DEBUG" tab in the top right corner that will lead to a separate page and dysplay the voltage analyser and UART detector.
        
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
    
      

      #### DEBUG
        - A separate GUI page that displays live, the voltage and any serial activity on each of the 8 channels.
        - Shows all 8 channels from top to bottom and graphically scrolls voltages horizontally across the page.
        - A block on each channels left hand side contains the following:
          - Channel (1-8), size small
          - Its numerical voltage, size large
          - Serial activity indicator, size medium, colour RED
          - Serial activity counter, size small
      - GUI updates should be maintained at a user-friendly rate (10-30 Hz per channel) or be averaged so as to appear smooth.
      - A button to access calibration/recalibration should be obviously located and labeled.
      
    ### **PTT**
      - When the MODE SWITCH is moved to RUN, the device will simply wait for the user to press the PTT button.
      - Upon receiving the PTT action, the device will use the pins saved under the current profile to either:
      - If "traditional Handpiece” was selected in the LEARN procedure, the PTT pin saved under the current profile will be grounded through the ULN2803 until the user releases the PTT button.
      - If “Remote Handpiece" was selected in the LEARN procedure, the “TXstart” serial command saved under the current profile will be transmitted on the RX pin. When the user releases the PTT button the “TXstop” serial command saved under the current profile will be transmitted on the RX pin.