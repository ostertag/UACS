R17/18 - 10Ω resistor on CAN\_H and CAN\_L lines is current-limiting/surge-protection. more information can be found

* https://www.ti.com/lit/an/slla486b/slla486b.pdf (question 6)
* https://e2e.ti.com/support/isolation-group/isolation/f/isolation-forum/1379831/iso1042-can-series-current-limiting-resistor-value





C2/3 - TCAN1057AVDRQ1 datasheet states they need to be 100nF





FDCAN\_OFF - should be set low by default. low = normal mode, high = silent mode



MAX98357AETE+T GAIN\_SLOT - the gain depends on resistance on its line and if its connected to DNG or VDD.

|GAIN\_SLOT|I2S/LJ GAIN (dB)|
|-|-|
|Connect to GND through 100kΩ<br />±5% resistor|15|
|Connect to GND|12|
|Unconnected|9|
|Connect to VDD|6|
|Connect to VDD through 100kΩ <br />±5% resistor|3|

As such we have 2 paths... one to gnd which is by default connected with 0Ω resistor working as 6dB gain. than we have path to GND which has empty slot for resistor. lastly we have unconnected resistor of 100kΩ ±5%



RFID\_IRQ - added to mcu so we can detect when card was read, use it to interrupt and start new logical cycle.

