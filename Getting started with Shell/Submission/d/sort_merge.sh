#! /bin/bash

# Create directory
if [ ! -e 1.d.files.out ]
then
  mkdir 1.d.files.out
fi

# Sort all files and save to newfile
echo "Sorting..."
for file in ./1.d.files/*
do
  newfile=$( printf "1.d.files.out/%s" ${file#./*/} )
  sort -nr -o $newfile $file
done

# Merge all the files
echo "Merging..."
sort -nr --batch-size=50 ./1.d.files.out/* > 1.d.out.txt
