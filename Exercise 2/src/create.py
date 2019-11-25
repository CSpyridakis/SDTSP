import sys
import random
import products_afms

try:
    import tqdm
    IMPO = True
except Exception:
    print("You may want to install tqdm package in order to see status bar!")
    IMPO = False

PRODUCTS = products_afms.PRODUCTS
AFMS = products_afms.AFMS

# Number of receipts to create
receipts = sys.argv[1]

if IMPO:
    for i in tqdm.tqdm(range(1, int(receipts)+1)):
        afm = AFMS[random.randrange(0, len(AFMS))]
        line = "-"*random.randrange(1, 40) + '\n' + ' '*random.randrange(0, 10) + 'ΑΦΜ' + ' '*random.randrange(0, 10) + ':' + ' '*random.randrange(0, 10) + str(afm)
        print(line)
        total = 0.0

        max_products = random.randrange(1, 60)
        for j in range(1, int(max_products)+1):
            prod = PRODUCTS[random.randrange(0, len(PRODUCTS))]
            quantity = random.randrange(1, 20)
            price = round(random.uniform(0.20, 15.99),2)
            sum = round(quantity*price,2)
            total = round(total+sum,2)
            line = ' '*random.randrange(0, 10) + str(prod) + ' '*random.randrange(0, 10) + ':' + ' '*random.randrange(0, 10) + str(quantity) + ' '*random.randrange(1, 10) + str(price) + ' '*random.randrange(1, 10) + str(sum) + ' '*random.randrange(0, 10)
            print(str(line))

        line = ' '*random.randrange(0, 10) + 'ΣΥΝΟΛΟ' + ' '*random.randrange(0, 10) + ':' + ' '*random.randrange(0, 10) + str(total)
        print(line)
    print('-'*random.randrange(1, 40))
else:
    for i in range(1, int(receipts)+1):
        afm = AFMS[random.randrange(0, len(AFMS))]
        line = "-"*random.randrange(1, 40) + '\n' + ' '*random.randrange(0, 10) + 'ΑΦΜ' + ' '*random.randrange(0, 10) + ':' + ' '*random.randrange(0, 10) + str(afm)
        print(line)
        total = 0.0

        max_products = random.randrange(1, 60)
        for j in range(1, int(max_products)+1):
            prod = PRODUCTS[random.randrange(0, len(PRODUCTS))]
            quantity = random.randrange(1, 20)
            price = round(random.uniform(0.20, 15.99),2)
            sum = round(quantity*price,2)
            total = round(total+sum,2)
            line = ' '*random.randrange(0, 10) + str(prod) + ' '*random.randrange(0, 10) + ':' + ' '*random.randrange(0, 10) + str(quantity) + ' '*random.randrange(1, 10) + str(price) + ' '*random.randrange(1, 10) + str(sum) + ' '*random.randrange(0, 10)
            print(str(line))

        line = ' '*random.randrange(0, 10) + 'ΣΥΝΟΛΟ' + ' '*random.randrange(0, 10) + ':' + ' '*random.randrange(0, 10) + str(total)
        print(line)
    print('-'*random.randrange(1, 40))