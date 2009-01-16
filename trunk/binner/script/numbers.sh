#!/bin/bash

# use: numbers.sh 10 100 2
#      this generate all numbers between 10 and 100 use an incremental of 2

for (( i = $1; i <= $2; i = $i + $3))
do
	if [ $i -lt 10 ]; then
       echo -n 000$i " "
	   continue
	fi
	
	if [ $i -lt 100 ]; then
	   echo -n 00$i " "
	   continue
	fi
	
	if [ $i -lt 1000 ]; then
	   echo -n 0$i " "
	   continue
	fi

	echo -n $i " "
done

echo 
exit 0


