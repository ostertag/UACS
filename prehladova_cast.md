# Kapitola 1 Prehľad Systému 

## Predošlá práca na projekte UACS

## Architektúra projektu
Architektúra celého projektu ostáva z väčšej časti rovnaká a to modulárna architektúra skladajúca sa z troch hlavných súčastí:
- Server: Program ktorý zabezpečuje kontrolu celého systému, vykonáva firmwearové updaty, kontroluje pravidlá prístupov do jednotlivých miestností alebo k jednotlivým zariadeniam, udržiava prístupový denník. 
- Controller: Fyzické zariadenie ktoré komunikuje so serverom a s readrami ktoré sú k nemu pripojené. V základnej forme by boli k controleru pripojené dva "readre" a controller by dokázal otvárať dvere. 
- Reader: Fyzické zariadenie ktoré získava informácie od používateľa pomocou podoporovanej autentifikačnej metódy daným readrom. Zároveň poskytuje spätnú väzbu používateľovy pomocou svetelnej a zvukovej signalizácie. V základnej implementácii by reader vedel čítať RFID karty a čipy 


## Rozsah a cieľ práce
Táto práca je oživením projektu Deadlock ktorému sa venovali predošlé práce od K. Součkovej[^1] a A. Deja[^2]. Prvá práca definovala serverové komponenty a komunikačný protokol používanom v projekte deadlock. V druhej práci bol spravený dizajn komponentov pre "Reader" a "Controler" ako aj ich prepojenia a firmwearu na oboch zariadeniach. Avšak keďže sa nedochovala dostatočná dokumentácia nie je možné spoľahlivo zreplikovať nadizajnovaný hardwear. Preto sa táto práca zameriava na dôkladné zdokumentovanie postupu pri dizajnovaní hardwearu a to konkrétne "Readra" ako aj pri dizajnovaní firmwearu pre reader a ďaľších softwearových súčastí. 
Základné ciele tejto práce sú: 
- Modularita systému: Chceme aby sme vedeli meniť spôsoby autententífikácie pri readroch a spôsoby odomykania prístupových bodov
- Robustnosť systému: Pre celkové a spolahlivé fungovanie je potrebné aby celý systém fungoval aj pri nefnkčnosti niektorých zariadení. Zároveň je potrebné aby zariadenia vedeli nahlasovať svoju nefunkčnosť alebo nefunkčnosť niektorých iných súčastí systému. 
- Dostupnosť systému: Jednotlivé súčasti systému a fyzických zariadení sa snažíme vyberať tak aby boli dostupné, dobre zdokumentované a aj cenovo prístupne, aby systém ktorý vytvoríme vedel konkurovať iným riešeniam 
- Reprodukovateľnosť systému: Pre spoľahlivé reprodukovanie komponetov systému je potrebná podrobná technická dokumentácia. Týka sa to hlavne fyzických komponentov, kde treba zohľadňovať veľa faktorov. Jedným z hlavných cieľov práce je teda aj vytvorenie podrobnej, prehľadnej a dostupnej dokumentácie, aby sa na túto prácu dalo jednoducho nadviazať.

## Iné dostupné riešenia 
Na trhu sa vyskytuje mnoho dostupných riešení ktoré vedia zabezpečiť podobnú funkcionalitu ako namy navrhovaný systém. Veľa vývojových platforiem ako Arduino alebo Raspberry PI buď nie sú až tak spoľahlivé alebo nespľňajú úplne všetky parametre. Či už veľkostné alebo možnosti napajania. Zároveň sa tu ťažšie robia cenové optimalizácie a implementuje modularita rôznych rozhraní. 

Ďaľšou podstatnou časťou Readra je antena na čítanie a aj prípadný zápis na RFID karty a čipy. V našom projekte využívame anténu MFRC-522 RC522. Keďže sa jedná o cenovo dostupnú, dobre dokumentovanú a ľahko pripojiteľnú anténu. Na trhu sa vyskutuje veľa rôznych podobných antén, avšak táto konkrétna vychádzala ako najlepšia varianta

## Budúcnosť projektu 

# Kapitola 2 Dizajn a výroba fyzických častí Readra

Reader sa je plošný spoj, na ktorom je umiestnený STM32 mikrokontroler, antená na čítanie rfid kariet, konektory pre pripojenie USB-C, RJ-12 a debugera. Ako používateľské rozhranie slúžia dve dvojfarebné ledky a bzučiak. Napájanie plošného zdroja je možné buď cez USB-C alebo cez RJ-12 kábel vďaka Power over Ethernet (PoE)

## Napäťový zdroj 

Napäťový zdroj nám v našej implementácii slúži na zmenu vstupného
napätia 12 V alebo 5 V na 3.3 V

![Schéma zapojenia napäťového
zdroja](schematic_power_suplly.png)    
*Schéma zapojenia napäťového zdroja*

Schéma je použitá z [dátového
listu](https://github.com/ostertag/UACS/blob/hardwear_kozuch/power_suply_shc_1/Data_sheet.pdf)
súčiastky MCP16301Hx-xCH, konkrétne Figure 6-1 na strane 23.

![Figure 6-1 ](sch_original.png)    
*Figure 6-1*

Niektoré externé komponenty vieme upraviť, aby sme dostali iné výsledné
napätie, prípadne iný prúd. Naša schéma s ktorou pracujeme predpokladá
vstupné napätie 12V prípadne 5V a výstupné napätie 3.3V. Externé
komponenty sú:

-   rezistory:

    -   rezistory $R_1$ a $R_2$:

        -   v dátovom liste sú to $R_{TOP}$ ($R_1$) a $R_{BOT}$ ($R_2$)

        -   od ich hodnôt závisí výstupné napätie

        -   v dátovom liste máme rovnicu na výpočet odporov jednotlivých
            rezistorov:

            $$R_{1} = R_{2} \times \left(\frac{V_{OUT}}{V_{FB}} - 1\right)$$

        -   naše $V_{OUT}$ je napätie ktoré chceme dostať na výstupe a
            $V_{FB}$ je napätie idúce do $V_{FB}$ pinu a musí mať
            hodnotu 0.8V

        -   na nájdenie hodnôt rezistorov môžeme použiť script
            [find_rezistor.py](https://github.com/ostertag/UACS/tree/hardwear_kozuch/power_suply_shc_1/find_rezistor)

        -   snažíme sa hľadať rezistory z čo najnižších sérii

        -   v dátovom liste uvádzajú že $R_2$ by mal mať 10 $k\Omega$ a
            $R_1$ by mal mať 31.6 $k\Omega$ pre výstupné napätie 3.3V

        -   my použijeme hodnoty ktoré nám našiel skript a to 75
            $k\Omega$ pre $R_1$ a 24 $k\Omega$ pre $R_2$ z dôvodu že
            malé hodnoty odporov majú veľký stratový prúd.

-   kondenzátory:

    -   kondenzátor $C_1$:

        -   v dátovom liste je to aj $C_{IN}$

        -   v dátovom liste je uvedené že treba použiť dva paralelne
            zapojené kondenzátory, každý o hodnote 4.7 µF

        -   používame 10 µF kondenzátor aby sme znížili množstvo
            komponentov a náročnosť zapojenia

    -   kondenzátory $C_3$ a $C_4$:

        -   v dátovom liste je to aj $C_{OUT}$

        -   ich hodnota zavisí od výsledného napätia

        -   pre naše výstupné napätie používame dva kondenzátory každý o
            hodnote 10 µF, ktoré budú paralelne zapojené

    -   pri hodnotách kondenzátorov sa riadime tabuľkou 5-2 na strane
        18:

          |Parameter|   Min   |   Max|
          |-----------|--------|------|
          |$C_{IN}$|    2.2 µF|   None|
          |$C_{OUT}$|   20 µF |   None|

    -   zároveň minimálna napätie kondenzátorov musí byť napätie ktoré
        nimi maximálne môže prechádzať plus nejaká rezerva

        -   Ako rezervu zvyčajne používame hodnotu maximálneho napätia,
            ktoré môže kondenzátorom prechádzať, takže napätie
            kondenzátorov bude dvojnásobkom maximálneho napätia, ktoré
            nimi môže prechádzať.

-   cievka:

    -   používame 15 µH cievku

    -   podľa výstupného napätia upravujeme jej hodnotu podľa tejto
        rovnice:

        $$K = V_{OUT}/L$$

    -   kde K by malo mať hodnotu 0.22 V/µH a $V_{OUT}$ je naše výstupné
        napätie (3.3V)

    -   pre iné napätia sa môžeme riadiť tabuľkou 5-1 na strane 17

          |$V_{OUT}$|   K   |   $L_{STANDART}$|
          |-----------|------|----------------|
          |2.0V       | 0.20 |  10 µH|
          |3.3V       | 0.22 |  15 µH|
          |5.0V       | 0.23 |  22 µH|
          |12V        | 0.21 |  56 µH|
          |15V        | 0.22 |  68 µH|

-   ostatné externé komponenty aj ich charakteristiky vieme nájsť v
    dátovom liste na stranách 19 až 20

-   v pôvodnom návrhu z projektu Deadlock, bola v schéme umiestnená
    dióda BAT54, nakoľko si nie sme istý dôvodom umiestnenia tejto
    diódy, je pri nej v schéme otáznik

## Komunikačné rozhranie 

![Schéma zapojenia komunikačného
rozhrania](rs-232_schematic.png)
*Schéma zapojenia komunikačného rozhrania*

Komunikačné rozhranie sa skladá z dvoch komponentov:

-   RJ12 konektor 6p6c:

    -   slúži na komunikáciu s externou súčiastkou, ktorá môže byť
        pripojená prostredníctvom kábla s RJ12 konektormi

    -   slúži zároveň aj ako napájanie celého zariadenia napätím medzi
        4V až 15V

    -   funkcie jednotlivých pinov konektora sú v tejto tabuľke:

          |Pin |  Funkcia|
          |-----|-----------|
          |1    |  RST|
          |2    | Napájanie|
          |3     |Receive|
          |4     |Transmit|
          |5     |Zem|
          |6     |BOOT0|

    -   pin1 budeme používať pre RST signál

    -   pin2 budeme používať pre napájanie

    -   pin3 budeme používať na prijímanie signálu do nášho MCU, zatiaľ
        čo na druhej strane kábla bude použitý na vysielanie

    -   pin4 budeme používať na odosielanie signálu z nášho MCU, zatiaľ
        čo na druhej strane kábla bude použitý na prijímanie

    -   pin5 pripojíme na zem

    -   pin6 budeme používať pre BOOT0 signál

-   MAX3232

    -   schému zapojenia používame z [dátového listu](https://github.com/ostertag/UACS/blob/hardwear_kozuch/rs-232_interface/Data_sheet.pdf)
        zo strany 12

        ![Schéma z dátového listu na strane 12](org_sch.png)
        *Schéma z dátového listu na strane 12*

    -   vrámci RS-232 štandardu sú napätia pre logickú jednotku a nulu
        definované takto:

      | Logická hodnota|     Napätie|
      |----------------- |------------|
       |      0        |  +3 až +15 V |
      |        1       |   -15 až -3 V |

    -   pri prijímaní signálu MAX 3232 konvertuje vstupné napätie na
        napätie pre logickú nulu (0V až 0.8V) alebo jednotku(2V až 3.3V)

    -   pri odosielaní signálu konvertuje napätie logickej nuly alebo
        jednotky na napätie +5V až +5.4V alebo -5V až -5.4V

    -   pre podrobnejšie informácie treba pozrieť tabuľku na strane na
        strane 2 až 3 (Electrical Characteristics)

    -   funkcie jednotlivých pinov sú v tejto tabuľke, nájdeme ju na
        strane 6 v dátovom liste:

      |Pin     | Meno       |Funkcia|
      |--------|----------|------------------------------------------------------------|
      |1         |C1+        |Positive Terminal of Voltage-Doubler Charge-Pump Capacitor|
      |2        |V+         |+5.5V Generated by the Charge Pump|
      |3        |C1-        |Negative Terminal of Voltage-Doubler Charge-Pump Capacitor|
      |4        |C2+        |Positive Terminal of Inverting Charge-Pump Capacitor|
      |5        |C2-        |Negative Terminal of Inverting Charge-Pump Capacitor|
      |6        |V-         |-5.5V Generated by the Charge Pump|
      |7,14     |T_OUT      |RS-232 Transmitter Outputs|
      |8,13     |R_IN       |RS-232 Receiver Inputs|
      |9,12     |R_OUT      |TTL/CMOS Receiver Outputs|
      |10, 11   |T_IN       |TTL/CMOS Transmitter Inputs|
      |15       |GND        |Ground|
      |16       |$V_{CC}$  | +3.0V to +5.5V Supply Voltage|

    -   piny 1 až 6 sú súčasťou takzvanej "Charge pump"

        -   slúži na zvýšenie napätia, aj keď vstupné napätie je nižšie

        -   v našom prípade zvyšuje zo vstupného napätia 3.3V na 5.5V
            alebo -5.5V

    -   piny 7 a 14 posielajú výstupný signál o hodnote medzi -3V až -15
        V alebo 3V až 15V

    -   piny 8 a 13 prijímajú vstupný signál o hodnote medzi -3V až -15
        V alebo 3V až 15V

    -   piny 9 a 12 posielajú výstupný signál ktorý je buď logická nula
        alebo jednotka

    -   piny 10 a 11 prijímajú vstupný signál ktorý je buď logická nula
        alebo jednotka

    -   pin 15 je uzemnený

    -   pin 16 je určený pre napájanie súčiastky

    -   v našej implementácii:

        -   pre posielanie signálov používame dvojicu pinov 11 a 14

        -   pre prijímanie signálov používame dvojicu pinov 12 a 13

    -   jediné externé súčiastky sú kondenzátory, ich minimálne hodnoty
        pre dané napätie sú v tabuľke 2 na strane 9:

          |$V_{CC}$ (V) |  C1 (µF) |  C2, C3, C4 (µF)|
          |-------------- |--------- |-----------------|
          |to 3.6        | 0.1       |0.1 |
          |4.5 to 5.5    | 0.047    | 0.33 |
          |3.0 to 5.5    | 0.1     |  0.47 |

    -   vieme použiť vyššie hodnoty kondenzátorov, zvlášť ak
        kondenzátory ktoré používame menia svoju hodnotu s rastúcou
        teplotou

    -   ak chceme zvýšiť hodnotu kondenzátora C1 musíme zvýšiť aj
        hodnoty ostatných kondenzátorov

    -   pri zvyšovaní hodnôt kondenzátorov C2 , C3 a C4 nemusíme
        zvyšovať hodnotu kondenzátora C1

    -   kondenzátor C5 používame v prípade ak je naša implementácia
        citlivá na "hluk" zdroja

    -   kondenzátor C5 bude mať rovnakú hodnotu ako kondenzátor C1

    -   pre viacej detailov ohľadom kondenzátorov pozri strany 8, 9
        (Capacitor Selection, Power-Supply Decoupling) v dátovom liste

    -   piny ktoré nebudú na nič pripojené, pripojíme na zem, prípadne
        na $V_{cc}$ (pozri stranu 7 - RS-232 Transmitters v dátovom
        liste)

## STM32

![Schéma zapojenia STM32F103C8T6](stm32_schematic.png)

Schému zapojenia nášho microcontrolera vieme rozdeliť na 4 časti:

### 1. Decoupling
Decouplig používame pri pinoch VBAT, VDD a VDDA aby sme  zlepšili stabilitu napájacieho napätia a znížili elektrický šum.   
Pre decoupling pri pinoch VBAT a VDD používame štyri sériovo zapojéne 100 nF kondenzátory, za každý VBAT a VDD pin jeden, a jeden 4.7 μF kondenzátor.  Avšak mi použijeme 10 μF kondenzátor, kvôli dostupnosti súčiastok nech môžeme nakupovať viac súčiastok z jedného druhu. 10 μF sa bežne vyskytuje v rôznych schémach s naším modelom STM32, napríklad aj v tom [videu](https://youtu.be/aVUqaB0IMh4?si=yPWBV3e04wY7MyOk&t=697). 
Pre decoupling pri pine VDDA používame seriovo zapojený 10nF a 1uF kondenzátor. Pre pripojenie k 3.3 voltom pridáme 100 ohmový Ferite Bead a 1uF kondenzátor pre pripojenie k zemi.  
Pri týchto hodnotách pre komponenty vychádzame zo schémy 14 na strane 36 z [datasheetu](data_sheet.pdf) a aj z hore zmieneného videa.

### 2. Externý oscilátor
Náš STM32 mikrokontolér má síce vnútorný oscilátor ale aby sme mohli používať pripojenie cez USB tak potrebujeme pripojiť externý oscilátor. 
K oscilátoru budeme musieť pripojiť dva 10pF kondenzátory. Tieto hodnoty výchádzajú z [AN2867](an2867.pdf) z rovnice:

$$C_{L} = \frac{C_{L1} * C_{L2}}{C_{L1}+C_{L2}}+ C_{S} $$

Kde:
- $C_L$ je záťažová kapacita oscilátora, u nás to je 10 pF, kedže chceme použiť tento 
[kondezátor](https://jlcpcb.com/parts/componentSearch?searchTxt=16mHZ,%2010pF)  
- $C_S$ je rozptylová kapacita dosky plošných spojov, zvyčajne je medzi 2 až 7 pF, my použijeme hodnotu 5 pF 
- $C_{L1}$ a $C_{L2}$ sú hodnoty kondenzátorov a teda   
$$C_{L1} = C_{L2} = 10 pF  $$




### 3. Prepínače pri pinoch NRST a BOOT0
Chceme aby sme vedeli náš mikrokontolér resetovať manuálne ale aj pomocou signálov. Preto tieto piny pripojíme k prepínačom 
ale aj k výstupom z RJ12 konektoru a pri NRST pine aj zo SERIAL WIRE DEBUG konektora.
Prepínanie funguje takto:
- Ak je BOOT0 pin stiahnutý na zem tak bootloader skontroluje či je flash pamäť prázdna
  - ak je prázdna tak začne komunikovať s rozhraniami (UART/SWD/USB) aby z nich stiahol program
  - ak nie je práznda, spustí sa kód ktorý je nahratý vo flash pamäti
- Ak na BOOT0 pin privedieme 3.3 volta tak bootloader vyhodnotí že flash pamäť je prázdna
- NRST pin je defaultne na 3.3 volta, vďaka internému pull up resistoru.
- Ak je NRST stiahnutý na zem tak STM32 sa resetne 
- Preto ak RST linku stiahneme na zem, prestaneme na ňu privádzať 3.3V, vieme STM32 resetnut 

Kondenzátor pri pine NRST slúži ako ochrana proti náhodným resetom. Minimálna hodnota kondenzátora by mala byť 0.1μF


### 4. Serial wire debug 
Pre debugovanie budeme používať 10 pinový SWD. Zapojenie je podľa tejto 
schémy:     
![swd_schematic](swd_schematic.png)
 




## Zosilňovač audia pomocou LM386

Tento zosilňovač je využitý v schéme zosilňovača v [projekte Deadlock](https://gitlab.com/project-deadlock). 
Tu vysvetlíme výber komponentov, ich funkčnosť a ako voliť hodnoty.

### Charakteristika

Zosilňovač LM386 má minimálny počet externých častí, široký rozsah napätia (4 - 12V) a malý odber prúdu v stave nečinnosti. Poskytuje 20 až 200-násobné zosilnenie. Predvolené nastavenie zosilnenia je 20-násobné, ale pridaním rezistora alebo kondenzátora medzi piny 1 a 8 vieme túto hodnotu zmeniť na hociktorú z rozsahu.

### Konfigurácia pinov LM386

| **Názov** | **Číslo pinu** | **Typ** | **Popis**                                               |
| --------- | :------------: | :-----: | ------------------------------------------------------- |
| Gain      |       1        |    -    | pin na nastavenie zosilnenia                            |
| - Input   |       2        |    I    | invertujúci vstup                                       |
| + Input   |       3        |    I    | neinvertujúci vstup                                     |
| GND       |       4        |    P    | referencia na zem                                       |
| $V_{out}$ |       5        |    O    | výstup                                                  |
| $V_S$     |       6        |    P    | napätie zdroja                                          |
| Bypass    |       7        |    O    | bypass decoupling path                                  |
| Gain      |       8        |    -    | pin na nastavenie zosilnenia                            |

Pri type:

- I - Vstup
- O - Výstup
- P - Napájanie

### Odporúčané operačné podmienky LM386

- Napätie zdroja: $4$ až $12V$
- Impedancia reproduktora: $4$ až $32\Omega$
- Vstupné napätie: $-0.4$ až $0.4V$
- Teplota: $0$ až $70$$^\circ C$ 

### Schéma

![Schéma 9.2.1 z dátového listu](schematic_lm386_orig.png)

Využívame schému 9.2.1 z [dátového listu](lm386_datasheet.pdf) , ktorú sme si mierne prispôsobili pridaním rezistora $R_1$, ktorý spolu s potenciometrom $R_2$ tvorí napäťový delič a zaručuje, že vstupné napätie bude v odporúčanom rozsahu (maximálne 0.4V pri vstupnom napätí 3.3V).

![Upravená schéma](schematic.png)



### Komponenty a ich hodnoty

#### Rezistor $R_1$ a potenciometer $R_2$

Potenciometer $R_2$ bude slúžiť hlavne na nastavenie hlasitosti a použijeme hodnotu $10k\Omega$, ale jeho hodnota nie je veľmi dôležitá, kým je dostatočne vysoká. Hodnoty ako $22k\Omega$ alebo $4.7k\Omega$ by fungovali tiež.
Ako už je vyššie spomenuté, tieto komponenty tvoria napäťový delič a ich hodnoty sú zvolené podľa [vzorca pre napäťový delič](https://www.digikey.lv/en/resources/conversion-calculators/conversion-calculator-voltage-divider) za predpokladu, že $V_{AUDIO\_OUT} = 3.3V$  

$$V_{pin3} = \frac{V_{AUDIO\_OUT} R_2}{(R_1 + R_2)}$$ 

ak chceme dostať hodnotu rezistora $R_1$, tak si vieme vzorec upraviť nasledovne:

$$R_1 = \frac{V_{AUDIO\_OUT} R_2}{V_{pin3}} + R_2$$ 

kde $V_{AUDIO\_OUT}$ je vstupný signál a $V_{pin3}$ je napätie, ktoré nám potečie na pin 3, ktoré už je v odporúčanom rozsahu z dátového listu. Po dosadení hodnôt dostaneme $R_1 = 100k\Omega$.

môžeme overiť, či sme vybrali správnu hodnotu rezistoru dosadením do vzorca a overením, či $V_{pin3}$ je v norme:

$$V_{pin3} = \frac{3.3 \times 10 \times 10^{3} }{(100 \times 10^{3} + 10 \times 10^{3})} = \frac{3}{10}V$$ 

Pre naše využitie to teda bude $R_1 = 100k\Omega$ a $R_2 = 10k\Omega$.

#### Integrovaný obvod $IC_1$

Toto je náš zosilňovač, ktorý zaručí funkčnosť reproduktoru.
Zosilnenie sa dá nastaviť nasledovne:

1. Predvolené (minimálne) zosilnenie- 20x - nič nie je pripojené medzi piny 1 a 8
2. Maximálne zosilnenie - 200x - medzi pinmi 1 a 8 pripojíme kondenzátor
3. Vlastné zosilnenie - medzi piny 1 a 8 pripojíme kondenzátor a rezistory

Na výpočet zosilnenia  môžeme využiť nasledovný [vzorec](https://www.electrosmash.com/lm386-analysis#lm386input):

 $$Gain = 2 \frac{Z_{1-5}}{150 + Z_{1-8}}$$

kde $Z_{1-5}$ je impedancia medzi pinmi 1 a 5 a $Z_{1-8}$ je impedancia medzi pinmi 1 a 8.

Pri vyššom zosilnení je potrebné na pin 7 pripojiť bypass kondenzátor - zvyčajne $0.1\mu F$, prípadne tento pin uzemniť.
Pin 4 zapojíme na zem, rovnako ako aj invertujúci pin 2.
Z pinu 5 nám pôjde výstup - zosilnený signál.

#### Kondenzátor $C_4$ a rezistor $R_5$

Spolu tvoria Boucherot cell - elektronický filter, ktorý tlmí vysokofrekvenčné oscilácie, ktoré sa môžu vyskytnúť pri vyšších frekvenciách. Je potrebný, pretože chceme zabezpečiť, aby mal zosilňovač ilúziu rovnakej impedancie zo strany reproduktora pri všetkých frekvenciách. 

![Boucherot cell](boucherot.png)

Na obrázku môžeme vidieť, že reproduktor je zložený z cievky a rezistora. Čím sa cievka viac zahrieva, tým je menej priepustná a obvod sa stáva menej stabilný. Ako protiváhu použijeme  kondenzátor C<sub>4</sub> a rezistor R<sub>5</sub>, ktorých hodnoty sú zvolené podľa obrázka. Keďže náš reproduktor má impedanciu $8\Omega$, tak hodnota $R_5$ by mala byť tiež $8\Omega$, ale kvôli dostupnosti a cene súčiastok si postačíme s hodnotou $10\Omega$.
Kondenzátor by mal mať zvolenú hodnotu podľa vzorca na obrázku: 

$$C = \frac{L}{R^2}$$

kde: 

- $L$ je indukčnosť reproduktoru $SP_1$
- $R$ je impedancia reproduktoru  $SP_1$
- $C$ je hodnota kondenzátoru $C_4$

keďže indukčnosť nášho reproduktoru nie je v [dátovom liste](K15S-8_datasheet.pdf) spomenutá tak použijeme hodnotu  1mH, ktorá je bežná pre reproduktory v tejto kategórii. Po dosadení dostaneme $C_4 = 100nF$.

Táto dvojica komponentov zároveň odstraňuje šum a praskanie a robí celý obvod viac stabilný.
Ak by sme ešte chceli zvýšiť basy nášho reproduktoru, tak vieme hodnotu $C_5$ mierne zvýšiť.
Pre naše využitie to bude $R_5 = 10\Omega$ a $C_4 = 100nF$.

#### Kondenzátor $C_5$

Je to takzvaný coupling kondenzátor, ktorý funguje ako high pass filter, ktorý narozdiel od low pass filtra vyfiltruje frekvencie pod počuteľné spektrum, teda menej ako 20Hz a zároveň odstráni DC zložku napätia zo signálu. Hodnotu $C_5$ chceme voliť vyššiu, rádovo v stovkách až tisíckach  $\mu F$. Pre naše potreby bude stačiť  $C_5 = 100\mu F$. Využijeme vzorec pre high pass filter:
$$f_c = \frac{1}{2\pi RC}$$ 
kde: 

- $f_c$ je frekvencia, od ktorej nižšia frekvencia už bude vyfiltrovaná
- $R$ je hodnota reproduktoru  $SP_1$
- $C$ je hodnota kondenzátoru $C_5$

Ak poznáme impedanciu reproduktoru a frekvenciu, ktorú chceme filtrovať, tak vieme použiť odvodený vzorec: 

$$C = \frac{1}{2\pi Rf_c}$$ 

Je dobré spomenúť, že ak reproduktor nie je schopný nejakú frekvenciu prehrať, tak ak máme nastavený high pass filter nižšie, tak ju reproduktor aj tak neprehrá - stačí hodnotu nastaviť podľa [dátového listu reproduktora](K15S-8_datasheet.pdf) - u nás to je približne 200Hz, takže bude stačiť hodnota  $C_5 = 100\mu F$.

#### Kondenzátor $C_1$

V [dátovom liste LM386](lm386_datasheet.pdf) na strane 17 - časť 10 sa píše, že sa odporúča použiť uzemnený kondenzátor v blízkosti pinu 3.

#### Reproduktor $SP_1$

Na LM386 vieme pripojiť hociktorý reproduktor s impedanciou $4 - 32\Omega$, my sme si vybrali s hodnotou $8\Omega$. Ako je vyššie spomenuté a viditeľné v grafe nižšie, reproduktor nedokáže prehrať frekvenciu nižšiu ako približne 200Hz.

![Frekvenčná charakteristika reproduktora](frek_charakteristika.png)

## LED KAA-3528SRSGS

Tieto LED diódy sú využité v schéme readera v [projekte Deadlock](https://gitlab.com/project-deadlock). 

### Schéma zapojenia

Používame odporúčanú schému zapojenia z [dátového listu](KAA-3528SRSGS_datasheet_1.pdf) na strane 6

![Schéma](schematic1.png)

### Komponenty v obvode

#### LED diódy $LED_1$ a $LED_2$ 

Skladajú sa z 2 diód - zelenej a červenej farby, ktoré sa dajú nezávisle na sebe ovládať.

| **Parameter**   | **Červená** | **Zelená** | **Jednotka** |
|-----------------|-------------|------------|--------------|
| Kapacita        | 45          | 15         | pF           |
| Forward Voltage | 1.85        | 2.2        | V            |
| Max. forward Current | 30          | 25         | mA           |

V tabuľke vyššie vidíme pre nás dôležité parametre z tabuliek v [dátovom liste](KAA-3528SRSGS_datasheet_2.pdf) na strane 3.

#### Rezistory

Vybrali sme si rezistory s hodnotou $330\Omega$, aby redukovali prúd ktorý tečie cez diódy, aby sa nezničili. Za predpokladu že napätie $V_{in}$, ktoré tečie do každého rezistoru je $5V$, tak vieme vypočítať prúd podľa [tohto vzorca](https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-led-series-resistor): 

$$I = \frac{V_{in} - V_{fw}}{R}$$

kde: 
- $V_{in}$ je vstupné napätie
  
## USB interface

Tento USB interface je využitý v schéme readera v [projekte Deadlock](https://gitlab.com/project-deadlock). 

###  Schéma zapojenia

![Schéma](schematic2.png "Schéma")

Schéma je z [aplikačnej poznámky](an1.pdf) na strane 19, Figure 10.

![Figure 10](usb_orig.png "Figure 10")

### Rozdelenie pinov

Tieto piny využívame v našej schéme, ostatné sa nachádzajú v [tabuľke 2](an1.pdf)  na strane 6 a 7.

| **Pin** | **Názov**       | **Popis**                                                                |
|---------|-----------------|--------------------------------------------------------------------------|
| A4      | VBUS            | Bus power                                                                |
| A5, B5  | CC1 alebo VCONN | Configuration channel or power for active or electronically marked cable |
| A6, B6  | D+              | USB2.0 Data line                                                         |
| A7, B7  | D-              | USB2.0 Data line                                                         |
| A1      | GND             | Ground return                                                            |

###  Komponenty a ich hodnoty

#### Rezistory $R_{13}$ a $R_{14}$
Tieto rezistory nie sú potrebné, ale slúžia ako ukončovacie rezistory, ktoré zabraňujú odrážaniu signálu a tým zvyšujú jeho kvalitu.

#### Rezistor $R_{15}$
Kvôli splneniu USB2.0 full-speed electrical špecifikácie, D+ pin musí byť pripojený s $1.5k\Omega$ pull up rezistorom na napätie v rozsahu 3.0 až 3.6V. Tento rezistor je niekedy zabudovaný na D+ linke, dá sa to overiť podľa [tabuľky 4](an2.pdf) na strane 3 a 4 v poslednom stĺpci.

#### Rezistory $R_{16}$ a $R_{17}$
Podľa [tabuľky 6](an1.pdf) na strane 10 potrebujeme pripojiť na piny CC1 aj CC2 $5.1k\Omega$ pull-down rezistory

# Kapitola 3 Dizajn a implementácia firmwearu pre reader

# Kapitola 4 Komunikačná knižnica pre operačné systémy založené na Linuxe


# Referencie
[^1]: Součková, K. 2017. Design and implementation of an rfid access control system. (2017).

[^2]: Dej, A. 2018. Design and implementation of embedded components of an RFID access control system. (2018).


