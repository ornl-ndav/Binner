#!/bin/bash

TOP=..
rebinproc="${TOP}/bin/netrebin"
from="${TOP}/bin/getfrom"
to="${TOP}/bin/giveto"

count=0

while :
do
    #$from $nd $sp | $rebinproc 0.1 | $to $nd $cp 
	$from $1 $2 | $rebinproc 0.04 | $to $1 $3 
	if [ $? -ne 0 ];then
	   break
	fi
	((count++))
done

echo "done with $count tasks"

exit 0


