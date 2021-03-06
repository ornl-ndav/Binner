#!/bin/bash

# use: gmesht2b.sh directory

if [ ! -e "$1" ] && [ ! -d "$1" ] # test if target directory exists
then
  echo "$1 is not a valid directory"
  exit 127 # error code 127
fi
 
echo -n "deleting all *.inb files ... "
rm -rf $1/*.inb
echo "done"

for f in $1/*.in
do
  echo -n "$f --> ${f}b: "
  gmesht2b < $f > $1/${f}.tmp
  return_val=$?

  if (( return_val > 0 ))
  then
    echo "failed"
  else
	gmeshorderv < $1/${f}.tmp > ${f}b
    echo "successful"
  fi

  rm -rf $1/${f}.tmp

done

exit 0


