#!/bin/bash

#numbers.sh 10 100, i.e. print all numbers between 10 and 100

for (( i = $1; i <= $2; i++))
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


