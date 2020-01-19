#! /bin/bash

# Create a temporary file
tmpfile=$(mktemp ./temp.$$)

awk -f "line_numbers.awk" < 1b_input.txt > $tmpfile
cat $tmpfile > 1b_input.txt

rm $tmpfile
