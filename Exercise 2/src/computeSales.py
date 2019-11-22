'''
_______________________________________________________________________________________________________

    computeSales python program

    Authors : Zacharioudakis Nikolas, Spyridakis Christos
    Created Date : 13/11/2019
    Last Updated : 17/11/2019
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

                    [TODO] Each file has to have at least one valid receipt, surrounded by lines containing only
                    dash symbol of unknown length.

                    [TODO] The number of lines and receipts that each file has, is not predefined. But program
                    should be able to parse files containing hundreds of thousands of them easily, fast
                    and without memory waste.

                * Acceptable Receipts:
                    - Must begin with a line containing only dash symbol.

                    - TIN (ΑΦΜ) has to be presented and be a 10 digit number. Actually a 10 digit INTEGER
                     WITHOUT fractional part.

                    - Each receipt has to have at least on product in it.

                    - The name of the product (ΟΝΟΜΑ_ΠΡΟΙΟΝ) must be a STRING. Quantity per product bought
                    (ΠΟΣΟΤΗΤΑ) must be an INTEGER number. Price of product per unit (ΤΙΜΗ_ΜΟΝΑΔΑΣ) and
                    total cost per product (ΣΥΝΟΛΙΚΗ_ΤΙΜΗ) could be FLOATING point numbers.

                    - ΣΥΝΟΛΙΚΗ_ΤΙΜΗ of a product with name ΟΝΟΜΑ_ΠΡΟΙΟΝ has to be equal with ΣΥΝΟΛΙΚΗ_ΤΙΜΗ =
                    ΠΟΣΟΤΗΤΑ * ΤΙΜΗ_ΜΟΝΑΔΑΣ in order to be accepted.

                    - Line spacing is unpredictable.

                    - Finally, total cost of all products per receipt (ΠΟΣΟ_ΜΕ_ΣΥΝΟΛΟ_ΑΠΟΔΕΙΞΗΣ) has to be
                    appeared in the last line of the receipt be a FLOATING point number and equals with the
                    sum of all subtotals costs for each product (ΣΥΝΟΛΙΚΗ_ΤΙΜΗ).

_______________________________________________________________________________________________________
'''

# Auxiliary variables
DEBUG = False
TIME = False
MEMORY = False

# REGULAR EXPRESSIONS
INT_NUMBER='[1-9][0-9]*'
FLOAT_NUMBER='([0-9]+[\.,]?|[0-9]*[\.,][0-9]+)'                 # TODO: comma or full-stop separator
PRODUCT_NAME='[^\s\d\!\#\$\%\&\'\*\+\-\.\^\_\`\|\~\:]+'         # TODO: Re-check this

AFM_PATTERN = '^\s*ΑΦΜ\s*:\s*[0-9]{10,10}\s*$'                  # ΑΦΜ: <10_digit_number>   <- Multiple white spaces
PRODUCT_PATTERN = '^\s*' + PRODUCT_NAME + '\s*:\s*' + INT_NUMBER + '\s+' + FLOAT_NUMBER + '\s+' + FLOAT_NUMBER + '\s*$'
SYNOLO_PATTERN = '^\s*ΣΥΝΟΛΟ\s*:\s*' + FLOAT_NUMBER + '\s*$'



def create_parser():
    """
        Just create a custom parser (using ArgumentParser) to parse debug and/or other flags during program
        development period

        :exception: if parser creation failed raise exception

        :return: parser that created
    """
    version = '1.0.0'

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
        parser.add_argument('-d', '--debug', dest="DEBUG", action="store_true", help='enable debug mode')
        parser.add_argument('-t', '--time', dest="TIME", action="store_true", help='display execution time per request')
        parser.add_argument('-m', '--memory', dest="MEMORY", action="store_true", help='display memory usage')
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
        t_time = round_up(stop_timer - start_timer)
        iterations = 0  # TODO: fix it

        if DEBUG or TIME:
            print('-' * 50 +
                  '\n{TIME} Function: ' + func.__name__ +
                  ' | Time: ' + t_time.__str__() + ' (sec)' +
                  ' | Iterations: ' + str(iterations))

    return wrap


def get_size(obj, seen=None):
    """
    Recursively finds size of objects

    :author: this function was initially developed by bosswissam

    :see: https://gist.github.com/bosswissam/a369b7a31d9dcab46b4a034be7d263b2#file-pysize-py
    """
    size = sys.getsizeof(obj)
    if seen is None:
        seen = set()
    obj_id = id(obj)
    if obj_id in seen:
        return 0
    # Important mark as seen *before* entering recursion to gracefully handle
    # self-referential objects
    seen.add(obj_id)
    if isinstance(obj, dict):
        size += sum([get_size(v, seen) for v in obj.values()])
        size += sum([get_size(k, seen) for k in obj.keys()])
    elif hasattr(obj, '__dict__'):
        size += get_size(obj.__dict__, seen)
    elif hasattr(obj, '__iter__') and not isinstance(obj, (str, bytes, bytearray)):
        size += sum([get_size(i, seen) for i in obj])
    return size


def human_size(bytes, units=[' bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB']):
    return str(bytes) + units[0] if bytes < 1024 else human_size(bytes >> 10, units[1:])


def round_up(n, decimals=4):
    """
    round up number in k decimal places

    :param: time of execution or other real number

    :return: rounded up result using k decimal places (by default is 4)
    """
    multiplier = 10 ** decimals
    return math.ceil(n * multiplier) / multiplier


def update_afm_receipts(new_receipt, afm):
    for key in new_receipt[afm]["products"]:
        if key in receipts_dict[afm]["products"]:
            receipts_dict[afm]["products"][key]["amount"] += new_receipt[afm]["products"][key]["amount"]
            # TODO: check about cost per product
            receipts_dict[afm]["products"][key]["total"] = \
                round_up(receipts_dict[afm]["products"][key]["total"] + new_receipt[afm]["products"][key]["total"])
        else:
            receipts_dict[afm]["products"][key] = {'amount': new_receipt[afm]["products"][key]["amount"],
                                                   'cost': new_receipt[afm]["products"][key]["cost"],
                                                   'total': new_receipt[afm]["products"][key]["total"]}

    receipts_dict[afm]["total"] = round_up(receipts_dict[afm]["total"] + new_receipt[afm]["total"])

def product_data(ln):
    ln = ln.strip()
    name = str(ln.strip().split(":").__getitem__(0))
    spec = str(ln.strip().split(":").__getitem__(1))
    amount = int(spec.strip().split().__getitem__(0))
    cost = float(spec.strip().split().__getitem__(1))
    sum = float(spec.strip().split().__getitem__(2))

    if sum == round(float(amount * cost),2):
        if DEBUG: print('{PRODUCT-INFO} - Name:' + str(name) + ' | Quantity: ' + str(amount) + ' | Cost: ' + str(cost) + ' | Sum: ' + str(sum))
        return [name, amount, cost, sum]
    else:
        if DEBUG: print('{NOT-VALID-PRODUCT} ' + '- Product name: ' + str(name) + ' | Quantity: ' + str(amount) + ' | Cost: ' + str(cost) + ' | Sum: ' + str(sum) + '  | Total: ' + str(float(amount * cost)) + '  | ~ [' + str(ln) + ']')
        return

def read_receipt(file):
    """
     reads only one receipt of the file at a time.
     Its return condition is the last line of the receipt "ΣΥΝΟΛΟ".
     So if we want to read all receipts of the file we should call 'read_receipt()' in a loop until
     returns 'end'.

    :param file: is the file contains the receipts
    :return: a receipt dictionary or 'end' if we have EOF
    """
    products = {}
    subtotal = 0.0

    try:
        # Read AFM
        ln = file.readline()
        if not ln: return
        elif afm_pattern.match(ln) is None:
            if DEBUG: print('{AFM-WRONG-PATTERN} - ' + str(ln).strip())
            return
        afm = str(ln.upper().strip().split(":").__getitem__(1))
        if DEBUG: print('-' * 50 + '\n{AFM} - New Receipt, AFM: ' + afm)

        # Read first product
        ln = file.readline()
        if not ln: return
        elif product_pattern.match(ln) is None:
            if DEBUG: print('{PRODUCT-WRONG-PATTERN} - ' + str(ln).strip())
            return
        product = product_data(ln.upper())
        if product is None: return
        [name, amount, cost, sum] = list(product)

        subtotal += sum
        # Add first product
        products[name] = {'amount': amount, 'cost': cost, 'total': sum}

        while True:
            l = file.readline()
            if not l: return

            if product_pattern.match(l) is None and total_pattern.match(l) is None:
                if DEBUG: print('{PATTERN-ERROR} - ' + str(l).strip())
                return
            elif product_pattern.match(l):
                product = product_data(l.upper())
                if product is None: return
                [name, amount, cost, sum] = list(product)
                subtotal += sum

                # Add product
                if name in products:
                    products[name]["amount"] += amount
                    products[name]["cost"] = cost                       # TODO: check about cost per product
                    products[name]["total"] = round_up(products[name]["total"] + sum)
                else:
                    products[name] = {'amount': amount, 'cost': cost, 'total': sum}
            elif total_pattern.match(l):
                total = l.upper().strip().split(":").__getitem__(1)
                if float(total) == round(float(subtotal), 2):
                    if DEBUG: print('{TOTAL} - Total: ' + str(total))
                    # l = file.readline()
                    # if not l: return
                    # if set(l.strip()).issubset("-"):
                    return {afm: {"products": products, "total": float(total)}}         # TODO: go back one line
                else:
                    if DEBUG: print('{WRONG-TOTAL} - Total: ' + str(total))
                return
    except ImportError:
        pass
        #TODO: Import error case

    #
    #     # if the product already exists in the dictionary then add the info in the existent value
    #     # else add new key-value in the dictionary



@calculate_time
def add_new_file(filename):
    """
    Insert receipt(s) in the system.

    When user presses 1, the file name is being ask.
    If the file name is valid, the data is being appended
    to the previously inserted file data.

    :param: filename, the path of the desired file to add for next calculations

    """
    FIRST = True
    try:
        with open(filename, "r", encoding='utf-8') as file:
            while True:
                l = file.readline()
                if not l: break
                elif set(l.strip()).issubset("-"):
                    FIRST = True
                    new_receipt = read_receipt(file)
                    if new_receipt is None: continue        # If receipt is not valid then continue to rest of file

                    afm = list(new_receipt.keys())[0]
                    if afm in receipts_dict:
                        update_afm_receipts(new_receipt, afm)
                    else:
                        receipts_dict.update(new_receipt)
                else:
                    if DEBUG and FIRST: print('-' * 50 + "\n{DASH-LINE-ERROR}")
                    FIRST = False

    except Exception:
        if DEBUG: print("{ADDNEWFILE} - There is something wrong with that file (I can not open it!)")
    return


@calculate_time
def give_statistics_for_product(prod):
    """
    Print statistics for a specific product

    Search the given product name for the sales has been
    done by every different "AFM". The output must be, one
    AFM (if it has at least one sale) per line and its sales.
    ("AFM Sales"). The list must be in ASCENDING ORDER by AFM.

    """
    for afm in sorted(receipts_dict.keys()):  # for name, age in dictionary.iteritems():  (for Python 2.x)
        if prod in receipts_dict[afm]["products"]:
            print(afm.__str__() + " " + receipts_dict[afm]["products"][prod]["total"].__str__())


# TODO: check validity of AFM (10 digits)
@calculate_time
def give_statistics_for_afm(afm):
    """
    Print statistics for a specific AFM

    Find the sales have been done by the given AFM and prints
    every product with its sum, ("Name sum"). The results must
    be in ASCENDING ORDER by product name.

    """
    data = receipts_dict.get(eval(afm), 'None')

    data = data["products"]
    for prods in sorted(data.keys()):
        print(prods + " " + data[prods]["amount"].__str__())


#
# ________________________________________________ MAIN ________________________________________________
#
if __name__ == '__main__':
    menu = 'Give your preference: (1: read new input file, 2: print statistics for a specific product, ' + \
           '3: print statistics for a specific AFM, 4: exit the program)\n'

    # Read input arguments. In case of error, assume that there are not input flags and just execute
    # program as needed.
    try:
        import argparse, sys, time, math  # For DEBUG, TIME and MEMORY usage

        _parser = create_parser()
        _args = _parser.parse_args()

        # Debugging messages "Defines"
        DEBUG = _args.DEBUG
        TIME = _args.TIME
        MEMORY = _args.MEMORY

        if DEBUG: print(_args)

        import re  # For regular expressions
        afm_pattern = re.compile(AFM_PATTERN)
        product_pattern = re.compile(PRODUCT_PATTERN)
        total_pattern = re.compile(SYNOLO_PATTERN)
        REGS = True
    except Exception as e:
        # print(e)
        pass

    # Initialize an empty dictionary for receipts, then give menu until exit option is selected
    receipts_dict = {}
    while True:
        if DEBUG or MEMORY or TIME: print('_' * 100)
        try:
            user = str(input(menu))  # Only 1, 2, 3 and 4 are valid -> 1.0, 2.00, two, 8, -1, +2, etc... are not!
        except Exception:
            user = 0

        # Menu options
        if user == '1':
            add_new_file(input("Give File name: "))
            if DEBUG or MEMORY:
                print('{MEMORY} Memory size of dictionary: ' + human_size(10000))  # TODO: execute for dictionary
        elif user == '2':
            give_statistics_for_product(input("Give a Product name: "))
        elif user == '3':
            give_statistics_for_afm(input("Give an AFM: "))
        elif user == '4':
            # print("Goodbye")
            exit(0)
