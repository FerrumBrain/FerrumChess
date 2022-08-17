f1 = open("opening_book.txt", "w")
for surname in ["Carlsen", "Caruana", "Ding", "Aronian", "Mamedyarov", "Nepomniachtchi", "Tal", "Kasparov", "Fischer", "Anand"]:
    f = open(surname + ".pgn", "r")
    flag = True
    for line in f.readlines():
        if flag and line[0] == '[':
            f1.write('\n')
            flag = False
        if not line[0].isdigit():
            continue
        flag = True
        line = line.split(' ')
        res = ""
        for i in range(len(line)):
            if len(line[i]) <= 1 or (line[i][0].isdigit() and (line[i][1] == '-' or line[i][1] == '/')):
                continue
            if i % 2 == 0:
                line[i] = line[i][1 + line[i].find('.'):]
            res += str(line[i]) + " "
        if res[-2] == '\n':
            res = res[:-2]
        res += ' '
        f1.write(res)
    f.close()
f1.close()
f = open("opening_book.txt", "r")
lines = sorted(f.readlines())
f.close()
f1 = open("opening_book.txt", "w")
for line in lines:
    f1.write(line)
f1.close()
