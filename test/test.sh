#!/bin/bash
if [ -n "$1" ]; then
    make && ./test.out
else
    make 2>&1 | head
fi
