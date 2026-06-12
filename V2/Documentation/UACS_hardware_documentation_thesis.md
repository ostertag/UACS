# Chapter 1 — System Overview

## Project: UACS (RFID Audio CAN Node)

**Repository:** https://github.com/ostertag/UACS  
**Microcontroller:** STM32H523CET6 (Arm Cortex-M33, LQFP-48)  
**EDA tools:** KiCad (schematic + PCB), STM32CubeMX (pin/peripheral assignment)  
**Debug hardware:** STLINK-V3PWR  
**Component sourcing:** LCSC / JLCPCB

## Introduction

UACS is a custom-designed printed circuit board that integrates contactless RFID reading, audio feedback through a loudspeaker, CAN FD communication, and SD-card audio storage, built around an STM32H523CET6 microcontroller. The board communicates over a CAN FD link, carried over standard RJ45/Cat5 cabling, to a host computer (a Raspberry Pi).

The STM32H523xx is a high-performance microcontroller built on the Arm Cortex-M33 32-bit core, operating at up to 250 MHz, with a single-precision FPU and TrustZone security extension. *[1]*

## System Architecture

The system is a single-board reader node whose main functional blocks are:

- **Microcontroller** — STM32H523CET6 (Cortex-M33, 250 MHz, LQFP-48), central coordinator of all peripherals.
- **RFID reader** — RC522 module (NXP MFRC522), reads ISO/IEC 14443 A / MIFARE cards and tags over SPI.
- **Audio amplifier** — MAX98357A, digital Class D amplifier with I²S input; drives the loudspeaker directly.
- **SD card** — micro-SD slot for audio file storage, connected over SPI.
- **CAN FD transceiver** — TCAN1057A, links the board to the host over Cat5/RJ45 cabling.
- **Power supply** — dual 5 V input (USB-C and RJ45), stepped down to 3.3 V by an MCP16301H buck converter.
- **USB-C interface** — USB 2.0 full-speed device, used for firmware flashing (DFU) and debugging.
- **Indicator LEDs** — two dual-colour (red/green) SMD LEDs for user feedback.

## Scope and Goal of This Work

This work continues the UACS project previously developed in the Deadlock project series. *[predecessor]* The principal goal is a thoroughly documented hardware design that can be reliably reproduced. In particular:

- **Reproducibility** — every component value is either calculated from first principles or traced to a specific datasheet reference, so the design can be rebuilt without reliance on undocumented institutional knowledge.
- **Availability** — components are sourced from LCSC/JLCPCB and selected for availability, good documentation, and competitive cost.
- **Robustness** — the dual power input, remote reset, and remote bootloader-entry capability allow firmware updates and recovery without physical access to a deployed board.

> **Note on references.** Inline citations use italic bracketed numbers, e.g.  *[1]*, referring to the numbered list at the end of the document. References 1–11 are component datasheets; 12–23 are technical literature.

---

# Chapter 2 — Design and Assembly of the UACS PCB

The UACS reader is a printed circuit board on which an STM32H523CET6 microcontroller, an RFID front-end module, an I²S audio amplifier, a micro-SD slot, a CAN FD transceiver, and associated connectors are assembled. User feedback is provided by two dual-colour LEDs. The board can be powered either from a USB-C connector or from the RJ45 CAN cable.

## 2.1 Microcontroller — STM32H523CET6

The STM32H523CET6 is the central controller of the board. It is supplied in an LQFP-48 package and runs the Cortex-M33 core at 250 MHz. *[1]* All peripheral communication — SPI, I²S, FDCAN, and USB — is handled by this device.

### 2.1.1 Debug Interface (SWD)

The board exposes a 10-pin 1.27 mm-pitch Cortex debug connector (SWD1), driven by an STLINK-V3PWR probe.

The STM32H523 supports both JTAG and Serial Wire Debug (SWD), as well as the Embedded Trace Macrocell (ETM). *[1]* SWD was selected over JTAG because it reduces the debug interface to two active signals — SWDIO and SWCLK — while providing equivalent core-debug and memory-access capability to the higher-pin-count JTAG interface. *[12][13]* On a pin-constrained LQFP-48 device, freeing the JTAG pins (TDI, TDO, nTRST) for other functions is a meaningful saving, and SWD is the conventional default for Cortex-M bring-up. *[13]*

The Serial Wire Output trace pin (TRACESWO) is brought out on PB3, enabling single-wire instrumentation trace (printf-style debug output) without consuming a UART peripheral. The connector follows the standard 10-pin 1.27 mm Cortex-debug pinout, which balances size against reliability. *[13]*

The STLINK-V3PWR probe was already available and was therefore the natural choice. In addition to SWD/SWV (supported up to 10 MHz) it integrates a source measurement unit (SMU) with a programmable 1.6–3.6 V output and current measurement from the nanoampere range up to 500 mA. *[4]* This capability is useful during development for verifying the board's power budget, since the entire 3.3 V system is supplied by a single 1 A regulator. The probe is supported directly by the STM32CubeMonitor-Power software tool. *[4]*

### 2.1.2 Reset Circuit

The NRST line is shared between three reset sources: the on-board push button (RESET1), the remote reset line carried over the RJ45 connector (Section 2.4.2), and the STLINK debug probe. Tying the probe's reset into the same net improves recovery when firmware or a low-power state would otherwise block a debug attach. *[13]*

#### Resistor R5

A 10 kΩ pull-up resistor holds the NRST line high during normal operation. Without it the line would float and could trigger spurious resets from noise.

#### Capacitor C3

A 100 nF capacitor from NRST to GND provides debounce for the mechanical reset button and protection against parasitic resets. ST explicitly recommends an external capacitor on NRST for exactly this purpose. *[1]*

### 2.1.3 BOOT0 Circuit

BOOT0 selects whether the MCU starts from user flash (normal operation) or from the embedded system bootloader. The STM32H523 embedded bootloader, located in system memory and programmed by ST during production, can reprogram the flash over USART, I²C, I³C, SPI, FDCAN, or USB-FS (via DFU). *[1]* On UACS, USB-FS DFU is the relevant path (Section 2.6).

BOOT0 is additionally exposed on the RJ45 connector (Section 2.4.2), so the bootloader can be entered remotely without physical access to the board.

#### Resistor R6

A 10 kΩ pull-down resistor holds BOOT0 low during normal operation. With BOOT0 low, the device boots from user flash at address 0x0800 0000. *[1]*

#### Resistor R7

A 10 kΩ series resistor is placed between the R6 junction and the BOOT1 switch to limit current and provide isolation between the pull-down and the switch.

#### Switch BOOT1

An SPDT switch that selects GND (normal boot) or 3.3 V (bootloader entry). Connecting BOOT0 to 3.3 V causes the MCU to enter the embedded system bootloader on the next reset.

### 2.1.4 Crystal Oscillator

A 16 MHz crystal (Y1, SMD3225-4P, 10 pF load) provides the high-speed external (HSE) clock. The STM32H523 supports a 4–50 MHz HSE oscillator. *[1]* Although the device has an internal 64 MHz HSI oscillator, an external crystal is used because the USB and CAN peripherals require a more accurate and stable time base than the internal RC oscillator can guarantee. The USB full-speed interface has particularly tight frequency-tolerance requirements.

No 32.768 kHz crystal is fitted; the LSE pins (PC14/PC15) are left unconnected and configured in Analog mode.

#### Capacitors C1 and C2

Two 10 pF C0G load capacitors are placed to GND, one on each side of the crystal, matched to its specified 10 pF load capacitance. C0G (Class 1) dielectric is used because its capacitance is essentially independent of temperature and applied voltage — typically within ±30 ppm/°C — which is important for a stable oscillator load. *[19][20]*

### 2.1.5 VCAP — Internal Regulator Capacitors

The STM32H5 family integrates an LDO voltage regulator (nominally 3.3 V → 1.2 V core) that requires external stabilisation capacitors on its VCAP pin(s). *[1]* The datasheet specifies this requirement explicitly in Section 5.3.2, "VCAP external capacitor". *[1]*

#### Capacitors C14 and C15

Two 2.2 µF capacitors, one per VCAP pin, are placed to GND. These are mandated by the datasheet for the internal regulator's stability and are not interchangeable with generic decoupling — the value and placement are part of the regulator's stability requirement.

### 2.1.6 Power Pins and Decoupling

The STM32H523 uses several supply domains: the main VDD digital supply, an analog supply (VDDA), a USB supply domain, and the VBAT backup domain. *[1]*

- **VDD** — supplied from +3.3 V; decoupled with C8–C11 (100 nF each) for high-frequency bypass, plus C12 (10 µF) for bulk capacitance.
- **VDDA** — analog supply, filtered from the 3.3 V rail through the VDDA filter (Section 2.3.3).
- **VBAT** — tied to the 3.3 V rail, since no backup battery is used.
- **VSS / VSSA** — ground.

### 2.1.7 RFID Reader — RC522 Module (MFRC522)

The RFID front end is an RC522 module based on the NXP MFRC522 IC, mounted on an 8-pin 2.54 mm socket (RC1) and connected to the MCU over SPI.

The RC522 module was selected because it was used in the predecessor project, allowing existing experience and firmware to be carried over. The MFRC522 is a 13.56 MHz reader/writer IC supporting the ISO/IEC 14443 A / MIFARE protocol family — MIFARE Mini, 1K, 4K, Ultralight, DESFire EV1, MIFARE Plus, and NTAG — with a host SPI interface running up to 10 Mbit/s. *[3]*

**Interface signals:**

- **SPI** — shares SPI1 with the SD card (Section 2.2); chip select is software-controlled on PB14 (RFID_SS), active low.
- **RST (PB10)** — the MCU drives the module's NRSTPD pin as a push-pull output. The pin defaults high at boot so the reader starts in normal operation; driving it low triggers a hard reset/power-down of the module. *[3]*
- **IRQ (PB13)** — the module's interrupt output drives EXTI13, configured for falling-edge interrupts, enabling interrupt-driven card detection rather than polling.

On the MFRC522, the IRQ pin is open-drain and active-low by default, *[3]* which matches the MCU's falling-edge EXTI configuration; only an MCU-side internal pull-up is required on PB13, with no external resistor. The MFRC522's CommIEnReg register must be configured in firmware to enable interrupt propagation to the IRQ pin. *[3]*

#### Capacitor C21

A 10 µF local bypass capacitor is placed on the module VCC pin. VCC connects directly to the 3.3 V rail. (PB12 is left unconnected; an earlier plan to use it for switched module power was dropped, since a single GPIO cannot source the MFRC522's worst-case current.)

---

## 2.2 SD Card — MLD-TF PUSH-H18

The micro-SD slot (U3) stores the audio files played back through the amplifier.

The preferred interface would have been the dedicated SDMMC peripheral, which provides higher throughput than SPI. However, the SDMMC peripheral on the STM32H523 is only available on larger packages; on the LQFP-48 the required pins are not bonded out, so SDMMC is physically unavailable and SPI is the only option. *[1]* This is a package constraint, not a design preference.

The SD card shares SPI1 with the RC522 reader, with its own software chip select on PB0 (SD_SS). In SPI mode, the card's CD/DAT3 pin (slot pin 2) functions as the active-low chip select; this is confirmed by the slot's pin table, which labels pin 2 as CD/DAT3 with I/O type PP. *[8]*

The MLD-TF PUSH-H18 uses a push-push ejection mechanism (card ejects on the second press); the card-detect switch is a separate mechanical contact from the signal pins. *[8]*

#### Card Detect — PB1

The slot's dedicated card-detect mechanical contact drives PB1 (SD_DETECTED) as a GPIO input, allowing the firmware to detect card insertion and removal.

#### Resistors R2 and R3

Two 10 kΩ pull-up resistors are placed to 3.3 V on the DAT1 line (R2) and the DAT2 line (R3). In SPI mode both of these data lines are unused; the pull-ups keep them at a defined high level rather than floating.

---

## 2.3 Power Supply

### 2.3.1 Architecture

The board accepts power from two independent 5 V sources, diode-OR-ed onto a common +5 V rail and then stepped down to 3.3 V by a buck converter.

| Source | OR-ing diode | ESD protection |
|--------|--------------|----------------|
| USB-C VBUS | D8 (PMEG2010ER) | D7 (PESD5V0U1BA,115) |
| RJ45 pair (brown) | D1 (PMEG2010ER) | D2 (PESD5V0U1BA,115) |

The two inputs serve different operational scenarios. The RJ45 input is the primary supply during normal deployment, where the board is powered from the CAN cabling infrastructure. The USB-C input is used during development and programming, when the board is connected to a PC without the full CAN harness. Diode OR-ing lets both sources be connected simultaneously without back-feeding. Each TVS diode is placed as close as possible to its respective input connector for effective clamping. *[10]*

### 2.3.2 Buck Converter — MCP16301H

The MCP16301H (U7) is a high-voltage-input, integrated-switch, fixed-frequency step-down converter in a SOT-23-6 package. *[2]* It is AEC-Q100 Grade 1 automotive-qualified and can supply up to 1 A at 3.3 V output. *[2]* The part was carried over from the previous version of the design, where it was already proven.

The UACS buck stage follows the MCP16301H datasheet's own 3.3 V reference design very closely, which is a deliberate choice for a known-good result.

#### Inductor L1

A 15 µH inductor is used. The datasheet explicitly recommends 15 µH for a 3.3 V output, derived from the inductor-selection constant K:

$$K = \frac{V_{OUT}}{L}$$

where K should equal 0.22 V/µH, giving:

$$L = \frac{V_{OUT}}{K} = \frac{3.3}{0.22} \approx 15 \, \mu H$$

| $V_{OUT}$ | K (V/µH) | Standard L |
|-----------|----------|------------|
| 2.0 V | 0.20 | 10 µH |
| 3.3 V | 0.22 | 15 µH |
| 5.0 V | 0.23 | 22 µH |
| 12 V  | 0.21 | 56 µH |
| 15 V  | 0.22 | 68 µH |

#### Diode D9 (B140-E3, SMA)

Schottky freewheeling diode; the datasheet reference design uses the B140. *[2]*

#### Diode D6 (1N4148WS)

Boost (bootstrap) diode. The datasheet specifically recommends a 1N4148 for this role for its fast recovery speed, voltage blocking capability, wide availability, and low cost. *[2]*

#### Capacitor C22

A 100 nF boost capacitor. The datasheet recommends a 0.1 µF X5R/X7R capacitor for all applications. *[2]*

#### Capacitors C23, C24, C25

C23 (10 µF) is the input capacitor; C24 and C25 (10 µF each) are output capacitors placed in parallel. Their values are consistent with the datasheet's recommended input/output capacitance. *[2]*

#### Diode D5 (PMEG2010ER)

Back-powering protection diode between the 3.3 V output and the 5 V input, reverse-biased during normal operation. It prevents the 3.3 V rail from back-feeding the 5 V input when only the 3.3 V side is powered — for example, when supplied from the STLINK probe during development.

#### Feedback Divider (R18 and R21) — Output Voltage Setting

The MCP16301H regulates to an internal feedback reference of **0.800 V (typical)**. The output voltage is set by an external resistor divider according to the datasheet formula: *[2]*

$$R_{TOP} = R_{BOT} \times \left(\frac{V_{OUT}}{V_{FB}} - 1\right)$$

equivalently:

$$V_{OUT} = V_{FB} \times \left(1 + \frac{R_{TOP}}{R_{BOT}}\right)$$

With the values used on UACS (R18 = 75 kΩ = $R_{TOP}$, R21 = 24 kΩ = $R_{BOT}$):

$$V_{OUT} = 0.800 \times \left(1 + \frac{75}{24}\right) = 0.800 \times 4.125 \approx 3.30 \, V \quad \checkmark$$

> **Note:** The datasheet's own example 3.3 V divider uses $R_{TOP}$ = 31.6 kΩ and $R_{BOT}$ = 10 kΩ. *[2]* UACS uses higher-value resistors (75 kΩ / 24 kΩ); both ratios produce 3.3 V. The higher values were carried over from the predecessor design, where they were chosen to reduce quiescent current through the divider.

### 2.3.3 VDDA Filter

The MCU analog supply (VDDA) is filtered from the digital 3.3 V rail to keep switching noise out of the analog domain (ADC reference, internal oscillator). A ferrite bead in series with the supply, combined with decoupling capacitors on each side, forms a low-pass filter that absorbs high-frequency noise and dissipates it as heat; this is the standard technique for isolating a sensitive analog supply from a noisier digital rail. *[21]*

#### Ferrite Bead FB1

A 120 Ω @ 100 MHz, 500 mA, 0603 ferrite bead acts as the series filter element, presenting high impedance to high-frequency noise while passing DC with negligible voltage drop.

#### Capacitor C18

A 1 µF capacitor placed before the ferrite bead, from the digital 3.3 V rail to GND. Together with the bead, it forms the input side of the low-pass filter.

#### Capacitors C19 and C20

A 1 µF and a 10 nF capacitor placed after the ferrite bead, from the VDDA node to GND. The combination of C19 and C20 provides filtering across a wide frequency range, covering both mid-frequency and high-frequency noise at the filtered VDDA node.

---

## 2.4 CAN Bus — TCAN1057A

The CAN interface links the board to the host Raspberry Pi. The transceiver is the TCAN1057AVDRQ1 (U4), an automotive-grade CAN FD device in a SOIC-8 package, driven by the MCU FDCAN1 peripheral (PB7 TX, PB8 RX). *[7]*

The TCAN1057A was selected because it meets the required parameters, including CAN FD support. CAN FD (Flexible Data-rate) extends classical CAN by allowing the data phase to be transmitted at a higher bit rate than the arbitration phase, and by increasing the maximum payload from 8 to 64 bytes per frame. *[14][15]* In practice CAN FD data-phase rates of up to 5 Mbit/s are common, *[15]* giving the link more bandwidth and headroom than classical CAN, while remaining compatible with the familiar CAN arbitration and error-handling mechanisms. *[14]*

### 2.4.1 Transceiver Configuration

#### Capacitors C16 and C17

Two 100 nF bypass capacitors are placed on the transceiver VCC and GND pins for high-frequency supply decoupling.

#### Resistor R12 — Mode Select Pin

A 10 kΩ resistor ties the S (mode-select) pin permanently to GND, placing the transceiver in Normal Mode. The datasheet (Table 8-4) confirms: S = Low → Normal Mode (driver and receiver both enabled); S = High → Silent Mode (driver disabled, receive-only). The datasheet explicitly states that when normal mode is the only intended operating mode, the S pin may be tied directly to GND via a pull-down resistor. *[7]*

### 2.4.2 RJ45 Cabling and Pair Assignment

The bus is carried over standard RJ45/Cat5 cabling. The eight conductors are assigned as four pairs:

| Connector pins | Pair function |
|----------------|---------------|
| 1 + 2 | GND / +5 V (power) |
| 3 + 6 | BOOT0 / NRST (remote MCU control) |
| 4 + 5 | CANH / CANL (differential bus) |
| 7 + 8 | GND / +5 V (power) |

In a Cat5 cable, pins 4 and 5 form the centre, tightest-twisted pair. The differential CAN signal is assigned to this pair because tight twisting maximises noise rejection: both conductors pick up external interference almost equally as common-mode, which the differential receiver then rejects.

Carrying BOOT0 and NRST over the cable allows remote firmware flashing and reset without physical access to the deployed board.

#### Diodes D3 and D4 (PMEG2010ER)

Protection diodes placed on the BOOT0 (D3) and NRST (D4) lines respectively, guarding against overvoltage transients on these control signals.

### 2.4.3 Bus Termination

Termination is the most involved part of the CAN design and is documented in full below.

**Why termination is required.** A CAN bus is a transmission line whose characteristic impedance (≈120 Ω for twisted-pair CAN cable) must be matched at both ends. Without 120 Ω termination at each end, signal edges reflect from the unterminated ends and corrupt communication. Terminating both ends absorbs the signal energy and prevents these reflections. *[16]*

**Why split termination instead of a single 120 Ω resistor.** In a split-termination scheme the single 120 Ω resistor is replaced by two 60 Ω resistors in series, with a capacitor from their midpoint to ground. The differential impedance remains 120 Ω (60 + 60), so CAN signalling is unaffected, while the midpoint capacitor provides a low-impedance path to ground for high-frequency common-mode noise — improving electromagnetic compatibility (EMC). *[17][18]* This is particularly valuable on a board that also carries a switching regulator and a multi-metre cable run.

**Why two 120 Ω resistors in parallel per side.** Each 60 Ω half is implemented using two 120 Ω resistors in parallel, rather than a single 60 Ω part. Single 60 Ω resistors in the 0805 footprint are less commonly stocked and more expensive; 120 Ω is a very common value, and two in parallel give exactly 60 Ω. A secondary benefit is that power dissipation is split across two resistors, halving the dissipation each must handle.

#### Resistors R8 ∥ R9 (CANH side)

Two 120 Ω resistors in parallel, giving 60 Ω, placed in series on CANH. Each resistor is 0805, 1%, 0.125 W.

#### Resistors R10 ∥ R11 (CANL side)

Two 120 Ω resistors in parallel, giving 60 Ω, placed in series on CANL. Same specification as R8/R9.

#### Capacitor C7

A 4.7 nF X7R, 50 V capacitor from the split-termination midpoint to GND. The chosen value sits within the commonly recommended 4.7 nF–10 nF range for common-mode filtering. *[17]*

### 2.4.4 High-Frequency Bypass and ESD Protection

#### Capacitors C6 and C13

Two 10 pF C0G, 50 V capacitors are placed from CANH (C6) and CANL (C13) respectively to GND, providing additional high-frequency bypass on the bus lines. C0G (Class 1) dielectric is specified deliberately: its capacitance is stable with voltage and temperature, whereas a Class 2 dielectric such as X7R varies with both and would risk rounding the fast CAN FD signal edges. *[19][20]*

#### U5 — ESD Protection (ESD2CAN24-ES, SOT-23-3)

A CAN-specific TVS/ESD protection device placed on the bus side of the filtering network. The device is rated for ±30 kV IEC 61000-4-2 contact and air discharge, with a 24 V reverse standoff voltage and a typical junction capacitance of 18 pF, making it well-suited for a CAN FD bus. *[11]*

---

## 2.5 Audio Amplifier — MAX98357A

The MAX98357A (U2) is a digital-input Class D audio amplifier in a TQFN-16 package, supplied from 3.3 V. It receives audio over I²S and drives the loudspeaker directly.

The MAX98357A was selected for several reasons: it integrates an I²S receiver and a Class D amplifier in a single device, eliminating the separate DAC/translator, amplifier, and output-filter stages that a discrete audio path would require; it drives the speaker directly with no external output filter; it provides hardware-selectable gain; and it comes from a reputable manufacturer with thorough documentation. *[6]*

The amplifier connects to the MCU I²S peripheral over three lines:

- **DIN — PB15 (I2S_SDO)** — serial audio data
- **BCLK — PB2 (I2S_BCLK)** — bit clock
- **LRCLK — PA3 (I2S_WS)** — word select / left-right clock

The exposed pad (EP) is tied to GND. The speaker connects through a Molex 1.25 mm right-angle connector (Speaker2).

### 2.5.1 SD_MODE / Shutdown Control

The SD_MODE pin controls both shutdown and the output channel mode. It is driven from PA4 (SPEAKER_ON/OFF) as a push-pull output. PA4 defaults low at boot, so the amplifier starts in shutdown.

#### Resistor R4 — SD_MODE Series Resistor

The output channel mode is set by the value of the resistor placed between the GPIO and the SD_MODE pin. Per the MAX98357A datasheet  *[6]* formula:

$$R = 222.2 \times V_{DDIO} - 100 \quad [\text{result in k}\Omega]$$

Substituting the 3.3 V supply:

$$R = 222.2 \times 3.3 - 100 \approx 634 \, k\Omega$$

A **634 kΩ ±1 %** resistor (R4) is used. With this configuration:

- **GPIO HIGH (3.3 V)** → SD_MODE pulled high through R4, placing the amplifier in (L/2 + R/2) mono-mix mode (active).
- **GPIO LOW (0 V)** → amplifier in shutdown / deep sleep.

### 2.5.2 Gain Configuration (GAIN_SLOT)

The amplifier gain is set by how the GAIN_SLOT pin is connected. The datasheet  *[6]* defines five discrete settings (Table 8):

| GAIN_SLOT connection | Gain (dB) |
|----------------------|-----------|
| 100 kΩ to GND | 15 |
| Direct to GND | 12 |
| Unconnected | 9 |
| Direct to VDD | 6 |
| 100 kΩ to VDD | 3 |

To make all five settings selectable in hardware, the board uses a two-switch network:

- **SW1 (SP3T)** — selects the path as open (NC), direct (0 Ω), or through a 100 kΩ resistor.
- **SW2 (SPDT)** — selects whether that path connects to GND or VDD.

The default state (GAIN_SLOT unconnected) gives **9 dB**.

#### Resistor R1

A 100 kΩ gain-setting resistor used for the 3 dB and 15 dB settings. Its value is taken directly from the datasheet gain table. *[6]*

### 2.5.3 Speaker and Power Budget

An **8 Ω, 1 W** speaker is used. The output power into the load is estimated from the supply voltage and load resistance:

$$P = \frac{V_{DD}^2}{2 \times R_L} = \frac{3.3^2}{2 \times 8} \approx 0.68 \, W$$

An 8 Ω load was chosen deliberately over 4 Ω. At 4 Ω, the output power and corresponding current draw roughly double, which would push the peak current close to the 1 A limit of the single buck regulator supplying the whole board. The 8 Ω choice keeps the peak current comfortably within budget.

### 2.5.4 Decoupling

#### Capacitors C4 and C5

C4 (100 nF) and C5 (10 µF) provide supply decoupling on the amplifier VDD pins, covering both high-frequency and bulk requirements.

---

## 2.6 USB-C Interface

The USB-C interface (USB-C1) is a 14-pin horizontal SMD Type-C receptacle used to connect the board to a PC.

The interface is USB 2.0 only — carried over from the previous version; there is no requirement for higher transfer rates in this application. The board operates purely as a USB device (USB_DRD_FS), used for firmware flashing (USB-FS DFU) and debugging; it does not act as a host. The STM32H523 provides one USB 2.0 full-speed device interface. *[1]*

The USB 2.0 differential data pair uses pins PA11 (D+) and PA12 (D−).

### Schematic

The USB-C circuit follows the standard UFP (upstream-facing port / device) topology as specified in the USB Type-C specification. *[22][23]*

### Components and Their Values

#### Resistors R19 and R20

Two 5.1 kΩ Rd pull-down resistors are placed on CC1 and CC2 to GND. The USB Type-C specification requires a device (UFP) to present a 5.1 kΩ ±10 % Rd pull-down on each CC pin; this is the only acceptable value for advertising a standard Type-C current contract. *[22][23]* The two pins are pulled down independently — not tied together. *[23]*

#### Resistor R13

A 1.5 kΩ resistor is placed between the 3.3 V rail and VBUS for VBUS sensing.

#### Diode D7 (PESD5V0U1BA,115)

A bidirectional TVS diode on VBUS for ESD protection. The device provides a 5 V reverse standoff voltage, 2.9 pF typical capacitance, and up to 10 kV IEC 61000-4-2 protection in a SOD323 package. *[10]*

#### Diode D8 (PMEG2010ER)

An OR-ing Schottky diode feeding VBUS into the shared +5 V power rail. *[9]*

SBU1 and SBU2 pins are left unconnected, as they are not required for USB 2.0 device operation. The shield and GND (A1) are tied to GND.

---

## 2.7 Indicator LEDs — APHBM2012 Series

The board carries two dual-colour (red + emerald-green) SMD indicator LEDs from the Kingbright APHBM2012 series. Each LED has independent red and green elements with common cathodes to GND, so each colour is driven through its own current-limiting resistor from a GPIO.

| Ref | Part | Nominal current | Signals |
|-----|------|-----------------|---------|
| LED1 | APHBM2012LSURKZGKC | 2 mA (low current) | Green: LED_G1 (PB4); Red: LED_R1 (PA15) |
| LED2 | APHBM2012LSURKZGKC | 2 mA (low current) | Green: LED_G2 (PB6); Red: LED_R2 (PB5) |

The predecessor board used two different LED variants (2 mA and 20 mA) because the required brightness was unknown during the initial design. Once confirmed that 2 mA provides sufficient visibility for the application, both LEDs were standardised to the low-current APHBM2012LSURKZGKC variant, simplifying the BOM.

### Schematic

Each LED element is driven from a GPIO through a series current-limiting resistor, with the cathode tied to GND.

### Components and Their Values

#### Current-Limiting Resistors

The current-limiting resistor value is calculated from the supply voltage, the LED forward voltage, and the target forward current:

$$R = \frac{V_{DD} - V_f}{I_f} \quad \text{with } V_{DD} = 3.3 \, V$$

Using the typical forward voltages from the Kingbright datasheets: *[5]*

**LED1 — $I_f$ = 2 mA** ($V_f$: Red 1.75 V, Green 2.65 V):

$$R_{red} = \frac{3.3 - 1.75}{0.002} = 775 \, \Omega \quad \rightarrow \quad \textbf{787 Ω chosen (R15)}$$

$$R_{green} = \frac{3.3 - 2.65}{0.002} = 325 \, \Omega \quad \rightarrow \quad \textbf{330 Ω chosen (R14)}$$

Both LEDs are identical, so LED2 uses the same resistor values: R17 = 787 Ω (red) and R16 = 330 Ω (green).

In both cases the nearest standard resistor value was selected.

---

# Chapter 3 — Resistor Value Summary

| Ref(s) | Value | Basis |
|--------|-------|-------|
| R1 | 100 kΩ | MAX98357A GAIN_SLOT setting resistor (datasheet Table 8) *[6]* |
| R2, R3 | 10 kΩ | SD SPI-mode pull-ups (standard practice) |
| R4 | 634 kΩ | MAX98357A SD_MODE: $R = 222.2 \times V_{DDIO} - 100$ → mono mode  *[6]* |
| R5 | 10 kΩ | NRST pull-up (datasheet-recommended NRST protection) *[1]* |
| R6, R7 | 10 kΩ | BOOT0 pull-down / series (standard) |
| R8–R11 | 120 Ω | CAN split termination, 2 × 120 Ω ∥ = 60 Ω per line  *[16][17]* |
| R12 | 10 kΩ | TCAN1057A S-pin pull-down → Normal Mode  *[7]* |
| R13 | 1.5 kΩ | USB VBUS sense |
| R14 | 330 Ω | LED1 green: $(3.3 - 2.65) / 2\,\text{mA}$ *[5]* |
| R15 | 787 Ω | LED1 red: $(3.3 - 1.75) / 2\,\text{mA}$ *[5]* |
| R16 | 330 Ω | LED2 green: $(3.3 - 2.65) / 2\,\text{mA}$ *[5]* |
| R17 | 787 Ω | LED2 red: $(3.3 - 1.75) / 2\,\text{mA}$ *[5]* |
| R18, R21 | 75 kΩ / 24 kΩ | MCP16301H feedback divider, $V_{FB}$ = 0.8 V; carried over from predecessor design  *[2]* |
| R19, R20 | 5.1 kΩ | USB-C CC Rd pull-downs (Type-C spec) *[22][23]* |

---

# Chapter 4 — Summary of Design Decisions

| Decision | Reasoning |
|----------|-----------|
| STM32H523CET6 (LQFP-48) | Cortex-M33 at 250 MHz; central controller *[1]* |
| SWD debug (not JTAG) | Two-signal interface, frees pins on LQFP-48 *[12][13]* |
| SPI for SD card (not SDMMC) | SDMMC pins unavailable on LQFP-48 — package constraint *[1]* |
| RC522 RFID module | Carried over from predecessor; known design *[3]* |
| MAX98357A amplifier | I²S receiver + Class D amp + direct drive in one device; settable gain; reputable vendor |
| 8 Ω speaker (not 4 Ω) | Keeps peak current within the 1 A regulator budget |
| MCP16301H buck | Carried over; follows datasheet 3.3 V reference design closely *[2]* |
| Dual power input (USB + RJ45) | RJ45 for deployment, USB for development/programming |
| TCAN1057A CAN FD | Meets parameters; CAN FD for higher data rate *[14][15]* |
| Split CAN termination | Maintains 120 Ω differential while filtering common-mode noise *[16][17]* |
| 2 × 120 Ω parallel per side | 60 Ω parts costly/rare in 0805; also splits dissipation |
| C0G caps on CAN/crystal | Stable capacitance vs. voltage/temperature *[19][20]* |
| Ferrite-bead VDDA filter | Isolates analog supply from switching noise *[21]* |
| USB 2.0 device only | Carried over; sufficient for flashing/debug |
| External 16 MHz crystal | Accuracy needed for USB/CAN timing *[1]* |
| VCAP 2.2 µF caps | Mandatory for STM32H5 internal regulator *[1]* |
| LED resistor values | $R = (V_{DD} - V_f) / I_f$, nearest standard value *[5]* |

---

# References

## Component Datasheets (1–12)

 *[1]* STMicroelectronics, *STM32H523xx datasheet* (DS14540). https://www.st.com/resource/en/datasheet/stm32h523ce.pdf

 *[2]* Microchip, *MCP16301/H High-Voltage Input Integrated Switch Step-Down Regulator* (DS20005004D). http://ww1.microchip.com/downloads/en/DeviceDoc/20005004D.pdf

 *[3]* NXP, *MFRC522 Standard performance MIFARE and NTAG frontend* (Rev. 3.9). https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf

 *[4]* STMicroelectronics, *STLINK-V3PWR* product page. https://www.st.com/en/development-tools/stlink-v3pwr.html

 *[5]* Kingbright, *APHBM2012LSURKZGKC datasheet* (LED1, 2 mA). https://www.lcsc.com/datasheet/C6465979.pdf


 *[6]* Analog Devices (Maxim), *MAX98357A/MAX98357B datasheet* (Rev. 16). https://www.analog.com/media/en/technical-documentation/data-sheets/MAX98357A-MAX98357B.pdf

 *[7]* Texas Instruments, *TCAN1057A-Q1 / TCAN1057AV-Q1 Automotive CAN FD Transceiver datasheet* (ZHCSN36B). https://www.lcsc.com/datasheet/C3235000.pdf

 *[8]* Shenzhen Minlianda Technology Co., Ltd., *MLD-TF PUSH-H18 micro-SD push-push slot mechanical drawing* (DWG No. MLD-2509031032, Rev. A/0). https://www.lcsc.com/datasheet/C52750848.pdf

 *[9]* Nexperia, *PMEG2010ER datasheet*. https://assets.nexperia.com/documents/data-sheet/PMEG2010ER.pdf

 *[10]* Nexperia, *PESD5V0U1BA low-capacitance bidirectional ESD protection diode datasheet* (v.2, April 2023). https://www.lcsc.com/datasheet/C552574.pdf

 *[11]* ElecSuper, *ESD2CAN24-ES Normal Capacitance ESD protection datasheet* (Rev. 1.5). https://www.lcsc.com/datasheet/C19170841.pdf

## Technical Literature (13–23)

 *[12]* Arm, *ARM Debug Interface Architecture Specification* — SWD as a two-pin alternative to JTAG providing equivalent functionality. https://developer.arm.com/documentation/ihi0031/latest/

 *[13]* SparkFun, *ARM Programming: JTAG and SWD* — SWD reduces the interface to SWDIO/SWCLK; nRESET improves recovery; 10-pin 1.27 mm Cortex header. https://learn.sparkfun.com/tutorials/arm-programming/jtag-and-swd

 *[14]* Kvaser, *CAN FD Protocol Tutorial* — BRS dual bit rate, 64-byte payload, compatibility with classical CAN arbitration/error handling. https://kvaser.com/can-fd-protocol-tutorial/

 *[15]* NI, *Understanding CAN FD (Flexible Data-Rate) vs. CAN* — data-phase rates commonly up to 5 Mbit/s; 64-byte payload. https://www.ni.com/en-us/innovations/white-papers/14/understanding-can-with-flexible-data-rate--can-fd-.html

 *[16]* All About Circuits / Eric Hackett (TI), *Overcoming CAN Design Challenges: How to Easily Terminate CAN Signals* — necessity of 120 Ω termination at both ends to prevent reflections. https://www.allaboutcircuits.com/industry-articles/overcoming-can-design-challenges-can-signal-termination-made-easy/

 *[17]* CAN Bus Debugger, *CAN Bus Termination Explained* — split termination: two 60 Ω resistors with a 4.7 nF–10 nF midpoint capacitor to ground; differential impedance stays 120 Ω. https://www.canbusdebugger.com/articles/can-bus-termination

 *[18]* Texas Instruments, *How Termination CAN Improve EMC Performance in a CAN Transceiver* (SSZTAM0) — split termination stabilises common-mode voltage and filters common-mode noise. https://www.ti.com/document-viewer/lit/html/SSZTAM0

 *[19]* Knowles Capacitors, *Capacitor Fundamentals Part 8: Dielectric Classifications* — C0G/NP0 Class 1: ±30 ppm/°C, stable with voltage, low loss. https://blog.knowlescapacitors.com/blog/capacitor-fundamentals-part-8-dielectric-classifications

 *[20]* All About Circuits, *X7R, X5R, C0G…: A Concise Guide to Ceramic Capacitor Types* — C0G stable vs temperature/voltage/aging; X7R Class 2 varies with both. https://www.allaboutcircuits.com/technical-articles/x7r-x5r-c0g...-a-concise-guide-to-ceramic-capacitor-types/

 *[21]* Analog Devices, *AN-1368: Ferrite Bead Demystified* — ferrite bead + decoupling capacitors form a low-pass filter protecting sensitive loads (ADC, PLL) from switching noise. https://www.analog.com/en/resources/app-notes/an-1368.html

 *[22]* Infineon, *USB Type-C connector: Rp, Rd, and Ra termination resistors* (KBA97180) — UFP Rd fixed at 5.1 kΩ. https://community.infineon.com/t5/Knowledge-Base-Articles/USB-Type-C-connector-Rp-Rd-and-Ra-termination-resistors/ta-p/253544

 *[23]* Microchip Technology, *Introduction to USB Type-C* (DS00001953A) — UFP must present 5.1 kΩ ±10 % Rd on both CC pins; pins pulled down independently. https://ww1.microchip.com/downloads/en/AppNotes/00001953A.pdf

 *[predecessor]* Previous UACS/Deadlock project documentation (predecessor board revision).

---

*This document is the distilled hardware design rationale for the UACS project and will be extended into a full bachelor thesis in the following semester. Inherited component values (e.g. the MCP16301H feedback divider, the two LED variants) will be expanded using the predecessor project's documentation.*
