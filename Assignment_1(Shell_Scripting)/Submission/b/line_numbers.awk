BEGIN {
  print "Serial, RandomString"
}

{
  print NR ", " $1
}
