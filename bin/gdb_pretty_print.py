#  Copyright (C) 2025 AlgoRND
#
#  License: GPL
# helper script for gdb to print algo::aryptr, algo::strptr, algo::cstring, algo::Smallstr

# to invoke add this to your .gdbinit file:python
# import gdb_pretty_print
# end

# Cannot be run outside of gdb python environment
import gdb
import gdb.printing

# Debug print to confirm loading
print("Loading aryptr_printer.py")

# convert to horizontal display
def display(n_elems, elems):
    if n_elems == 0:
        return "len=0:"
    if n_elems < 0:
        try:
            return "len=" + str(int(n_elems)) + "(garbage):"
        except Exception as e:
            return "len=unknown(garbage):"  # Fallback in case of error
    if not elems:
        return "nullptr"

    # protect against garbage values
    lim=400
    if n_elems > lim:
        try:
            len_caption= str(n_elems) + " trunc at " + str(lim)
        except Exception as e:
            len_caption="len=unknown(garbage):"  # Fallback in case of error
        n_elems = lim
    else:
        len_caption= str(n_elems)

    try:
        # Read exactly n_elems characters as raw bytes
        chars = []
        for i in range(n_elems):
            # Read 1 byte from memory
            mem = gdb.selected_inferior().read_memory(elems + i, 1)
            byte = ord(mem.tobytes())  # convert to int
            if 32 <= byte < 127:
                chars.append(mem.tobytes().decode('utf-8', errors='ignore'))
            else:
                chars.append('.')

        tmp_str= ''.join(chars)
        return "len="+len_caption+":"+tmp_str
    except Exception as e:
        return str(e)

# ------
class AryPtrPrinter:
    def __init__(self, val):
        self.val = val
    def to_string(self):
        return display(int(self.val['n_elems']),self.val['elems'])
    def display_hint(self):
        return 'string'

def AryPtrPrinter_build():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("aryptr")
    # Match algo::aryptr<char> and algo::strptr
    pp.add_printer('aryptr', '^algo::aryptr<char>$|^algo::strptr$', AryPtrPrinter)
    return pp
# ------
class CstringPrinter:
    def __init__(self, val):
        self.val = val
    def to_string(self):
        return display(int(self.val['ch_n']), self.val['ch_elems'])
    def display_hint(self):
        return 'string'

def CstringPrinter_build():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("cstring")
    # Match algo::strptr and algo::cstring
    pp.add_printer('cstring', '^algo::cstring$|^algo::tempstr$', CstringPrinter)
    return pp
# ------
class SmallstrPrinter:
    def __init__(self, val):
        self.val = val
    def to_string(self):
        # unclear why int() is needed
        return display(int(self.val['n_ch']), int(self.val['ch'].address))
    def display_hint(self):
        return 'string'

def SmallstrPrinter_build():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("smallstr")
    # Match algo::Smallstr*
    pp.add_printer('smallstr','.*Smallstr.*', SmallstrPrinter)
    return pp


# Register the pretty printers
gdb.printing.register_pretty_printer(None, CstringPrinter_build(), replace=True)
gdb.printing.register_pretty_printer(None, AryPtrPrinter_build(), replace=True)
gdb.printing.register_pretty_printer(None, SmallstrPrinter_build(), replace=True)
