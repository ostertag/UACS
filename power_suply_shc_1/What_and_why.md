Tu sú všetky informacie ohľadom komponentov a schémy [konvertora napätia](power_suply_schematic_1.kicad_sch).  
Schéma je použitá z [data sheetu sučiastky MCP16301Hx-xCH](Data_sheet.pdf), konkrétne Figure 6-1 na strane 23.   
Niektoré externé komponenty vieme upraviť, aby sme dostali iné výsledné napätie, prípadne iný prúd.    
Naše schéma s ktorou pracujeme predpokladá vstupné napätie 12V a výstupné napätie 3.3V.  
Externé komponenty sú:
- rezistory:
  - rezistory R<sub>1</sub> a R<sub>2</sub>:
    - v Data sheete sú to R<sub>TOP</sub> (R<sub>1</sub>) a R<sub>BOT</sub> (R<sub>2</sub>) 
    - od ich hodnôt závisy výstupné napätie
    - v data sheete mame rovnicu na vypocet odporov jednotlivých rezistorov:

      $$R_{1} = R_{2} \times \left(\frac{V_{OUT}}{V_{FB}} - 1\right)$$
    - naše V<sub>OUT</sub> je napätie ktoré chceme dostať na výstupe a V<sub>FB</sub> je napätie idúce do Feedback pinu a musí mať hodnotu 0.8V
    - na nájdenie hodnôt rezistorov môžeme pouziť script [find_rezistor.py](find_rezistor/find_rezistor.py) ([viď jeho readme.md](find_rezistor/README.md))
    - snažíme sa hladať rezistory z čo najnižších sérii
    - v data sheete uvadzajú že R<sub>2</sub> by mal mať 10 kΩ a R<sub>1</sub> by mal mať 31.6 kΩ
  
- kondenzátory:
  - kondenzátor C1:
    - v data sheete je to aj C<sub>IN</sub>
    - v data sheete je uvedené že treba použiť dva paraelne zapojené kondenzátory, každý o hodnote 4.7 µF
    - používame 10 µF kondenzátor aby sme znížili množstvo komponentov a náročnosť zapojenia
  - kondezátori C3 a C4:
    - v data sheete je to aj C<sub>OUT</sub>
    - ich hodnota zavisý od výsledneho napätia
    - pre naše výstupne napätie používame dva kapacitatory každý o hodnote 10 µF, ktoré budú paraelne zapojene

- cievka:
  - používame 15 µH cievku
  - podľa výstupného napätia upravujeme jej hodnotu podľa tejto rovnice:

    $$K = V_{OUT}/L$$
  - kde K by malo mať hodnotu 0.22 V/µH a V<sub>OUT</sub> je naše výstupne napätie (3.3V)
- ostatne externe komponenty aj ich charakteristiky vieme najst v Data sheete
    
V povodnom navrhu z ktoreho tento vychadza mal medzi vystupom a vstupom BAT54 diodu
