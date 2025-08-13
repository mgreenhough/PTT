# Datasheets Catalog

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

---

## How to use this catalog  

- If a datasheet PDF is included locally, open it from the `/docs/datasheets/` folder for quick reference.  
- If a datasheet is online only, the link points to the official source — verify the version before referencing.  
- Use the **Notes / Key Sections** column to highlight firmware-relevant info (registers, timing, commands).  
- Add new components or updated datasheets here as hardware changes.  
- Keep datasheets updated and remove obsolete versions to avoid confusion.

---

## Adding new datasheets  

1. Save the datasheet PDF to `/docs/datasheets/` if redistribution is allowed.  
2. Add an entry to this table with a direct relative link and date/version info.  
3. Include any notes that will help future firmware work.

---

If you want, I can help you generate this catalog from your existing hardware list or even parse datasheets to extract the key firmware sections automatically!
