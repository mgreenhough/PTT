# Firmware Specification - [Peripheral Serial Emulator]


---


## **NOTE:** 

- One of the main intentions of the LEARN and PTT functions is purely to arbitrarily (115,200) **oversample unknown** serial commands at an **unknown** baud rate. Record and store the commands. Then retransmit the same commands at the same oversampled baud rate (115,200) with the intention that the radio will receive the commands and interpret them at its own baud rate ie. 57,600 for GME. We are not decoding or frame matching.

- In this document:
  - "SP(X)" means serial.println(X) in the GUI. 
  - "SP.red(X)" means make the font colour red in the GUI.
  - As per [**System Architecture**](Docs\PRD.md)
    - 2 x DG408 IC’s receive the 8 bus lines each in parallel. referred to as "MUX1" and "MUX2" in code.
    - ULN2803 sinks “learnt” TX pin to GND when “Traditional Handpiece” is selected and PTT button activated. Referred to as “ULN” in code.
    - MAX3232 will provide level shifting for and RS232 or 5V UART signals - Referred to as “MAX” in code.
    - TXS0102 will provide level shifting for and 3.3 and 5V UART signals - Referred to as “TXS” in code.
  - UART and serial are used interchangeably.


---


## 1. Context & RAG 
This specification relates to the **Peripheral Serial Emulator (PSE)** defined in the [**README.md**](Docs/README.md)
It is further referenced directly in **Firmware Requirements** of [**PRD.md**](docs/PRD.md).  
If applicable, refer to block designs [PSE3](Docs/Assets/Block Designs/PSE3.png) & [Hardware Interface](Docs/Assets/Block Designs/Hardware Interface.png).


---


// The DG408’s will be referred to and called as MUX1 & MUX2 in the code. They use a binary address control method. A function should be used to simplify this and call the MUX and its PIN with pins named 1 - 8. Eg: mux(1,5); would call MUX1 and select pin 4 (pins being 0-7 at the hardware level but 1-8 in the code). Within the same function “on” and “off” will be called to enable or disable the mux. Eg: mux(2,on); //

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

  - command* stores commands if "remote" type selected.
    - TXstart
    - TXstop

  - DG403(state)* stores the required state of the DG403 switch
    - MAX
    - TXS

  *pin_type will be defined against the following criteria:
    - Pins with Stable voltage >11V will be defined as +12v, SP.green(Power pin (voltage) stored)
    - Pins with Stable -.5~.5V will be defined as GND, SP.green(GND pin stored)
    - Pins with 2.5~6V will be defined as data, SP.green(Data pin stored) 
    - Pins with <-2.5V will be defined as RS232 - SP.green(RS232 pin stored)
    - Any pins not fitting above constraints will be defined as Undefined, SP.red(Undefined pin (voltage) stored)
  *pin_name, profile_type and command and DG403 are defined in the Firmware Function below.


## 3. Firmware Function
On boot:
- As per [**Functional Requirements**](Docs\PRD.md) **The core running the GUI and emulator has explicit priority in the event of hardware requirement conflict (MUX1)** It can however, wait for the other core to finish its main loop. (The other core polling whether the GUI/emulator core requires access and the start of main loop and not continuing if access is required.)
  - **One core** Will enable MUX1 and Set DG403 input to MAX (DG403(MAX) Routes its outputs to the MAX3232), to run the **8 channel bipolar voltage analyser and UART detector**. See section **2** of [**Interface Specification**](Docs\InterfaceSpec.md)
  - **One core** will start to run the **GUI and PTT emulator**. See section **3** of [**Interface Specification**](Docs\InterfaceSpec.md) for the interface and the following for function:
    - The device will read the state of MODE SWITCH.
      - If MODE SWITCH is set to LEARN:
        1. DG403 input already set to MAX (Its outputs routed to the MAX3232 for safety incase over v or neg v encountered)
        2. Await input from the user in the GUI: LEARN, Step 4.
        3. If “Remote Handpiece" option is selected by the user in the GUI, store “remote” to non volatile memory as the profile_type under the current profile and:
          1. Pull pin ADC readings from voltage analyser, a GUI update or efficient means. 
          2. Define each channel and pin_type as per **2. Data Structure** and store.
          3. If any combination of DATA and RS232 pins other than 2 DATA **OR** 2 RS232 pins are stored, SP.red(Data pins unclear, retry pin LEARN or contact support). GUI returns to “LEARN” step 3 after 5 seconds.
          4. If 2 DATA **OR** 2 RS232 pins are stored, the device will determine which of the data pins is TX & RX by determining which pin transmits first when user activates PTT. The commands are separated by less than 200us so it has to be accurate!
            - Hardware conflict will arise with now with this core requiring priority access to both MUX1 and MUX2.
            - If pins are RS232, keep DG403 set to MAX.
            - Else if pins are DATA, set DG403 to TXS.
            - Establish UART RX1 and RX2 at highest reliable baud rate (115,200?).
            - Prompt user to “Press & Hold Transmit Button”.
            - Determine which UART transmitted first and store temporarily as pin_name(TX).
            - Prompt user to “Release Transmit Button”.
            - Confirm which UART transmitted first referencing the temporarily stored pin.
            - If not confirmed SP.red(TX Unclear) and repeat 3 times. If still not successful SP.red(TX unclear, contact support).
            - Else if confirmed SP.green(TX Confirmed) and store the TX pin as pin_name(TX) and the other pin as pin_name(RX) under the current profile.
            - Store DG403 current input state as DG403(state) under the current profile.
           5. Hardware conflict has now ended and voltage analyser core can resume use of MUX1 (eg. set access required flag to 0).
           6. The device now needs to record and store to non volatile memory under the current profile, the “TXstart” and “TXstop” commands for spoofing.
             - Prompt the user to “Press & Hold Transmit Button”.
             - If UART received, record the incoming UART command and stop when >16 unchanged bits are received. SP.green(TXstart Received).
             - Store as TXstart command temporarily.
             - If UART not received, Prompt user to “Release Transmit Button” and repeat step 5 times. If still not received, SP.red(Unable to read TX command) and prompt user to retry or contact support. GUI returns to “LEARN” step 3 after 5 seconds. 
             - Prompt user to “Release Transmit Button”.
             - If UART received, record the incoming UART command and stop when >16 unchanged bits are received. SP.green(TXstop Received).
             - Store as as TXstop command temporarily.
             - If UART not received, go back to start and repeat step 5 times. If still not received, SP.red(Unable to read TX command) and prompt user to retry or contact support. GUI returns to “LEARN” step 3 after 5 seconds.
             - Clip both commands to 1 bit after the last low bit and store to non volatile memory under the current profile.
             - SP.green(TX Commands Recorded Successfuly!)
             - Now refer to GUI, LEARN step 7.

////////////////////  UP TO HERE  //////////////////////

        4. If “Traditional Handpiece” option is selected by the user in the GUI, store “trad” to non volatile memory as the “Profile Type” under the current profile:
           1. MUX1 is enabled.
           2. Each one of MUX1’s input pins are driven sequentially through the PIN READ pin ADC. 
           - Each pin is defined and submitted to non volatile memory as: 
pinInfo {        
Definition; //+12v(X), GND(X), Data(X) or Undefined(X) (X starting at 1)
        Mux; // mux1 or mux2
        pinNumber; // 1~8
ADC; // mapped ADC reading displayed as actual volts


Pins with Stable voltage >11V will be defined as “+12v-(X)” - SP(green): “Power pin (X) (voltage) stored”
Pins with Stable -.5~.5V will be defined as “GND(X)” - SP(green): “GND pin (X) stored”
Pins with 2.5~6V will be defined as “Data(X)” - SP(green): “Data pin (X) stored” 
Pins with <-2.5V will be defined as “RS232-(X)” - SP(green): “RS232 pin (X) stored”
Any pins not fitting above constraints will be defined as “Undefined(X) (voltage)” - SP(red): “Undefined pin (X) - (ADC) stored”
   3. Prompt the user to “Press & Hold Transmit Button”
Each “+12v” pin is driven sequentially through the PIN READ pin ADC and checked for GND.
If a +12v pin reads GND or close to when the transmit button is pressed it is stored as “TXpin(X)” under the current profile.
Prompt user to “Release Transmit Button”
If no change is detected on any pin, repeat 4.3. 5 times. Else prompt user to contact support.
If >1 TXpin is detected SP(red): “TXpin Unclear” and prompt user to contact support.




If MODE SWITCH is set to RUN:


1. Set DG403 input to MAX
2. Use the ADC to sequentially test each pin and confirm its stored ADC reading.
3. If any mismatches > +/-1v, SP(red): (List of any pins that do not match their stored ADC readings by +/- 1v) and Prompt user to select correct profile or Re-Learn.
4. If no mismatches are detected, change DG403 to DG403_state of current profile.
5. Assign interrupt to PTT
6. If current profile type is “trad” assign TXpin to ULN so that when PTT button is pressed, TXpin is sunk to GND through the ULN until it is released. SP(green): “TX Sent”.
7. Else if current profile type is “remote”, transmit “TXstart” on RXpin when PTT activated and transmit “TXstop” on RXpin when PTT released. SP(green): “TX Sent”. (if RS232 mux1 will be used, if DATA use mux2) // check pins in code because we haven't specified this to the AI and also check routing of UART when using 74LVC245//
