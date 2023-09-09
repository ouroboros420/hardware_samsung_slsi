import sys
import os

latest_chipid = sys.argv[1]
new_chipid = sys.argv[2]

find_command = "find . -name \"*" + latest_chipid + "*\" > find_result"
print(find_command)
os.system(find_command)

f = open("find_result")

for line in f.readlines():
    new_chip_loc = line.replace(latest_chipid, new_chipid)
    copy_command = "cp -rf " + line[:len(line)-1] + " " + new_chip_loc
    os.system(copy_command)

clear_variant_comamnd = "find ./variant/erd" + new_chipid + " -name \"*" + latest_chipid + "*\" -exec rm -rf {} \\;"
os.system(clear_variant_comamnd)
