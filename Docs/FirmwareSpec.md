# Firmware Specification - [Peripheral Serial Emulator]


---


## **NOTE:** 

### - When learning and retransmitting TXstart and TXstop commands, the device is **intentionally agnostic to the radio’s actual baud rate.** Different radios may use different commands and baud rates. To handle this, the device **oversamples all incoming serial commands at 115,200 baud,** records the raw waveform, and stores it. During retransmission, the commands are sent at the **same oversampled rate (115,200 baud),** relying on the radio to interpret them at its native baud (e.g., 57,600 baud for GME) and sample mid bit. **No baud detection, decoding or frame matching is performed** — the system purely matches waveforms.
- In this document:
  - "SP(X)" means serial.println(X) in the GUI. 
  - "SP.red(X)" means make the font colour red in the GUI.
  - As per [**System Architecture**](Docs\PRD.md)
    - 2 x DG408 IC’s receive the 8 bus lines each in parallel. referred to as "MUX1" and "MUX2" in code.
    - ULN2803 sinks “learnt” TX pin to GND when “Traditional Handpiece” is selected and PTT button activated. Referred to as “ULN” in code.
    - MAX3232 will provide level shifting for and RS232 or 5V UART signals - Referred to as “MAX” in code.
    - TXS0102 will provide level shifting for and 3.3 and 5V UART signals - Referred to as “TXS” in code.
  - UART and serial are used interchangeably.
- This document, whilst explicit in it's requirements is not prescriptive in its method. If there is a more efficient, simpler or clearer path to the same outcome, it should be employed.


---


## 1. Context & RAG 
This specification relates to the **Peripheral Serial Emulator (PSE)** defined in the [**README.md**](Docs/README.md)
It is further referenced directly in **Firmware Requirements** of [**PRD.md**](docs/PRD.md).  
If applicable, refer to block designs [PSE3.1](Docs/Assets/Block Designs/PSE3.1.png) & [Hardware Interface](Docs/Assets/Block Designs/Hardware Interface.png).


---


-The DG408’s will be referred to and called as MUX1 & MUX2 in the code. They use a binary address control method and are driven by 74HC595-1. A function should be used to aid easy reading and modification of the code by calling the MUX and its PIN with pins named 1 - 8. Eg: mux(1,5); would "shiftout()" the appropriate bits to enable MUX1 and select pin S5 (pin12 at the hardware level). Within the same function, “off” will be called to disable the mux. Eg: mux(2,off) *or "0" if easier to implement*.
- RX1/2 are GPIO interrupt pins aimed purely at detecting which pin receives first. Once detected, it is then read by a dedicated MCU UART - U1.

## 2. Data Structure
Stored in non volitile memory will be radio "Profiles" named and saved by the user in the GUI. They will contain all the necessary data for the PTT function of the device to operate when in RUN mode. Thes profiles will include but not be limited to:

  - chan_(1-8) stores the 8 mux pins with the following 3 variables stored under each "channel": 
    - pin_type*:
      - +12v
      - GND
      - data
      - RS232
      - Undefined
    - pin_v, voltage recorded at the time of calibration.
    - pin_name*:
      - PWR
      - GND
      - TX
      - RX 

  - profile_type*  stores the type of handpiece:
    - remote
    - trad

  - _command* stores commands if "remote" type selected.
    - TXstart
    - TXstop

  - DG403-1(state)* stores the required state of the DG403-1 switch
    - MAX
    - TXS
  
  - DG403-2(state)* stores the required state of the DG403-2 switch
    - 3.3v
    - 5v

  - _stats* stores use and productivity statistics
    - TX_calls
    - total_TX

  *pin_type will be defined against the following criteria:
    - Pins with Stable voltage >11V will be defined as +12v, SP.green(Power pin (voltage) stored)
    - Pins with Stable -.5~.5V will be defined as GND, SP.green(GND pin stored)
    - Pins with 2.5~6V will be defined as data, SP.green(Data pin stored)
    - Pins with <-2.5V will be defined as RS232 - SP.green(RS232 pin stored)
    - Any pins not fitting above constraints will be defined as Undefined, SP.red(Undefined pin (voltage) stored)
  *pin_name, profile_type, _command, DG403-1() and _stats are defined in the Firmware Function below.


## 3. Firmware Function
On boot:
  - The device will read the state of MODE SWITCH.

      - If MODE SWITCH is set to LEARN:

        - **One core** Will enable MUX1 and Set DG403-1() input to MAX (DG403-1(MAX) Routes its outputs to the MAX3232), to run the **8 channel bipolar voltage analyser and UART detector**. See section **2** of [**Interface Specification**](Docs\InterfaceSpec.md)
        - **One core** will start to run the **GUI and PTT emulator**. See section **3** of [**Interface Specification**](Docs\InterfaceSpec.md) for the interface and the following for function:
        - As per [**Functional Requirements**](Docs\PRD.md) **The core running the GUI and emulator has explicit priority in the event of hardware requirement conflict (MUX1)** It can however, wait for the other core to finish its main loop. (The other core polling whether the GUI/emulator core requires access and the start of main loop and not continuing if access is required.)

        - ***Make sure circuit physically connects MaX3232 to default (LOW) output of DG403-1.**

        1. Await input from the user in the GUI: LEARN, Step 4.

-------------

        2. If “Remote Handpiece" option is selected by the user in the GUI, store “remote” to non volatile memory as the profile_type under the current profile and:
          1. Pull pin ADC readings from voltage analyser, a GUI update or efficient means. 
          2. Define each channel and pin_type as per **2. Data Structure** and store.
          3. If any combination of DATA and RS232 pins other than 2 DATA **OR** 2 RS232 pins are stored, SP.red(Data pins unclear, retry pin LEARN or contact support). GUI returns to “LEARN” step 3 after 5 seconds.
          4. If 2 DATA **OR** 2 RS232 pins are stored, the device will determine which of the data pins is TX & RX by determining which pin transmits first when user activates PTT. The commands are separated by less than 200us so it has to be accurate and will likely be handled by two GPIO interrupts?
            - Hardware conflict will arise now with this core requiring priority access to both MUX1 and MUX2.
            - If pins are RS232, keep DG403-1() set to MAX.
            - Else if pins are DATA, set DG403-1() to TXS.
              - Store DG403-1 current input state as DG403-1(state).
              under the current profile.
              - If BOTH pins are <3.4v, store DG403-2(state) as 3.3v
              - If BOTH pins are >3.4v, store DG403-2(state) as 5v
              - Else SP.red(Data pins unclear, retry pin LEARN or contact support). GUI returns to “LEARN” step 3 after 5 seconds.
              - Set DG403-2 to DG403-2(state)            
            - Assign interrupts to RX1 and RX2 of MCU.
            - Prompt user to “Press & Hold Transmit Button”.
            - Determine which UART transmitted first (RX1/RX2) and store temporarily with the input channel, as pin_name(TX).
            - Prompt user to “Release Transmit Button”.
            - Confirm which UART transmitted first referencing the temporarily stored pin.
            - If not confirmed SP.red(TX Unclear) and repeat 3 times. If still not successful SP.red(TX unclear, contact support).
            - Else if confirmed SP.green(TX Confirmed) and store the TX pin as pin_name(TX) and the other pin as pin_name(RX) under the current profile.
          5. The device now needs to record and store to non volatile memory under the current profile, the “TXstart” and “TXstop” commands for spoofing.
            - Select pin_name(TX) on MUX1
            - Prompt the user to “Press & Hold Transmit Button”.
            - Establish UART1 at 115 200 baud.
            - If UART received on U1RX, record the incoming UART _command and stop when >16 unchanged bits are received. SP.green(TXstart Received).
            - Clip the command to 4 high bits after the last low bit to ensure complete stop bits at oversampled baud.
            - Store as TXstart _command in non volatile memory under the current profile. SP.green(TXstart command recorded!)
            - If UART not received, Prompt user to “Release Transmit Button” and repeat step 5 times. If still not received, SP.red(Unable to read TX _command) and prompt user to retry or contact support. GUI returns to “LEARN” step 3 after 5 seconds. 
            - Prompt user to “Release Transmit Button”.
            - If UART received, record the incoming UART _command and stop when >16 unchanged bits are received. SP.green(TXstop Received).
            - Clip the command to 4 high bits after the last low bit to ensure complete stop bits at oversampled baud.
            - Store as TXstop _command in non volatile memory under the current profile. SP.green(TXstop command recorded!)
            - If UART not received, go back to start and repeat step 5 times. If still not received, SP.red(Unable to read TX _command) and prompt user to retry or contact support. GUI returns to “LEARN” step 3 after 5 seconds.
            - Hardware conflict has now ended and voltage analyser core can resume use of MUX1 (eg. set access required flag to 0).

-------------

        3. If “Traditional Handpiece” option is selected by the user in the GUI, store “trad” to non volatile memory as the profile_type under the current profile:
          1. Pull pin ADC readings from voltage analyser, a GUI update or efficient means. 
          2. Define each channel and pin_type as per **2. Data Structure** and store.
          3. Prompt the user to “Press & Hold Transmit Button”
          4. Pull pin ADC readings again from voltage analyser, a GUI update or efficient means.
          5. Check if any of the pin_type - "12v" pins have changed to GND or close to it. That is TXpin.
            - If yes:
              - If >1 TXpin found SP.red(Multiple TX pins found, result unclear. Retry or contact support).
              - If 1 TXpin found, store to current profile as TXpin. SP.green(TX pin stored) and refer to GUI, LEARN step 7.
            - If no: SP.red(NO TX pins found. Retry or contact support).

-------------

      - If MODE SWITCH is set to RUN:

        - *Voltage analyser/UART detector is not enabled.
        - ***Make sure circuit physically connects MaX3232 to default output (LOW) of DG403-1.**

        1. Pull pin ADC readings from voltage analyser, a GUI update or efficient means.
        2. Check each pins ADC reading against its stored value and confirm it is the same (+/-1v)
        3. If any mismatches > +/-1v, SP.red((List of any pins that do not match)) and Prompt user to select correct profile, Re-Learn or contact support.
        4. If no mismatches are detected, change DG403-1() to DG403-1(state).
        of current profile.
        5. Assign interrupt to PTT.
        6. SP.green(Profile ready).

        7. If current profile_type is “trad”: 
          - Load last used profile.
          - Assign TXpin to 74HC595-2 so that when PTT button is pressed, TXpin is sunk to GND through the 74HC595 and ULN2803 until PTT button is released. SP.green(TXpin assigned)
          - When PTT button is pressed by the user, ground TXpin and SP.green(TXstart) *(start millis())
          - When PTT button is released by the user, un-ground TXpin and SP.green(TXstop). *(stop millis())

          - *Update "statistics" visible in the GUI - [Section 3. **RUN** of Interface Spec](Docs\InterfaceSpec.md):
            - Total times PTT function used. (If PTT is pressed, update total).
            - Total tx time (time between when PTT is pressed to when it is released. Add (stop_millis() - start_millis()) to total).

        8. Else if current profile_type is “remote”:
          - Load last used profile.
          - Set DG403-1 to the DG403-1(state).
          stored in current profile and SP.green(DG403-1 Set).
          - Set MUX1 to RXpin channel.
          - When PTT button is pressed by the user, transmit TXstart _command on **RXpin** and SP.green(TXstart). *(start millis())
          - When PTT button is released by the user, transmit “TXstop” on **RXpin** and SP.green(TXstop). *(stop millis())

          - *Update "statistics" visible in the GUI - [Section 3. **RUN** of Interface Spec](Docs\InterfaceSpec.md):
            - Total times PTT function used. (If PTT is pressed, update total)((TXstart & TXstop) is 1 PTT call).
            - Total tx time (time between when PTT is pressed to when it is released. Add (stop_millis() - start_millis()) to total).