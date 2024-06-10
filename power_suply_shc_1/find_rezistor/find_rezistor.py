cnt = 0
rezistors = dict()  # slovnik hodnota rezistora:seria
out = 0.8
min, max = 3.3, 3.4  # nastav rozsah vystupneho napatia
with open("series.txt", "r")as file:     # subor so seriami
    for riadok in file:
        if cnt % 2 == 0:
            serie = riadok.strip().split(" ")[0]
        else:
            tmp_list = list(map(float, riadok.strip().split(", ")))
            for item in tmp_list:
                if item not in rezistors.keys():   # kedze niektore serie su podmnozinami inych,
                                                    # toto zabezpeci aby kazdy rezistor mal len jednu
                    rezistors[item] = serie
        cnt += 1
# print(rezistors.items())

with open("result.csv", "w")as output:     # do tohto suboru sa zapisuju vysledky
    output.write("R1;Serie1;R2;Serie2;Source\n")
    for r1 in rezistors.keys():
        for r2 in rezistors.keys():
            source = (out*(r1+r2))/r2    # tu je rovnica cez ktoru sa to pocita
            if min <= source <= max:
                output.write(f'{r1};{rezistors[r1]};{r2};{rezistors[r2]};{source}\n')
