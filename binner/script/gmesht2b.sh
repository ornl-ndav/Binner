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
  /SNS/users/jhm/binner/bin/gmesht2b < $f > ${f}b
  return_val=$?

  if (( return_val > 0 ))
  then
    echo "$f --> ${f}b: failed"
    rm -rf ${f}b
  else
    echo "$f --> ${f}b: successful"
  fi
done

exit 0


