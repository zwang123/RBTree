#!/bin/bash
if [ -n "$1" ]; then
    make && valgrind --leak-check=yes ./test.out && \
    valgrind -v --leak-check=yes ./test.out 2 && \
    ./test.out 3
else
    make 2>&1 | head -n20
fi
