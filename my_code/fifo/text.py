count = 0

with open("1.txt", "w") as f:
    while True:
        f.write(str(count))
        count = count + 1
        if count == 1000:
            break

