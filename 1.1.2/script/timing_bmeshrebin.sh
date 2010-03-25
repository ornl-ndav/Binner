#!/bin/bash

# use: timgin_bmeshrebin.sh minres maxres incr testin

TOP=..
rebinproc="${TOP}/bin/bmeshrebin"
logfile="${TOP}/log/bmeshrebin_timing_`hostname`.log"

echo `date` >> $logfile
echo `hostname` >> $logfile

for ((i = $1; i <= $2; i += $3))
do
   echo rebinproc $i >> $logfile
   $rebinproc $i < $4 2>>$logfile
done

echo >> $logfile
exit 0


