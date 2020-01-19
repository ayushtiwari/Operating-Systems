#! /bin/bash

number='^[+-]?[0-9]+([.][0-9]+)?$'
operator='^[+-/*%]'

# Check for validity of input
if [ $# -ne 3 ] || ! [[ $1 =~ $number ]] || ! [[ $2 =~ $operator ]] || ! [[ $3 =~ $number ]]
then
  echo "Usage : ./calculator.sh integer op integer" >&2
  exit 1
fi

# Result using method 1
printf "Result (1st Method) = "

case $2 in
  "+") printf "%d\n" $(( $1 + $3 ))
       ;;
  "-") printf "%d\n" $(( $1 - $3 ))
       ;;
  "*") printf "%d\n" $(( $1 * $3 ))
       ;;
  "/") if [ $3 -eq 0 ]
       then
         printf "Error: Undefined\n" >&2
         exit 1
       else
         printf "%d\n" $(( $1 / $3 ))
       fi
       ;;
  "%") printf "%d\n" $(( $1 % $3 )) ;;
  *  ) printf "%s : Unknown option\n" $2 >&2
       exit 1
       ;;
esac

# Result using bc
printf "Result (2nd Method) = %s\n" $(bc <<< $1$2$3)
