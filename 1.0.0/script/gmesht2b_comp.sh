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

for f in $1/*.in.gz
do
  fh=${f%.*}
  echo -n "$f --> ${fh}b: "
  zcat ${f} | gmesht2b > ${f}.tmp
  #gmesht2b < $f > ${f}.tmp
  return_val=$?

  if (( return_val > 0 ))
  then
    echo "failed"
  else
	gmeshorderv < ${f}.tmp > ${fh}b
    echo "successful"
  fi

  rm -rf ${f}.tmp

done

exit 0


