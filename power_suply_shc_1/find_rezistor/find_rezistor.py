cnt = 0
rezistors = dict()
out = 0.8
with open("series.txt", "r")as file:
    for riadok in file:
        if cnt%2 == 0:
            serie = riadok.strip().split(" ")[0]
        else:
            tmp_list =list(map(float, riadok.strip().split(", ")))
            for item in tmp_list:
                if item not in rezistors.keys():
                    rezistors[item] = serie
        cnt += 1
print(rezistors.items())

with open("result.csv", "w")as output:
    output.write("R1;Serie1;R2;Serie2;Source\n")
    for r1 in rezistors.keys():
        for r2 in rezistors.keys():
            source = (out*(r1+r2))/r2
            if (2 <= source <= 3.4):
                output.write(f'{r1};{rezistors[r1]};{r2};{rezistors[r2]};{source}\n')