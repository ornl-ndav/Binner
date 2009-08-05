#!/bin/bash

#rebinall.sh 10 , i.e. want 10 parallel threads

TOP=..
count=0;
nd=`hostname` # node
echo "machinename $nd"

sorterproc="${TOP}/bin/sorter"
rebinproc="${TOP}/bin/netrebin"
collectorproc="${TOP}/bin/collector"
from="${TOP}/bin/getfrom"
to="${TOP}/bin/giveto"

getport()
{
	# wait for $pnumfile to be created
	while [ ! -e $1 ] || [ ! -s $1 ]
	do
	:
	done

	# read from $pnumfile and then remove it
	read $2 < $1
	rm $1
}

pnumfile=sorterport.num
${sorterproc} $nd < ${TOP}/tests/v750a &
getport $pnumfile sp

pnumfile=collectorport.num
${collectorproc} 300 300 300 $nd &
getport $pnumfile cp
#echo "sp $sp cp $cp"

#
# main task starts here
# by now, both sorter and collector should be up. let's run netrebin
#
#
function start_child() 
{
  ${TOP}/script/rebinone.sh $nd $sp $cp &
}

for ((i=1; i <= $1; i++))
do
   start_child;
done

wait
echo "rebinall exiting "

exit 0


