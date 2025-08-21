# Datasheets Index

**Purpose:**  
This document indexes all hardware datasheets relevant to the PTT project firmware development. It includes links to local PDFs (if included) or official online datasheets, with notes on important sections or revisions.

---

| Component / Part Number | Datasheet Location | Version / Date | Notes / Key Sections |
|------------------------|--------------------|----------------|---------------------|
| MCU – [STM32F103C8](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf) | Official Online Link | Rev 1, 2018-09-10 | See section 5: UART peripheral; Section 7: Power modes |
| UART Transceiver – MAX3232 | `/docs/datasheets/MAX3232.pdf` | Rev 4, 2012-03-15 | Pinout details p4; Timing diagrams p7-9 |
| OLED Display Module | `/docs/datasheets/OLED-SSD1306.pdf` | N/A (Vendor spec) | Command set reference, p10-15 |
| Voltage Regulator – LM7805 | [Texas Instruments LM7805](https://www.ti.com/lit/ds/symlink/lm7805.pdf) | Rev A, 2016-06 | Thermal characteristics, p7-9 |
| Stepper Motor Driver – DRV8825 | `/docs/datasheets/DRV8825.pdf` | 2017-04 | Control registers, p12-18 |


| MCU - In this instance the ESP32-PICO-KIT-1 is used | [User Guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-pico-kit-1/user_guide.html#) or [User Guide](Docs/Assets/Datasheets/esp32 Dev Kit User Guide.pdf) 
| DG408 MUX | [DG408-09](Docs\Assets\Datasheets\DG408-09.pdf)
| DG403 2XSPDT | [DG403](Docs\Assets\Datasheets\DG403.pdf) || **403 is 2 x SPDT. S1 & S2 are OFF when IN1 & IN2 are LOW (default)** see page 2. |
| TL072 Opamp | [TL072](Docs\Assets\Datasheets\TL072.pdf)
| ULN2803 Transistor array | [ULN2803c](Docs\Assets\Datasheets\ULN2803c.pdf)
| AM1DS-0515DH Dual supply | [AM1DS-0515dh](Docs\Assets\Datasheets\AM1DS-0515dh.pdf)
| MAX3232 RS232 Level shifter | [MAX3232](Docs\Assets\Datasheets\MAX3232.pdf)
| TXS0102 5-3.3v Level shifter | [TXS0102](Docs\Assets\Datasheets\TXS0102.pdf)

---

## How to use this catalog  

- If a datasheet PDF is included locally, open it from the `/Docs/Assets/Datasheets/` folder for quick reference.  
- If a datasheet is online only, the link points to the official source — verify the version before referencing.  
- Use the **Notes / Key Sections** column to highlight firmware-relevant info (registers, timing, commands).  
- Add new components or updated datasheets here as hardware changes.  
- Keep datasheets updated and remove obsolete versions to avoid confusion.

---

## Adding new datasheets  

1. Save the datasheet PDF to `/Docs/Assets/Datasheets/` if redistribution is allowed.  
2. Add an entry to this table with a direct relative link and date/version info.  
3. Include any notes that will help future firmware work.
