Tu budu vsetky info ohladom komponentov a schemy power suply
Schema je pouzita z data sheetu suciastky MCP16301Hx-xCH. Niektore komponenty vieme upravit, aby sme dostali ine vysledne napatie, pripadne iny prud
- rezistory:
  - rezistory R1 a R2:
    - od ich hodnot zavisy vystupne napatie
    - v data sheete mame rovnicu na vypocet potrebnych parametrov:

      $$R_{1} = R_{2} \times \left(\frac{V_{OUT}}{V_{FB}} - 1\right)$$
    - nase V<sub>OUT</sub> je napatie ktore chceme dostat na vystupe a V<sub>FB</sub> je napatie iduce z Feedback pinu a ma hodnotu 0.8V
    - na najdenie hodnot rezistorov mozeme pouzit script find_rezistor.py (vid jeho readme.md)
  
- capacitatory:
  - 
- cievka:
