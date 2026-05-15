# List of new changes

## Čo treba spraviť všeobecne

- Do symbol properties treba doplniť
  - Description - kde budú uvedené ďalšie parametre ako (napr. 50V X7R type)
  - JLCPCB linku - na stránku s komponentom (aby sa dalo overiť či je extended)
  - Datasheet - ak nefunguje linka z LCSC (nie je po EN alebo je prázdne PDF),
    tak na dataseep zo stránky výrobcu
- Pre TCAN1057AVDRQ1 doplniť:
  - split termination
    - R4, R5 60Ω 1% 0.25W, C3 4.7nF 50V X7R type
  - TVS diode ESD2CAN24
    - [TVS diode](https://e2e.ti.com/support/interface-group/interface/f/interface-forum/1205208/tcan1057av-q1-common-mode-choke-value)
  - S pin (FDCAN_OFF) na GND
    - If normal mode is the only intended mode of operation than the S pin can be
    tied directly to system GND using a pull-down resistor
  - pridať test point pre CAN_H a CAN_L za vstupný konektor a pred transceiver
- Doplniť TVS diode Nexperia PESD5V0U1BA,115 (Voltage - Breakdown 5.5V) na +5V,
  hneď za D7 a D4.
- Prečo je SD_MODE na GND?
  - Podľa [dátového listu](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX98357A-MAX98357B.pdf)
    strana 17 to znamená, že shutdown?
  - Podľa mňa chceme pullup cez R_large (633,26kΩ ?) pre Left + Right.
  - Pridať test pointy na BCLK, LRCLK, DIN
  - Pridať konektor pre speaker
  - Pridáme 0.1uF k C13?

## TCAN1057AVDRQ1

Ak by sme chceli niekedy osadiť [Common mode choke](https://electronics.stackexchange.com/questions/416788/common-mode-choke-parameters-can-circuitry-split-termination-concept).

Pri návrhu PCB postupovať podľa 11.1 Layout Guidelines.

### Resistors R17 and R18

10Ω resistor R17 and R18 on CAN_H and CAN_L lines are used for current-limiting
and surge-protection. More information can be found:

- [Top Design Questions About Isolated CAN Bus Design](https://www.ti.com/lit/an/slla486b/slla486b.pdf)
  (question 6)
- [ISO1042: CAN series current limiting resistor value](https://e2e.ti.com/support/isolation-group/isolation/f/isolation-forum/1379831/iso1042-can-series-current-limiting-resistor-value)

### Kondenzátory C2 C3

TCAN1057AVDRQ1 datasheet states they need to be 100nF

### FDCAN_OFF

FDCAN_OFF should be set low by default.

- low = normal mode
  - môže aj odpovedať
  - potrebné pre normálnu komunikáciu, lebo prijatý paket musí potvrdiť (ACK)
- high = silent mode
  - môže iba počúvať

## MAX98357AETE+T

### GAIN_SLOT

The gain depends on resistance on GAIN_SLOT and if its connected to
DNG or VDD:

|                 GAIN_SLOT                 | I2S/LJ GAIN (dB) |
| ----------------------------------------- | ---------------- |
| Connect to GND through 100kΩ ±5% resistor | 15               |
| Connect to GND                            | 12               |
| Unconnected                               | 9                |
| Connect to VDD                            | 6                |
| Connect to VDD through 100kΩ ±5% resistor | 3                |

As such we have 2 paths:

- first to GND
  - which is by default connected with 0Ω resistor working as 12dB gain.
- second we have path to VDD
  - which has empty slot for resistor.

We also have spare unconnected resistor of 100kΩ ±5%.

## RC522

### RFID_IRQ

Added to MCU so we can detect when card was read. Use it to interrupt and start
new logical cycle.
