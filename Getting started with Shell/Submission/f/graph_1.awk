# awk file for amking clean graph

{
  if(!table[$1, $2] && !table[$2, $1] && $1 != $2) {
      print $1, $2
    table[$1, $2]=1
  }
}
