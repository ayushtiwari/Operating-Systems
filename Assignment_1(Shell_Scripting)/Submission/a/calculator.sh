#! /bin/bash

if [ $# -ne 3 ]
then
  echo "Usage : ./calculator.sh x op y" >&2
  exit 1
fi

case $2 in
  "+") echo $(( $1 + $3 ))
       ;;
  "-") echo $(( $1 - $3 ))
       ;;
  "*") echo $(( $1 * $3 ))
       ;;
  "/") if [ $3 -eq 0 ]
       then
         echo Error: Undefined >&2
         exit 1
       else
         echo $(( $1 / $3 ))
       fi
       ;;
  "%") echo $(( $1 % $3 )) ;;
  *  ) echo $2: Unknown option >&2
       exit 1
       ;;
esac

printf "Result_2 = %s\n" $(bc <<< $1$2$3)
