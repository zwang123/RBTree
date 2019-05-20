#!/bin/bash
for i in *.cpp; do
    awk '{if (length($0)>80) print FILENAME, NR;}'  "$i"
done
for i in *.h; do
    awk '{if (length($0)>80) print FILENAME, NR;}'  "$i"
done
for i in *.hpp; do
    awk '{if (length($0)>80) print FILENAME, NR;}'  "$i"
done
for i in Makefile*; do
    awk '{if (length($0)>80) print FILENAME, NR;}'  "$i"
done
