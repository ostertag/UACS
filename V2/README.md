# List of new changes

## Čo treba spraviť všeobecne

* Do symbol properties treba doplniť:
  * Description - kde budú uvedené ďalšie parametre ako (napr. 50V X7R type)
  * JLCPCB linku - na stránku s komponentom (aby sa dalo overiť či je extended)
  * Datasheet - ak nefunguje linka z LCSC (nie je po EN alebo je prázdne PDF), tak na datasheet zo stránky výrobcu
* Pre MAX98357AETE+T:
  * Pridať konektor pre speaker


---

## TCAN1057AVDRQ1

### Implementované zmeny a ochrana
* **Split Termination:** Pridaná split terminácia pre stabilizáciu zbernice. Použité štyri rezistory $120\ \Omega$ (paralelne dve dvojice tvoriace $60\ \Omega$ + $60\ \Omega$, celkovo $120\ \Omega$, výkonovo dimenzované na $0.25\text{ W}$ na rezistor). Stredový bod je filtrovaný kondenzátorom `C5` s hodnotou $4.7\text{ nF}$ ($50\text{ V}$, typ X7R) vedúcim do GND.
* **RF Filtrácia (Bypass):** Pridané vysokofrekvenčné kondenzátory `C8` (CAN_H -> GND) a `C7` (CAN_L -> GND) s bezpečnou hodnotou **$10\text{ pF}$** (typ C0G/NP0, $50\text{ V}$), ktoré nezaobľujú hrany pri CAN-FD (5 Mbps).
* **TVS Ochrana zbernice:** Osadená špecializovaná automotive TVS dióda `U7` (`ESD2CAN24-ES`).
* **Testovacie body:** Pridané test pointy pre `CAN_H` a `CAN_L` umiestnené za vstupným konektorom a pred transceiverom.
* **S pin (FDCAN_OFF):** Pin S je natvrdo stiahnutý na systémovú zem (GND), čím je transceiver permanentne prepnutý do Normal Mode (nutné pre správne odosielanie ACK potvrdzovacích bitov).

### Zmeny v zapojení a referencie
* **Odstránenie sériových odporov:** Odstránili sa $10\ \Omega$ odpory z CANH/L, keď sa pridala split termination.
  * archived - [Top Design Questions About Isolated CAN Bus Design](https://www.ti.com/lit/an/slla486b/slla486b.pdf)
  * archived - [ISO1042: CAN series current limiting resistor value](https://e2e.ti.com/support/isolation-group/isolation/f/isolation-forum/1379831/iso1042-can-series-current-limiting-resistor-value)

### Spoločné napájanie +5V a ESD ochrana vstupov
* Vzhľadom na dva nezávislé napájacie vstupy (**D4** z USB-C a **D7** z Ethernet/PoE konektora) sú pre maximálnu bezpečnosť osadené **2 samostatné TVS diódy Nexperia PESD5V0U1BA,115** (Breakdown 5.5V).
* Každá dióda je umiestnená hneď za príslušnou ochrannou diódou (jedna za D4, druhá za D7) and fyzicky čo najbližšie k samotným vstupným konektorom, aby zachytili ESD iskru okamžite na vstupe a nepustili ju ďalej do spoločnej distribúcie `+5V` na doske.

### Common mode choke
Ak by sme chceli niekedy osadiť [Common mode choke](https://electronics.stackexchange.com/questions/416788/common-mode-choke-parameters-can-circuitry-split-termination-concept).
Pri návrhu PCB postupovať podľa 11.1 Layout Guidelines.

### Kondenzátory C2 C3
TCAN1057AVDRQ1 datasheet states they need to be 100nF.

### FDCAN_OFF
FDCAN_OFF should be set low by default.
* low = normal mode (môže aj odpovedať, potrebné pre normálnu komunikáciu, lebo prijatý paket musí potvrdiť - ACK)
* high = silent mode (môže iba počúvať)

---

## MAX98357AETE+T

### Konfigurácia SD_MODE (Mono Mix a Shutdown)
* Pôvodné priame zapojenie `SD_MODE` na GND (ktoré držalo čip trvalo v Shutdown režime) bolo zrušené.
* **Požadovaný režim:** **Both (Mono Mix $(L+R)/2$)**, aby čip správne sčítal oba audio kanály pre jeden mono reproduktor (ideálne pre stereo aj mono súbory).
* **Hardvérové riešenie:** Na pin `SD_MODE` je pripojený jeden presný pull-up rezistor **$634\text{ k}\Omega\ \pm1\,\%$\ (Yageo RC0805FR-07634KL)**. Hodnota bola vypočítaná presne pre $3.3\text{ V}$ logiku podľa oficiálneho vzorca z datasheetu ($R_{LARGE} = 222.2 \times V_{DDIO} - 100$).
* **Riadenie z MCU:** Tento rezistor je napájaný priamo z GPIO pinu STM32, ktorý je nakonfigurovaný ako **`GPIO_Output` (Push-Pull, bez interných pull-up/pull-down odporov)**. 
  * `LOGICKÁ 1` ($3.3\text{ V}$) na GPIO zapne zosilňovač do Mono Mix režimu.
  * `LOGICKÁ 0` ($0\text{ V}$) na GPIO kompletne uspí čip (Mute / Deep Sleep).
* **Decoupling:** 100nf + 10uf

### Testovacie body
* Pridané testovacie body pre digitálne audio linky: **BCLK**, **LRCLK** a **DIN**.

### GAIN_SLOT
The gain depends on resistance on GAIN_SLOT and if its connected to GND or VDD:

|GAIN_SLOT|I2S/LJ GAIN (dB)|
|-|-|
|Connect to GND through 100kΩ ±5% resistor|15|
|Connect to GND|12|
|Unconnected|9|
|Connect to VDD|6|
|Connect to VDD through 100kΩ ±5% resistor|3|

As such we have 2 paths:
* first to GND (which is by default connected with 0Ω resistor working as 12dB gain).
* second we have path to VDD (which has empty slot for resistor).
* We also have spare unconnected resistor of 100kΩ ±5%.

---

## RC522

### RFID_IRQ
Added to MCU so we can detect when card was read. Use it to interrupt and start new logical cycle.