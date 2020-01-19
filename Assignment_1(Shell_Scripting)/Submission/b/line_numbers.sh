#! /bin/bash

tmpfile=$(mktemp ./temp.$$)
inputfile="input.txt"

printf "Serial, RandomString\n" > $tmpfile
nl -s', ' $inputfile >> $tmpfile
cat $tmpfile > $inputfile

rm $tmpfile
