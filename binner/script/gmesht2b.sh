#!/bin/bash

# use: gmesht2b.sh directory

if [ ! -e "$1" ] && [ ! -d "$1" ] # test if target directory exists
then
  echo "$1 is not a valid directory"
  exit 127 # error code 127
fi
 
rm -rf $1/*.inb

for f in $1/*.in
do
  echo -n "$f --> ${f}b: "
  ../bin/gmesht2b < $f > ${f}b
  return_val=$?

  if (( return_val > 0 ))
  then
    echo "failed"
    rm -rf ${f}b
  else
    echo "successful"
  fi
done

exit 0


