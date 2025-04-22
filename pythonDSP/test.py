for i in range(1,200000000):
    formul = (i * i) / (i + i) 
    hizliyol = i / 2
    print(i)
    if formul != hizliyol:
        print(i, " icin; ",formul," != ",hizliyol)
        break