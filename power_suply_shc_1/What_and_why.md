Tu sú všetky informacie ohľadom komponentov a schemy [konvertora napätia](https://github.com/cheetahzlesa/UACS/blob/hardwer_kozuch/power_suply_shc_1/power_suply_schematic_1.kicad_sch).  
Schema je pouzita z data sheetu suciastky MCP16301Hx-xCH. Niektore komponenty vieme upravit, aby sme dostali ine vysledne napatie, pripadne iny prud
Nase napatie ktore potrebujem dodat do nasho obvodu je 3.3V a vstupne napatie je 12V
- rezistory:
  - rezistory R1 a R2:
    - od ich hodnot zavisy vystupne napatie
    - v data sheete mame rovnicu na vypocet potrebnych parametrov:

      $$R_{1} = R_{2} \times \left(\frac{V_{OUT}}{V_{FB}} - 1\right)$$
    - nase V<sub>OUT</sub> je napatie ktore chceme dostat na vystupe a V<sub>FB</sub> je napatie iduce z Feedback pinu a ma hodnotu 0.8V
    - na najdenie hodnot rezistorov mozeme pouzit script find_rezistor.py (vid jeho readme.md)
    - snazime sa hladat rezistory z co najnizsich serii
    - v data sheete uvadzaju ze R<sub>2</sub> by mal mat 10 kΩ
  
- capacitatory:
  - capacitator C1:
    - v data sheete je to aj C<sub>IN</sub>
    - pouzivame 10 µF capacitator
  - capacitatori C3 a C4:
    - v data sheete je to aj C<sub>OUT</sub>
    - jeho hodnota zavisy od vysledneho napatia a prudu
    - pre nase vystupne napatie pouzivame 2x10 µF capacitatory paraelne zapojene

- cievka:
  - pouzivame 15 µH cievku
  - podla vystupneho napatia upravujeme jej hodnotu podla tejto rovnice:

    $$K = V_{OUT}/L$$
  - kde K by malo mat hodnotu 0.22 V/µH a V<sub>OUT</sub> je nase vystupne napatie (3.3V)
- ostatne externe komponenty aj ich charakteristiky vieme najst v Data sheete
    
V povodnom navrhu z ktoreho tento vychadza mal medzi vystupom a vstupom BAT54 diodu
