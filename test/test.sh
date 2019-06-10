#!/bin/bash
if [ -n "$1" ]; then
    make && valgrind --leak-check=yes ./test.out
else
    make 2>&1 | head -n20
fi
