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
