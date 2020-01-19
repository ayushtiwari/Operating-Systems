{
  table_2[$1]++
  table_2[$2]++
}

END {
  for(key in table_2)
    print key " " table_2[key]
}
