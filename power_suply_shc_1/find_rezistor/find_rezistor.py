

cnt = 0
rezistors = dict()  # slovnik hodnota rezistora:seria
out = 0.8
min, max = 3.2, 3.4  # nastav rozsah vystupneho napatia
v_out = 3.3 # nase vystupne napatie ktore chceme dostat
with open("series.txt", "r")as file:     # subor so seriami
    for riadok in file:
        if cnt % 2 == 0:
            series = riadok.strip().split(" ")[0]
        else:
            tmp_list = list(map(float, riadok.strip().split(", ")))
            for item in tmp_list:
                if item not in rezistors.keys():   # kedze niektore serie su podmnozinami inych,
                                                    # toto zabezpeci aby kazdy rezistor mal len jednu
                    rezistors[item] = series
        cnt += 1
# print(rezistors.items())
l = []
with open("result.csv", "w")as output:     # do tohto suboru sa zapisuju vysledky
    output.write("R1;Serie1;R2;Serie2;Source;Delta\n")
    for r1 in rezistors.keys():
        for r2 in rezistors.keys():
            source = (out*(r1+r2))/r2    # tu je rovnica cez ktoru sa to pocita
            if min <= source <= max:
                delta = source-v_out
                l.append((r1, rezistors[r1], r2, rezistors[r2], source, delta))
    l.sort(key=lambda x: abs(x[5]))
    for item in l:
        output.write(f'{item[0]};{item[1]};{item[2]};{item[3]};{item[4]};{round(item[5], 15)}\n'.replace(".", ","))
        # replace funkcia je pouzita kvoli improtu do excelu - bodka menila cisla na datumy, v pripade chyb odstranit