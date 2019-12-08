'''
_______________________________________________________________________________________________________

    computeSales python program

    Authors : Zacharioudakis Nikolas, Spyridakis Christos
    Created Date : 13/11/2019
    Last Updated : 27/11/2019
    Email : zaharioudakis@yahoo.com , spyridakischristos@gmail.com

    Description :   Read repeatedly receipts containing products, that consumers bought from different stores,
                    then print statistics for specific products or specific TINs (Tax Identification
                    Numbers) of a store owner based on user input.

    Requirements:
                    In order to read and accept each file, it has to be in a specific format, as described
                    below:

                    #################################### START EXAMPLE ####################################
                    -----------------------------------
                    ΑΦΜ : 123456789
                    ΟΝΟΜΑ_ΠΡΟΙΟΝ: ΠΟΣΟΤΗΤΑ ΤΙΜΗ_ΜΟΝΑΔΑΣ ΣΥΝΟΛΙΚΗ_ΤΙΜΗ

                    <ONE OR MORE PRODUCTS>

                    ΟΝΟΜΑ_ΠΡΟΙΟΝ: ΠΟΣΟΤΗΤΑ ΤΙΜΗ_ΜΟΝΑΔΑΣ ΣΥΝΟΛΙΚΗ_ΤΙΜΗ
                    ΣΥΝΟΛΟ: ΠΟΣΟ_ΜΕ_ΣΥΝΟΛΟ_ΑΠΟΔΕΙΞΗΣ
                    -----------------------------------

                    <ONE OR MORE RECEIPTS>

                    -----------------------------------
                    ΑΦΜ : 987654321
                    ΟΝΟΜΑ_ΠΡΟΙΟΝ: ΠΟΣΟΤΗΤΑ ΤΙΜΗ_ΜΟΝΑΔΑΣ ΣΥΝΟΛΙΚΗ_ΤΙΜΗ
                    ...
                    ΟΝΟΜΑ_ΠΡΟΙΟΝ: ΠΟΣΟΤΗΤΑ ΤΙΜΗ_ΜΟΝΑΔΑΣ ΣΥΝΟΛΙΚΗ_ΤΙΜΗ
                    ΣΥΝΟΛΟ: ΠΟΣΟ_ΜΕ_ΣΥΝΟΛΟ_ΑΠΟΔΕΙΞΗΣ
                    -----------------------------------
                    #################################### END EXAMPLE ####################################

        Notes:
                * Acceptable File:
                    - Has to exist, is regular file, is readable and is not empty (if one of them is violated
                    just continue and don't print anything - NOT EVEN '\n').

                    - Each file has to have at least one valid receipt, surrounded by lines containing only
                    dash symbol of unknown length.

                    - The number of lines and receipts that each file has, is not predefined. But program
                    should be able to parse files containing hundreds of thousands of them easily, fast
                    and without memory waste.

                * Acceptable Receipts:
                    - Must begin with a line containing only dash symbol.

                    - TIN (ΑΦΜ) has to be presented and be a 10 digit number.

                    - Each receipt has to have at least on product in it.

                    - The name of the product (ΟΝΟΜΑ_ΠΡΟΙΟΝ) must be a STRING. Quantity per product bought
                    (ΠΟΣΟΤΗΤΑ) must be an INTEGER number. Price of product per unit (ΤΙΜΗ_ΜΟΝΑΔΑΣ) and
                    total cost per product (ΣΥΝΟΛΙΚΗ_ΤΙΜΗ) could be FLOATING point numbers.

                    - ΣΥΝΟΛΙΚΗ_ΤΙΜΗ of a product with name ΟΝΟΜΑ_ΠΡΟΙΟΝ has to be equal with ΣΥΝΟΛΙΚΗ_ΤΙΜΗ =
                    ΠΟΣΟΤΗΤΑ * ΤΙΜΗ_ΜΟΝΑΔΑΣ in order to be accepted.

                    - Line spacing is unpredictable.

                    - Finally, total cost of all products per receipt (ΠΟΣΟ_ΜΕ_ΣΥΝΟΛΟ_ΑΠΟΔΕΙΞΗΣ) has to be
                    appeared in the last line of the receipt, be a FLOATING point number and equals with the
                    sum of all subtotals costs for each product (ΣΥΝΟΛΙΚΗ_ΤΙΜΗ).

                    - Must end with a line containing only dash symbol.

_______________________________________________________________________________________________________
'''

import argparse
import re
import sys
import time

AFM = '[0-9]{10}'
INT_NUM = '[1-9][0-9]*'
# allowed costs are integers and float up to 2 digits i.e. 10, 110.1, 1.10
COST = '(([1-9][0-9]*([.][0-9]{1,2})?)|([0-9]*([.][0-9]{1,2})))'

# REGEXES
AFM_PATTERN = '^\s*ΑΦΜ\s*:\s*' + AFM + '\s*$'
PRODUCT_PATTERN = '^\s*[^ ]+\s*:\s*' + INT_NUM + '\s+' + COST + '\s+' + COST + '\s*$'
SYNOLO_PATTERN = '^\s*ΣΥΝΟΛΟ\s*:\s*' + COST + '\s*$'
SPLIT_RECEIPT = '^[-]+$'

# Auxiliary variables
DEBUG = False
TIME = False
MEMORY = False
PRODUCTS = False
VALID = False
ERROR = False
ITEMS_DICTIONARY = False
UPDATE = False


def create_parser():
    """
        Just create a custom parser (using ArgumentParser) to parse debug and/or other flags during program
        development period

        :exception: if parser creation failed raise exception

        :return: parser that created
    """
    version = '2.0.1'

    description = 'Read repeatedly receipts containing products, that consumers bought from different stores,\n' + \
                  'then print statistics for specific products or specific TINs (Tax Identification Numbers)\n' + \
                  'of a store owner based on user input. Read program\'s inline documentation to find out input\n' + \
                  'file\'s needed format.'

    epilog = 'Implementation:\n' + \
             '  version: ' + version + '\n' + \
             '  authors: Spyridakis Christos, Zacharioudakis Nikolas \n' + \
             '  copyright (c) 2019 https://github.com/CSpyridakis/SDTSP,  https://github.com/sneakyPent/OsDev \n' + \
             '  license: MIT License'

    # Create parser, add arguments and return it, or raise exception if something is wrong
    try:
        program_name = sys.argv[0]
        version_message = sys.argv[0] + ' ' + version
        parser = argparse.ArgumentParser(prog=program_name, description=description, epilog=epilog,
                                         formatter_class=argparse.RawDescriptionHelpFormatter)
        parser.add_argument('-c', '--correct', dest="VALID", action="store_true",
                            help='display when a receipt is valid')
        parser.add_argument('-d', '--debug', dest="DEBUG", action="store_true", help='enable debug mode')
        parser.add_argument('-e', '--error', dest="ERROR", action="store_true", help='print only not valid cases')
        parser.add_argument('-p', '--products', dest="PRODUCTS", action="store_true",
                            help='in insertion show products info')
        parser.add_argument('-t', '--time', dest="TIME", action="store_true", help='display execution time per request')
        parser.add_argument('-m', '--memory', dest="MEMORY", action="store_true", help='display memory usage (TODO)')
        parser.add_argument('-i', '--items', dest="ITEMS_DICTIONARY", action="store_true",
                            help='print all items for each afm after file insertion')
        parser.add_argument('-u', '--update', dest="UPDATE", action="store_true",
                            help='show updated products\' dictionary after each product\'s insertion')
        parser.add_argument('-v', '--version', action='version', version=version_message)
        return parser
    except Exception:
        raise Exception("Something went wrong with the parser creation!")


def calculate_time(func):
    """
        Use this function as a decorator in order to print execution time when is needed
        :param func: function that decorates (you do not need to pass any argument)
        :return: actually nothing, just executed inner wrapper
    """

    def wrap(*args, **kwargs):
        start_timer = time.time()
        func(*args, **kwargs)
        stop_timer = time.time()
        t_time = round((stop_timer - start_timer), 10)

        if DEBUG or TIME:
            print('-' * 50 +
                  '\n{TIME} Function: ' + func.__name__ +
                  ' | Time: ' + t_time.__str__() + ' (sec)')

    return wrap


def update_afm_receipts(new_receipt, afm):
    for key in new_receipt[afm]["products"]:
        if key in receipts_dict[afm]["products"]:
            receipts_dict[afm]["products"][key] = \
                round((receipts_dict[afm]["products"][key] + new_receipt[afm]["products"][key]), 2)
        else:
            receipts_dict[afm]["products"][key] = new_receipt[afm]["products"][key]


def print_receipts():
    print('------------------ {RECEIPTS-DICTIONARY} ------------------')
    n = 1
    for key in sorted(receipts_dict):
        value = receipts_dict.get(key)
        print(str(n) + ') AFM: ' + str(key))
        prod = dict(receipts_dict[key]['products'])
        for k in sorted(prod.keys()):
            print(' - ' + str(k) + '\t' + str(prod[k]))
        n += 1
        if n <= len(receipts_dict): print('\n')


@calculate_time
def add_new_file(filename):
    """
         Reads one receipt of the file at a time.
         So if we want to read all receipts of the file we should parse
         it in a loop until EOF appears. For each dash line we try to see
         if a valid receipt exist, else we move on to the next line.

        :param filename: is the file contains the receipts, actually its path
        :return: None if we have EOF (update GLOBAL dictionary)
    """
    if DEBUG: print('#' * 30 + '  ADD NEW FILE  ' + '#' * 30)
    with open(filename, "r", encoding='utf-8') as file:
        # Starting point of new file parsing
        line_Number = 1
        ln = file.readline()  # Read first line
        DASH_FIRST = True
        while True:  # For FILE loop
            if not ln:  # If END of file return
                if DEBUG: print('#' * 30 + '  EOF  ' + '#' * 30)
                if ITEMS_DICTIONARY: print_receipts()
                return
            elif dash_pattern.match(ln):  # Dash line PATTERN match
                DASH_FIRST = False
                ln = file.readline().upper()  # Read next line (must be AFM)
                line_Number += 1
                if afm_pattern.match(ln) is None:  # AFM PATTERN check
                    if ln and (ERROR or DEBUG): print(
                        '-' * 50 + '\n{AFM-WRONG-PATTERN:' + str(line_Number) + '} - ' + str(ln).strip())
                    continue  # Wrong AFM PATTERN

                afm = str(ln.strip().split(":").__getitem__(1).strip())  # Save AFM
                if DEBUG: print('-' * 50 + '\n{AFM:' + str(line_Number) + '} - New Receipt, AFM: ' + str(afm))

                products = {}  # Initialize products dict
                p_total = 0.0  # Initialize receipt total

                # Starting point of new receipt parsing
                while True:  # For RECEIPT loop
                    ln = file.readline().upper()  # Read next line (PROD or TOT)
                    line_Number += 1
                    if product_pattern.match(ln) is not None:  # Product PATTERN check
                        name = str(ln.strip().split(":").__getitem__(0).strip())  # Save Product's name
                        spec = str(ln.strip().split(":").__getitem__(1))
                        amount = int(spec.strip().split().__getitem__(0).strip())  # Save Product's quantity
                        cost = float(spec.strip().split().__getitem__(1).strip())  # Save Product's cost per unit
                        total = float(spec.strip().split().__getitem__(2).strip())  # Save Product's total cost

                        # Checking if amount*cost per product equals product's total
                        if round((amount * cost), 2) != total:
                            if ERROR or DEBUG: print(
                                '{SUM-ERROR:' + str(line_Number) + '} - Total: ' + str(total) + ' != ' + ' (' + str(
                                    amount) + ' * ' + str(
                                    cost) + ') = ' + str(round((amount * cost), 2)))
                            break
                        if PRODUCTS: print(
                            '{PRODUCT-INFO:' + str(line_Number) + '} - Name: ' + str(name) + ' | Quantity: ' + str(
                                amount) + ' | Cost: ' + str(
                                cost) + ' | Sum: ' + str(total))

                        # Get the subtotal of the receipt products
                        p_total = round(p_total + total, 2)

                        # ------------------------------------
                        #   Save Product to temp dictionary
                        # ------------------------------------
                        # if the product already exists in the dictionary then add the info in the existent value
                        # else add new key-value in the dictionary
                        if name in products:
                            products[name] = round(products[name] + total, 2)
                        else:
                            products[name] = total
                        if UPDATE: print('-> {UPDATE PRODUCTS} - ' + str(products) + '\n')
                        continue
                    elif total_pattern.match(ln) is not None:  # Total PATTERN check
                        rec_total = float(ln.strip().split(":").__getitem__(1).strip())  # Save total cost
                        if not bool(products) or p_total != float(rec_total):  # Empty receipt or total != subtotal
                            if ERROR or DEBUG: print('{WRONG-TOTAL:' + str(line_Number) + '} - | Total: ' + str(
                                rec_total) + ' | Sub-Total: ' + str(p_total))
                            break
                        if DEBUG: print('{TOTAL:' + str(line_Number) + '} - Total: ' + str(rec_total))

                        ln = file.readline()  # Read next line
                        line_Number += 1
                        if dash_pattern.match(ln):  # Dash line
                            if VALID: print("{VALID-RECEIPT !!!}")

                            # ------------------------------------
                            #   Save Receipt to dictionary
                            # ------------------------------------
                            if afm in receipts_dict:
                                update_afm_receipts({afm: {"products": products}}, afm)
                            else:
                                receipts_dict.update({afm: {"products": products}})
                            break
                        else:
                            if ln and (ERROR or DEBUG): print('{LAST-DASH-LINE-ERROR:' + str(line_Number) + '}')
                            break
                    else:
                        if ln and (ERROR or DEBUG):
                            print('{PATTERN-ERROR:' + str(line_Number) + '} - ' + str(ln).strip())
                        elif ERROR or DEBUG:
                            print('{UNEXPECTED EOF:' + str(line_Number) + '}')
                        break
            # Read next line if this is not a dash line
            else:
                if not DASH_FIRST and (DEBUG or ERROR):
                    print('{DASH-LINE-ERROR:' + str(line_Number) + '}')
                DASH_FIRST = True
                ln = file.readline()
                line_Number += 1


@calculate_time
def give_statistics_for_product(prod):
    """
        Print statistics for a specific product

        Search the given product name for the sales has been
        done by every different "AFM". The output must be, one
        AFM (if it has at least one sale) per line and its sales.
        ("AFM Sales"). The list must be in ASCENDING ORDER by AFM.
    """
    for afm in sorted(receipts_dict.keys()):  
        if prod in receipts_dict[afm]["products"]:
            print(afm.__str__() + " " + str(format(receipts_dict[afm]["products"][prod], '.2f')))


@calculate_time
def give_statistics_for_afm(afm):
    """
        Print statistics for a specific AFM

        Find the sales have been done by the given AFM and prints
        every product with its sum, ("Name sum"). The results must
        be in ASCENDING ORDER by product name.
    """
    data = receipts_dict.get(str(afm))
    if data is None:
        return
    data = data["products"]
    for prods in sorted(data.keys()):
        print(str(prods) + " " + str(format(data[prods], '.2f')))


def print_menu():
    print("Give your preference: (", end='')
    print("1: read new input file, ", end='')
    print("2: print statistics for a specific product, ", end='')
    print("3: print statistics for a specific AFM, ", end='')
    print("4: exit the program", end='')
    print(")")


#
# ________________________________________________ MAIN ________________________________________________
#
if __name__ == '__main__':
    _parser = create_parser()
    _args = _parser.parse_args()

    # Debugging messages "Defines"
    DEBUG = _args.DEBUG
    TIME = _args.TIME
    MEMORY = _args.MEMORY
    PRODUCTS = _args.PRODUCTS
    VALID = _args.VALID
    ERROR = _args.ERROR
    ITEMS_DICTIONARY = _args.ITEMS_DICTIONARY
    UPDATE = _args.UPDATE

    if DEBUG: print(_args)
    afm_pattern = re.compile(AFM_PATTERN)
    product_pattern = re.compile(PRODUCT_PATTERN)
    total_pattern = re.compile(SYNOLO_PATTERN)
    dash_pattern = re.compile(SPLIT_RECEIPT)

    receipts_dict = {}
    while True:
        if DEBUG or MEMORY or TIME: print('_' * 100)
        print_menu()
        try:
            # Only 1, 2, 3 and 4 are valid -> 1.0, 2.00, two, 8, -1, +2, etc... are not!
            user = str(input())
        except Exception:
            user = 0

        if user == '1':
            f_name = str(input("Give File name: "))
            try:
                add_new_file(f_name)
            except Exception:
                pass
        elif user == '2':
            give_statistics_for_product(str(input("Give a product name: ")).upper())
        elif user == '3':
            afm = str(input("Give an AFM: "))
            if re.match(AFM, afm) is not None:
                give_statistics_for_afm(afm)
        elif user == '4':
            exit(0)
