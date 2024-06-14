# Zosilňovač audia pomocou LM386
Tento zosilňovač je využitý v schéme zosilňovača v [projekte Deadlock](https://gitlab.com/project-deadlock). 
Tu vysvetlíme výber komponentov, ich funkčnosť a ako voliť hodnoty.

## Charakteristika
Zosilňovač LM386 má minimálny počet externých častí, široký rozsah napätia (4 - 12V) a malý odber prúdu v stave nečinnosti. Poskytuje 20 až 200-násobné zosilnenie. Predvolené nastavenie zosilnenia je 20-násobné, ale pridaním rezistora alebo kondenzátora medzi piny 1 a 8 vieme túto hodnotu zmeniť na hociktorú z rozsahu.

## Konfigurácia pinov LM386
| **Názov** | **Číslo pinu** | **Typ** | **Popis**               |
|-----------|-----------|---------|------------------------------|
| Gain      | 1         | -       | pin na nastavenie zosilnenia |
| - Input   | 2         | I       | invertujúci vstup            |
| + Input   | 3         | I       | neinvertujúci vstup          |
| GND       | 4         | P       | referencia na zem            |
| $V_{out}$ | 5         | O       | výstup                       |
| $V_S$     | 6         | P       | napätie zdroja               |
| Bypass    | 7         | O       | odďeľovacia cesta            |
| Gain      | 8         | -       | pin na nastavenie zosilnenia |

Pri type: 
- I - Vstup
- O - Výstup
- P - Napájanie

## Odporúčané operačné podmienky LM386
- Napätie zdroja: $4 - 12V$
- Impedancia reproduktora: $4 - 32\Omega$
- Vstupné napätie: $-0.4$ - $0.4V$
- Teplota: $0 - 70\degree C$

## Schéma
Využívame schému 9.2.1 z [datasheetu](lm386_datasheet.pdf) , ktorú sme si mierne prispôsobili pridaním rezistora **R<sub>1</sub>**, ktorý spolu s potenciometrom **R<sub>2</sub>** tvorí napäťový delič a zaručuje, že vstupné napätie nebude príliš vysoké.

![Upravená schéma](schematic.png "Upravená schéma")



## Komponenty a ich hodnoty
### Rezistor R<sub>1</sub> a potenciometer R<sub>2</sub>
Potenciometer R<sub>2</sub> bude slúžiť hlavne na nastavenie hlasitosti.
Ako už je vyššie spomenuté, tieto komponenty tvoria napäťový delič a ich hodnoty sú zvolené podľa vzorca $$V_{out} = \frac{V_1 R_2}{(R_1 + R_2)}$$ kde $V_1$ je vstupné napätie pre náš zosilňovač, ktoré privedieme na pin 3 - neinvertujúci vstup.
Pre naše využitie to bude $R_1 = 100k\Omega$ a $R_2 = 10k\Omega$.

### Integrovaný obvod IC<sub>1</sub>
Toto je náš zosilňovač, ktorý zaručí funkčnosť reproduktoru.
Zosilnenie sa dá nastaviť nasledovne:
1. Predvolené (minimálne) zosilnenie- 20x - nič nie je pripojené medzi piny 1 a 8
2. Maximálne zosilnenie - 200x - medzi pinmi 1 a 8 pripojíme kondenzátor
3. Vlastné zosilnenie - medzi piny 1 a 8 pripojíme kondenzátor a rezistory

Na výpočet zosilnenia pri vlastnej hodnote môžeme využiť nasledovný vzorec $$Gain = 2 \frac{Z_{1-5}}{150 + Z_{1-8}}$$

kde $Z_{1-5}$ je impedancia medzi pinmi 1 a 5 a $Z_{1-8}$ je impedancia medzi pinmi 1 a 8.

Pri vyššom zosilnení je potrebné na pin 7 pripojiť bypass kondenzátor - zvyčajne $0.1\mu F$, alebo tento pin uzemniť.
Pin 4 zapojíme na zem, rovnako ako aj invertujúci pin 2.
Z pinu 5 nám pôjde výstup - zosilnený signál.

### Kondenzátor C<sub>4</sub> a rezistor R<sub>5</sub>
Spolu tvoria RC - low pass filter, ktorý vyfiltruje frekvencie nad počuteľné spektrum, teda viac ako 20kHz, rádovo v MHz. Pre tento účel potrebujeme zvoliť nízku hodnotu pre C<sub>4</sub>, napríklad ako v našom obvode - 100nF. Rezistor R<sub>5</sub> chceme tiež zvoliť radovo v desiatkach ohmov. Presnú hodnotu vieme vypočítať pomocou nasledujúceho vzorca: $$f_c = \frac{1}{2\pi RC}$$ kde: 
- $f_c$ je frekvencia, od ktorej vyššia frekvencia už bude vyfiltrovaná
- $R$ je hodnota rezistoru  $R_5$
- $C$ je hodnota kondenzátoru $C_4$

Táto dvojica komponentov zároveň odstraňuje šum a praskanie a robí celý obvod viac stabilný.
Ak by sme ešte chceli zvýšiť basy nášho reproduktoru, tak vieme hodnotu $C_5$ mierne zvýšiť.
Pre naše využitie to bude $R_5 = 10\Omega$ a $C_4 = 10nF$.

### Kondenzátor C<sub>5</sub>
Je to takzvaný coupling kondenzátor, ktorý funguje ako high pass filter, ktorý narozdiel od low pass filtra filtruje vyfiltruje frekvencie pod počuteľné spektrum, teda menej ako 20Hz. Hodnotu $C_5$ chceme voliť vyššiu, rádovo v stovkách až tisíckach  $\mu F$. Pre naše potreby bude stačiť  $C_5 = 100\mu F$. Rovnako ako pri C<sub>4</sub> a R<sub>5</sub>, presnú hodnotu vieme vypočítať cez vzorec: $$f_c = \frac{1}{2\pi RC}$$ 
kde: 
- $f_c$ je frekvencia, od ktorej nižšia frekvencia už bude vyfiltrovaná
- $R$ je hodnota reproduktoru  $SP_1$
- $C$ je hodnota kondenzátoru $C_5$

### Reproduktor SP<sub>1</sub>
Na LM386 vieme pripojiť hociktorý reporduktor s impedanciou $4 - 32\Omega$, my sme si vybrali s hodnotou $8\Omega$.