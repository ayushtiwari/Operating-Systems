#! /bin/zsh -

printf "Enter password : "
stty -echo
read pass < /dev/tty
printf "\n"
printf "Enter again : "
read pass2 < /dev/tty
printf "\n"
stty echo