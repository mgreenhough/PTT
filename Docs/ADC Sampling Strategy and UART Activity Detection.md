ADC Sampling Strategy and UART Activity Detection

Objective:
- Detect UART and stepper motor signal activity across 8 bipolar voltage channels using a DG408 analog multiplexer and ESP32 ADC.
- Maintain GUI updates at a user-friendly rate (10-30 Hz per channel).
- Reliably detect signal activity within a 12 byte window without needing to decode entire UART packets or detect start bits specifically.

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
- Update GUI at a decimated rate (e.g., 10 Hz per channel) independent of ADC sampling frequency.

Summary:
- This approach balances reliable UART and stepper pulse activity detection with smooth GUI updates.
- Sampling timing and channel switching are optimized to ensure no UART activity bursts >/=12 bytes are missed.
- The design avoids complexity of full UART decoding and leverages efficient sampling for event detection.

