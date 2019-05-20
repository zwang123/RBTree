#!/bin/bash

python_script='
def strip_comment(line):
    idx = line.find("//")
    return line[:idx] if idx != -1 else line

from sys import argv, stderr
from re import sub
fname = argv[1]
try:
    keep = int(argv[2]) # if 0 then replace, else gen new file with ".new" suf
except IndexError: 
    keep = 0
try:
    verbose = int(argv[3]) # verbose ==> more output
except IndexError: 
    verbose = 0

try:
    # convert file name to the word to be defined
    def_word = fname[1:].replace("/", "__") # / ==> __, keep first "/"
    def_word = def_word.replace(".", "_")   # . ==> _
    def_word = def_word.replace(" ", "_")   # whitespace ==> _
    def_word = def_word.replace("\t", "_")
    
    # lowerUPPER ==> lower_UPPER
    def_word = sub(r"([a-z])([A-Z])", r"\1_\2", def_word) 
    def_word = def_word.upper()            # CAPITALIZE
    def_word = def_word + "_INCLUDED"      # SUFFIX
    #print("#ifndef " + def_word)
    
    with open(fname) as f:
        data = f.readlines()
    
    # replace head
    status = 3 # 0 for correct match, 1 for mismatch, 2 for not found
    legal_status = [0, 1, 2]
    for i, l in enumerate(data):
        spl = strip_comment(l).split()
        if len(spl) == 0:
            continue
        elif status == 0:
            assert(spl[0] == "#define" and spl[1] == def_word)
            break
        elif status == 1:
            assert(spl[0] == "#define" and spl[1] == old_word)
            data[i] = "#define " + def_word + "\n"
            break
        elif status in legal_status:
            print("Coding Error")
            raise RuntimeError
        elif spl[0] == "#ifndef":
            if spl[1] != def_word:
                old_word = spl[1]
                data[i] = "#ifndef " + def_word + "\n"
                status = 1
            else:
                status = 0
        else:
            status = 2
            break
    
    # empty file
    if status not in legal_status:
        print("WARNING! ", fname, " is empty!", file=stderr)
        status = 2
    
    assert(status in legal_status)
    
    # replace tail
    if status != 2:
        line_ptr = i
        found_endif = False
        for j in range(len(data) - 1, line_ptr, -1):
            spl = strip_comment(data[j]).split()
            if len(spl) == 0:
                continue
            assert(spl[0] == "#endif")
            if status == 1:
                data[j] = "#endif // " + def_word + "\n"
            found_endif = True
            break
        assert(found_endif)
    else:
        if (len(data) == 0):
            data = [""]
        data[0] = "#ifndef " + def_word + "\n#define " + def_word + "\n" \
                + data[0]
        if len(data[-1]) == 0 or data[-1][-1] != "\n":
            data[-1] += "\n"
        data[-1] += "#endif // " + def_word + "\n"
    
    # output info
    if verbose != 0:
        if status == 1:
            print("Header guard found but inconsistent. ",
                  "Replacing ", old_word, " with ", def_word)
        elif status == 2:
            print("Header guard not found. Adding word ", def_word)
    
    # output file
    if status != 0:
        oname = fname if keep == 0 else fname + ".new"
        with open(oname, "w") as f:
            f.writelines(data)

except:
    print("Error in file ", fname, file=stderr)
    raise
'

py='python3.5'

#add_header () {
#    fname="$1"
#    str=`echo "$fname" | cut -c2- | tr . _ | tr a-z A-Z | sed 's@/@__@g'`
#    echo "#ifndef ${str}_INCLUDED"
#    #sed -i '1i 
#}
#export -f add_header


#find . -path ./sample-os -prune -o \( -name '*.h' -o -name '*.hpp' \) -type f \
#    -exec "$py" -c "$python_script" '{}' 1 1 \;
find . -path ./obsolete -prune -o \
       -path ./test_header -prune -o \
       \( -name '*.h' -o -name '*.hpp' \) -type f \
       -exec "$py" -c "$python_script" '{}' 0 1 \;
