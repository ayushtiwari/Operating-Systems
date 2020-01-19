#! /bin/bash

if [ $# -ne 1 ]
then
  echo "Usage : fields [ column_number between 1 and 4 ]"
  exit 1
fi

if [ $1 -le 4 ] && [ $1 -ge 1 ]
then
   awk '{ print tolower($'$1') }' < 1e_input.txt | sort | uniq -c | awk '{ print $2, $1 }' | sort -k2 -nr > 1e_output.txt
else
  echo "Usage : fields [ column_number between 1 and 4 ]"
  exit 1
fi
