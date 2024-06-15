# LED KAA-3528SRSGS
Tieto LED diódy sú využité v schéme readera v [projekte Deadlock](https://gitlab.com/project-deadlock). 

## Schéma zapojenia
Používame odporúčanú schému zapojenia z [datasheetu](KAA-3528SRSGS_datasheet_1.pdf) na strane 6

![Schéma](schematic.png "Schéma")

## Komponenty v obvode
### LED<sub>1</sub> a LED<sub>2</sub>
Skladajú sa z 2 diód - zelenej a červenej farby, ktoré sa dajú nezávisle na sebe ovládať.

| **Parameter**   | **Červená** | **Zelená** | **Jednotka** |
|-----------------|-------------|------------|--------------|
| Kapacita        | 45          | 15         | pF           |
| Forward Voltage | 1.85        | 2.2        | V            |
| Max. forward Current | 30          | 25         | mA           |

V tabuľke vyššie vidíme pre nás dôležité parametre.

### Rezistory
Vybrali sme si rezistory s hodnotou $330\Omega$, aby redukovali prúd ktorý tečie cez diódy, aby sa nezničili. Za predpokladu že napätie $V_{in}$, ktoré tečie do každého rezistoru je $5V$, tak vieme vypočítať prúd nasledovne: 

$$I = \frac{V_{in} - V_{fw}}{R}$$

kde: 
- $V_{in}$ je vstupné napätie
- $V_{fw}$ je forward voltage konkrétnej diódy
- $R$ je impedancia rezistora pred diódou

Po dosadení hodnôt dostaneme pre červenú diódu hodnotu $9mA$ a pre zelenú diódu hodnotu $8.5mA$, čo sú hodnoty nižšie ako max. forward current.