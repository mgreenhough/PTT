/*
 * ShiftRegisterMux.h - A library to control multiple MUXes via shift registers
 * 
 * This library uses 74HC595 shift registers to control multiple MUXes.
 * Each MUX requires 3 control pins (for an 8-channel MUX like 74HC4051).
 * The shift registers allow expanding the number of MUXes without using many MCU pins.
 */

#ifndef SHIFTREGISTERMUX_H
#define SHIFTREGISTERMUX_H

#include <ShiftRegister74HC595.h>  // Include the Shift Register Library

class ShiftRegisterMux {
public:
    /**
     * Constructor: Initializes the shift registers with optional MUX names
     * 
     * @param dataPin    - Serial data input (DS) of the shift register
     * @param clockPin   - Shift clock (SHCP) of the shift register
     * @param latchPin   - Storage register clock (STCP) of the shift register
     * @param numShiftRegisters - Number of shift registers in the chain
     * @param muxNames   - Array of strings for MUX names (optional)
     */
    ShiftRegisterMux(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numShiftRegisters, const char* muxNames[] = nullptr)
        : sr(numShiftRegisters, dataPin, clockPin, latchPin), muxNames(muxNames) {
        sr.setAllLow();  // Ensure all outputs start low
    }

    /**
     * Selects a specific channel on a given MUX by name
     * 
     * @param muxName - The name of the MUX
     * @param channel - The desired channel (1-8 for 8-channel MUXes)
     */
    void selectChannel(const char* muxName, uint8_t channel) {
        if (!muxNames) return; // Prevent issues if no names are provided
        if (channel < 1 || channel > 8) return; // Ensure channel is within valid range

        // Find the MUX index based on its name
        for (uint8_t i = 0; i < numMuxes; i++) {
            if (strcmp(muxNames[i], muxName) == 0) {
                selectChannelByIndex(i, channel);
                return;
            }
        }
    }

private:
    ShiftRegister74HC595<3> sr;
    const char** muxNames;
    uint8_t numMuxes = 4; // Set the number of MUXes

    /**
     * Selects a specific channel on a given MUX by index
     * 
     * @param muxIndex - The index of the MUX (0-based)
     * @param channel  - The desired channel (1-8 for 8-channel MUXes)
     */
    void selectChannelByIndex(uint8_t muxIndex, uint8_t channel) {
        channel -= 1;  // Convert 1-based to 0-based numbering
        uint8_t basePin = muxIndex * 3;  // Each MUX uses 3 control pins

        // Set the shift register bits corresponding to the MUX control lines
        for (uint8_t i = 0; i < 3; i++) {
            sr.set(basePin + i, (channel >> i) & 1);  // Extract bit i from channel and set it in shift register
        }

        sr.updateRegisters();  // Push the new values to the shift register output
    }
};

#endif


*Example
    *const char* muxNames[] = {"SENSOR", "OUTPUT", "AUDIO", "LIGHTS"};              String of MUX names
    *ShiftRegisterMux myMux(12, 11, 10, 3, muxNames);                               Data, Clock, Latch, # of shift registers, muxNames string)

    *void setup() {
        *Serial.begin(115200);
    *}

    *void loop() {
        *myMux.selectChannel("SENSOR", 3); // Select channel 3 on SENSOR MUX
        *delay(500);
        *myMux.selectChannel("OUTPUT", 7); // Select channel 7 on OUTPUT MUX
        *delay(500);
    *}