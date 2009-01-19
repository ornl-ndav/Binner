#!/bin/bash

#use: generate_report.sh logfile

TOP=..
logfile=$1

grep rebinproc $logfile | cut -f2 -d" " > t1
grep throughput $logfile | cut -f2 -d":" | cut -f2 -d" " > t2
grep time $logfile | cut -f2 -d":" | cut -f2 -d" " > t3
grep percentage $logfile | cut -f2 -d":" | cut -f2 -d" " | cut -f1 -d"%" > t4

echo "res     num_p/s sec     %non_empty"
paste t1 t2 t3 t4

#rm t1 t2 t3

exit 0


