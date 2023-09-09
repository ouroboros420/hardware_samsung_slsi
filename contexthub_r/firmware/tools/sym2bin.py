#!/usr/bin/env python3

import os.path
import re
import sys
from struct import *

def extract_symbol_info(inputfile, outputfile):
    result = None
    p = re.compile(r'([0-9a-f]{8}).{9}[.]text\s([0-9a-f]{8})\s([a-zA-Z_][a-zA-Z0-9_]*)')

    with open(inputfile, "r") as f:
        data = f.read()
        result = p.finditer(data)

    with open(outputfile, "wb") as f:
        count = 0
        pack_size = 0
        offset = 0
        pack_addrsize = bytearray()
        pack_symbol = bytearray()
        for r in result:
            #if int(r.group(2), 16) is not 0:
            count += 1
            str_size = len(r.group(3).encode('utf-8'))
            pack_symbol = pack_symbol + pack('%ds' % (str_size), r.group(3).encode('utf-8'))
            pack_addrsize = pack_addrsize + pack('IIII', int(r.group(1), 16), int(r.group(2), 16), offset, str_size)
            offset = offset + str_size


        pack_size = calcsize('IIII') * count
        pack_size += calcsize('IIII')

        f.write(pack('IIII', pack_size , count, offset, 0))
        f.write(pack_addrsize)
        f.write(pack_symbol)

        print("count is {}, tablesize {}, str offset {}".format(count, pack_size, offset))
    return

if __name__ == "__main__":
    print("input {}, output {}".format(sys.argv[1], sys.argv[2]))
    if (os.path.exists(sys.argv[1]) == False):
        print("file does not exists : {} ".format(sys.argv[1]))
        exit()
    if os.path.isfile(sys.argv[2]):
        print("delete file : {} ...".format(sys.argv[2]))
        os.remove(sys.argv[2])

    extract_symbol_info(sys.argv[1], sys.argv[2])
    symbolsize = os.path.getsize(sys.argv[2])
    if (symbolsize % 4 != 0):
        padsize = 4 - (symbolsize % 4)
        f = open(sys.argv[2], 'ab')
        f.seek(0, 2)

        for i in range (0, padsize):
            f.write(b'\x00')
        f.close
        symbolsize = symbolsize + padsize

    magic = 0x626d7973

    f = open(sys.argv[2], "ab")
    f.seek(0, 2)
    f.write((symbolsize).to_bytes(4, byteorder='little', signed=False))
    f.write((magic).to_bytes(4, byteorder='little', signed=False))
    f.close()
