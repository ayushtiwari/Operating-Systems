#! /bin/bash

# GCD of two numbers
binary_gcd() {
  if [ $# -ne 2 ]
  then
    echo "Usage : binary_gcd x y"
    exit 1
  fi

  if [ $2 -eq 0 ]
  then
    return $1
  else
    binary_gcd $2 $(( $1 % $2 ))
    return $?
  fi
}

# Validity check
if [ $# -le 10 ] && [ $# -ge 2 ]
then
  GCD=$1
  shift

  # GCD of multiple numbers
  while [ $# -gt 0 ]
  do
    binary_gcd GCD $1
    GCD=$?
    shift
  done

  echo GCD = $GCD
  exit 0
else
  echo "Usage : gcd [ 2 to 10 integers space seperated integers ]"
  exit 1
fi
